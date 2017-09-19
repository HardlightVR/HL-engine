#include "stdafx.h"
#include "PluginManager.h"
#include <iostream>
#include <bitset>
#include "DeviceContainer.h"
#include "HardwareEventDispatcher.h"
#include <boost/filesystem.hpp>
#include "logger.h"
PluginManager::PluginManager(boost::asio::io_service& io, DeviceContainer& hw)
	: m_plugins()
	, m_deviceContainer(hw)
	, m_io(io)
	, m_pluginEventLoop(io, boost::posix_time::millisec(16))
	, m_pluginManifests()
	, m_fatalErrorHandler([]() {})
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

	try {
		fs::directory_iterator it(root);
		fs::directory_iterator endit;

		while (it != endit) {
			if (fs::is_regular_file(*it) && Parsing::IsProbablyManifest(it->path().string())) {
				paths.push_back(it->path());
			}
			++it;
		}
	}
	catch (const boost::filesystem::filesystem_error& ec) {
		BOOST_LOG_TRIVIAL(error) << "[PluginDiscovery] " << ec.what();
	}

	return paths;
}

void PluginManager::Discover()
{
	namespace fs = boost::filesystem;

	std::vector<fs::path> paths = findManifests(fs::current_path());

	BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) << "[PluginManager] " << " Found " << paths.size() << " plugin manifests:";
	for (const auto& manifest : paths) {
		BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) <<  "--> " << manifest;
	}

	for (const auto& manifest : paths) {
		BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) << "[PluginManager] " << " Attempting to parse " << manifest.stem().string();
		if (auto config = Parsing::ParseConfig(manifest.string())) {
			std::string stem = manifest.stem().string();
			std::string dllName = stem.substr(0, stem.find_last_of('_'));
			m_pluginManifests[dllName] = *config;
			BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) << "--> Success. Assuming dll name is " << dllName;
		}
		else {
			BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) << "--> Failure";

		}
	}
}

void PluginManager::LoadAll()
{
	for (const auto& nameManifestPair : m_pluginManifests) {
		LoadPlugin(nameManifestPair.first);
	}
}


void PluginManager::UnloadAll()
{
	destroyAll();
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
			BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) << "[PluginManager] Successfully unloaded misbehaving plugin " << crashedPlugin;
		}
		else {
			BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_info) << "[PluginManager] Unable to unload misbehaving plugin " << crashedPlugin;
			return false;

		}

	}

	return true;
}

bool PluginManager::Reload(const std::string & name)
{
	//Todo: This is completely untested

	std::vector<nsvr_device_id> toBeRemoved;
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
	}

	return false;

}

void PluginManager::OnFatalError(std::function<void()> handler)
{
	m_fatalErrorHandler = handler;
}




bool PluginManager::linkPlugin(const std::string& name) {

	auto dispatcher = std::make_unique<HardwareEventDispatcher>(m_io);
	dispatcher->OnDeviceConnected([this](nsvr_device_id id, PluginApis& apis, const std::string& pluginName) {
		m_deviceContainer.AddDevice(id, apis, m_pluginManifests.at(pluginName).bodygraph, pluginName);
	});

	dispatcher->OnDeviceDisconnected([this](nsvr_device_id id) {
		m_deviceContainer.RemoveDevice(id);
	}); 

	auto instance = std::make_shared<PluginInstance>(m_io, std::move(dispatcher), name);
	if (instance->Link()) {
		m_plugins.insert(std::make_pair(name, instance));
		return true;
	}
	
	return false;
	
}
bool PluginManager::instantiatePlugin(PluginInstance* plugin)
{
	return plugin->Instantiate();
}

bool PluginManager::configurePlugin(PluginInstance* plugin)
{

	if (!plugin->Configure()) {
		BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_warning) << "Couldn't configure " << plugin->GetFileName();
		return false;
	}

	return true;
}

bool PluginManager::LoadPlugin(const std::string& name)
{
	if (!linkPlugin(name)) {
		return false;
	}

	if (!instantiatePlugin(m_plugins[name].get())) {
		return false;
	}

	if (!configurePlugin(m_plugins[name].get())) {
		return false;
	}

	return true;
}


void PluginManager::destroyAll()
{
	for (auto& plugin : m_plugins) {
		if (!plugin.second->Unload()) {
			BOOST_LOG_SEV(sclogger::get(), nsvr_loglevel_error) << "[PluginManager] Warning: unable to destroy " << plugin.first;
		}
	}

}

