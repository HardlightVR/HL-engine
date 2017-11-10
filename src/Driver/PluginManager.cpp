#include "stdafx.h"
#include "PluginManager.h"
#include <iostream>
#include <bitset>
#include "DeviceContainer.h"
#include "HardwareEventDispatcher.h"
#include <boost/filesystem.hpp>
#include "logger.h"
#include <cassert>
PluginManager::PluginManager(boost::asio::io_service& io)
	: m_plugins()
	, m_deviceContainer(nullptr)
	, m_io(io)
	, m_pluginEventLoop(io, boost::posix_time::millisec(16))
	, m_pluginInfo()
	, m_fatalErrorHandler([]() {})
	, m_virtualDevices()
{
	m_pluginEventLoop.SetEvent([this]() {

		if (!this->TickOnce(16)) {//todo: should store 16 as variable
			m_fatalErrorHandler();
		}
	});

	m_pluginEventLoop.Start();


}

std::vector<boost::filesystem::path> findManifests(const boost::filesystem::path& root) {
	namespace fs = boost::filesystem;

	std::vector<fs::path> paths;

	std::unordered_map<std::string, std::vector<fs::directory_entry>> directoryManifestMap;
	try {
		fs::directory_iterator it(root);
		fs::directory_iterator endit;

		while (it != endit) {
			if (fs::is_directory(*it)) {
				std::vector<fs::directory_entry> dirs;

				std::copy(fs::directory_iterator(*it), fs::directory_iterator(), std::back_inserter(dirs));
				directoryManifestMap[it->path().string()] = dirs;

			}
			++it;
		}
	}
	catch (const boost::filesystem::filesystem_error& ec) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_error) << "[PluginDiscovery] " << ec.what();
	}


	for (const auto& dir : directoryManifestMap) {
		for (const auto& entry : dir.second) {
			if (fs::is_regular_file(entry) && Parsing::IsProbablyManifest(entry.path().string())) {
				paths.push_back(entry.path());
			}
		}
	}
	return paths;
}

void PluginManager::Discover()
{
	namespace fs = boost::filesystem;

	std::vector<fs::path> paths = findManifests(fs::current_path() / "plugins");

	BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "[PluginManager] " << " Found " << paths.size() << " plugin manifests";
	for (const auto& manifest : paths) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) <<  "--> " << manifest;
	}

	for (const auto& manifest : paths) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "[PluginManager] " << " Attempting to parse " << manifest.stem().string();
		if (auto config = Parsing::ParseConfig(manifest.string())) {
			std::string stem = manifest.stem().string();
			std::string dllName = stem.substr(0, stem.find_last_of('_'));
			m_pluginInfo[dllName].Descriptor = *config;
			m_pluginInfo[dllName].DllPath = manifest.parent_path().string();
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "--> Success. Assuming dll name is " << dllName;
		}
		else {
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "--> Failure";

		}
	}
}

void PluginManager::LoadAll()
{
	for (const auto& nameManifestPair : m_pluginInfo) {
		LoadPlugin(nameManifestPair.second.DllPath, nameManifestPair.first);
	}
}


void PluginManager::UnloadAll()
{
	destroyAll();
}

PluginInstance * PluginManager::MakeVirtualPlugin()
{
	assert(m_deviceContainer);

	auto size = m_plugins.size();
	std::string pluginName = "VirtualDeviceHost" + std::to_string(size);
	auto plugin = std::make_unique<PluginInstance>(m_io, pluginName, size);
	auto dispatcher = std::make_unique<HardwareEventDispatcher>(m_io);
	dispatcher->OnDeviceConnected([name = pluginName, this](nsvr_device_id id, PluginInstance* plugin) {
		m_deviceContainer->AddDevice(id, plugin->apis(),  name, plugin->resources());
	});
	plugin->setDispatcher(std::move(dispatcher));


	m_plugins[pluginName] = std::move(plugin);
	return m_plugins.at(pluginName).get();
}

