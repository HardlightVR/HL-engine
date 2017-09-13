#include "stdafx.h"
#include "PluginManager.h"
#include <iostream>
#include <bitset>
#include "DeviceContainer.h"
#include "HardwareEventDispatcher.h"
#include <boost/filesystem.hpp>
PluginManager::PluginManager(boost::asio::io_service& io, DeviceContainer& hw) 
	: m_plugins()
	, m_deviceContainer(hw)
	, m_io(io)
	, m_pluginEventLoop(io, boost::posix_time::millisec(16))
	, m_pluginManifests()
{
	m_pluginEventLoop.SetEvent([this]() {
		
		this->TickOnce(16); //todo: should store 16 as variable
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

	for (const auto& manifest : paths) {
		if (auto config = Parsing::ParseConfig(manifest.string())) {
			std::string stem = manifest.stem().string();
			std::string dllName = stem.substr(0, stem.find_last_of('_'));

			m_pluginManifests[dllName] = *config;
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

void PluginManager::TickOnce(uint64_t dt)
{
	for (auto& plugin : m_plugins) {
		//if return value is false, we should destroy and reinstantiate the plugin
		//taking care to clean up the DeviceContainer somehow
		plugin.second->tick_once(dt);
	}
}

bool PluginManager::Reload(const std::string & name)
{
	//it's not the plugin's job to clean up the DeviceContainer
	//so we just unload it and try to reload it

	if (m_plugins[name]->Unload()) {
		return m_plugins[name]->Instantiate();
	}

	return false;

}




bool PluginManager::linkPlugin(const std::string& name) {

	auto dispatcher = std::make_unique<HardwareEventDispatcher>(m_io);
	dispatcher->OnDeviceConnected([this](nsvr_device_id id, PluginApis& apis, const std::string& pluginName) {
		m_deviceContainer.AddDevice(id, apis, m_pluginManifests.at(pluginName).bodygraph);
	});

	dispatcher->OnDeviceDisconnected([this](nsvr_device_id id) {
		m_deviceContainer.RemoveDevice(id);
	}); 

	auto instance = std::make_shared<PluginInstance>(std::move(dispatcher), name);
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
		BOOST_LOG_TRIVIAL(warning) << "Couldn't configure " << plugin->GetFileName();
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
			BOOST_LOG_TRIVIAL(error) << "[PluginManager] Warning: unable to destroy " << plugin.first;
		}
	}

}