bool PluginManager::TickOnce(uint64_t dt)
{
	std::vector<std::string> crashedPlugins;
	for (auto& plugin : m_plugins) {
		//if return value is false, we should destroy and reinstantiate the plugin
		//taking care to clean up the DeviceContainer somehow
		
		if (!plugin.second->tick_once(dt)) {
			crashedPlugins.push_back(plugin.first);
		}
	
	}

	for (auto& crashedPlugin : crashedPlugins) {
		if (m_plugins[crashedPlugin]->Unload()) {
			m_plugins.erase(crashedPlugin);
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "[PluginManager] Successfully unloaded misbehaving plugin " << crashedPlugin;
		}
		else {
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "[PluginManager] Unable to unload misbehaving plugin " << crashedPlugin;
			return false;

		}

	}

	return true;
}

bool PluginManager::Reload(const std::string & name)
{
	//Todo: this must be rewritten if we want this functionality

	/*std::vector<DeviceId<local>> toBeRemoved;
	m_deviceContainer.EachDevice([&name, &toBeRemoved](Device* device) {
		if (device->parentPlugin() == name) {
			toBeRemoved.push_back(device->id());
		}
	});

	for (nsvr_device_id id : toBeRemoved) {
		m_deviceContainer.RemoveDevice(id);
	}

	if (m_plugins[name]->Unload()) {
		return m_plugins[name]->Instantiate();
	}*/

	return false;

}

void PluginManager::OnFatalError(std::function<void()> handler)
{
	m_fatalErrorHandler = handler;
}




std::vector<uint32_t> PluginManager::GetPluginIds() const
{
	std::vector<uint32_t> pluginIds;
	for (const auto& kvp : m_plugins) {
		//Don't bother returning virtual device hosts
		if (kvp.first.find("VirtualDeviceHost") != std::string::npos) {
			continue;
		}
		pluginIds.push_back(kvp.second->GetId());
	}
	return pluginIds;
}

void PluginManager::DrawDiagnostics(uint32_t id, nsvr_diagnostics_ui* ui)
{
	for (auto& kvp : m_plugins) {
		if (kvp.second->GetId() == id) {
			diagnostics_api* api = kvp.second->apis().GetApi<diagnostics_api>();
			if (api) {
				api->submit_updatemenu(ui);
			}

			const auto& vdevices = m_pluginInfo.at(kvp.first).Descriptor.vdevices.devices;
			ui->keyval("Virtual devices available", std::to_string(vdevices.size()).c_str());
			for (const auto& device : vdevices) {
				if (ui->button(device.name.c_str())) {
					m_virtualDevices.push_back(instantiateVirtualDevice(kvp.first, device));
				}
			}

			if (ui->button("Remove all virtual devices")) {
				for (const auto& device : m_virtualDevices) {
					m_deviceContainer->RemoveDevice(device.first, device.second);
				}

				m_virtualDevices.clear();
			}
		}
	}
}

boost::optional<std::pair<std::string, PluginManager::PluginInfo>> PluginManager::GetPluginInfo(uint32_t id)
{
	boost::optional<std::string> foundPlugin;
	for (auto& kvp : m_plugins) {
		if (kvp.second->GetId() == id) {
			foundPlugin = kvp.first;
		}
	}

	if (foundPlugin) {
		return std::make_pair(*foundPlugin, m_pluginInfo[*foundPlugin]);
	}

	return boost::none;
}

void PluginManager::SetDeviceContainer(DeviceContainer* devices)
{
	m_deviceContainer = devices;
}

std::unique_ptr<PluginInstance> PluginManager::linkPlugin(const std::string& filename) {


	//auto dispatcher = std::make_unique<HardwareEventDispatcher>(m_io);
	//dispatcher->OnDeviceConnected([this](nsvr_device_id id, PluginApis& apis, const std::string& pluginName) {
	//	m_deviceContainer.AddDevice(id, apis, m_pluginInfo.at(pluginName).Descriptor.bodygraph, pluginName);
	//});

	//dispatcher->OnDeviceDisconnected([this](nsvr_device_id id) {
	//	m_deviceContainer.RemoveDevice(id);
	//}); 

	auto instance = std::make_unique<PluginInstance>(m_io, filename, m_plugins.size());
	
	if (instance->Link()) {
		return instance;
	}
	else {
		return std::unique_ptr<PluginInstance>();
	}
	
}
bool PluginManager::instantiatePlugin(PluginInstance* plugin)
{
	return plugin->Instantiate();
}

bool PluginManager::configurePlugin(PluginInstance* plugin)
{

	if (!plugin->Configure()) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_warning) << "Couldn't configure " << plugin->GetFileName();
		return false;
	}

	return true;
}

bool PluginManager::LoadPlugin(const std::string& searchDirectory, const std::string& dllName)
{

	auto plugin = linkPlugin((boost::filesystem::path(searchDirectory) / dllName).string());
	if (plugin) {
	
		m_plugins[dllName] = std::move(plugin);


		auto dispatcher = std::make_unique<HardwareEventDispatcher>(m_io);
		
		dispatcher->OnDeviceConnected([this, dll = dllName](nsvr_device_id id, PluginInstance* plugin) {
			assert(m_deviceContainer);
			plugin->resources()->bodygraphDescriptor = m_pluginInfo.at(dll).Descriptor.bodygraph;

			m_deviceContainer->AddDevice(id, plugin->apis(), dll, plugin->resources());
		});
		dispatcher->OnDeviceDisconnected([this, dll = dllName](nsvr_device_id id) {
			assert(m_deviceContainer);


			m_deviceContainer->RemoveDevice(DeviceId<local>{id}, dll);
		}); 

		m_plugins[dllName]->setDispatcher(std::move(dispatcher));

	}
	else {
		return false;
	}

	if (!instantiatePlugin(m_plugins[dllName].get())) {
		return false;
	}

	if (!configurePlugin(m_plugins[dllName].get())) {
		return false;
	}

	return true;
}


void PluginManager::destroyAll()
{
	for (auto& plugin : m_plugins) {
		if (!plugin.second->Unload()) {
			BOOST_LOG_SEV(clogger::get(), nsvr_severity_error) << "[PluginManager] Warning: unable to destroy " << plugin.first;
		}
	}

}

std::pair<DeviceId<local>, std::string> PluginManager::instantiateVirtualDevice(const std::string& plugin, const Parsing::VirtualDeviceDescriptor& device)
{
	const auto& originPlugin = m_pluginInfo.at(plugin);

	auto resources = std::make_unique<PluginInstance::DeviceResources>();
	resources->bodygraphDescriptor = originPlugin.Descriptor.bodygraph;
	//for now we use device ID 0. Probably want to pass in a specific ID.
	resources->deviceDescriptor = DeviceDescriptor{ device.name, 0, device.concept };

	std::vector<DefaultBodygraph::association> assocs;

	std::vector<Node> nodes;
	for (const auto& node : device.nodes) {
		nodes.emplace_back(NodeDescriptor{ node.concept, node.name, node.id });
		for (const auto& region : node.regions) {
			assocs.push_back(DefaultBodygraph::association{ region, node.id });
		}
	}
	resources->discoverer = std::make_unique<DefaultNodeDiscoverer>(nodes);
	resources->id = DeviceId<local>(0);
	resources->waveformHaptics = std::make_unique<DefaultWaveform>();
	resources->bufferedHaptics = std::make_unique<DefaultBuffered>();


	resources->bodygraph = std::make_unique<DefaultBodygraph>(assocs);

	PluginInstance* virtualHost = this->MakeVirtualPlugin();
	virtualHost->addDeviceResources(std::move(resources));

	return std::make_pair(DeviceId<local>{0}, virtualHost->GetFileName());

}

