#include "stdafx.h"
#include "PluginManager.h"
#include <iostream>
#include <bitset>
#include "DeviceContainer.h"
#include "HardwareEventDispatcher.h"
PluginManager::PluginManager(boost::asio::io_service& io, DeviceContainer& hw, std::vector<std::string> plugins) 
	: m_pluginNames(std::move(plugins))
	, m_plugins()
	, m_deviceContainer(hw)
	, m_io(io)
	, m_pluginEventLoop(io, boost::posix_time::millisec(16))
{
	m_pluginEventLoop.SetEvent([this]() {
		
		this->run_event_loop(16); //todo: should store 16 as variable
	});
	m_pluginEventLoop.Start();
}

bool PluginManager::LoadAll()
{
	for (const auto& pluginName : m_pluginNames) {
		LoadPlugin(pluginName);
	}
	return true;
}


bool PluginManager::UnloadAll()
{
	destroyAll();
	return true;
}

void PluginManager::run_event_loop(uint64_t dt)
{
	for (auto& plugin : m_plugins) {
		//if return value is false, we should destroy and reinstantiate the plugin
		//taking care to clean up the DeviceContainer somehow
		plugin.second->run_update_loop_once(dt);
	}
}

bool PluginManager::Reload(const std::string & name)
{
	//it's not the plugin's job to clean up the DeviceContainer
	//so we just unload it and try to reload it

	if (m_plugins[name]->Unload()) {
		return m_plugins[name]->Load();
	}

	return false;

}




bool PluginManager::linkPlugin(const std::string& name) {

	auto dispatcher = std::make_unique<HardwareEventDispatcher>(m_io);
	dispatcher->OnDeviceConnected([this](nsvr_device_id id, PluginApis& apis, const Parsing::ManifestDescriptor& description) {
		m_deviceContainer.AddDevice(id, apis, description.bodygraph);
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
bool PluginManager::instantiatePlugin(std::shared_ptr<PluginInstance>& plugin)
{
	return plugin->Load();
}

bool PluginManager::configurePlugin(std::shared_ptr<PluginInstance>& plugin)
{
	//The manifest must be parsed before instantiating the plugins
	if (!plugin->ParseManifest()) {
		BOOST_LOG_TRIVIAL(warning) << "Couldn't parse manifest of " << plugin->GetFileName();

		return false;
	}

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

	if (!instantiatePlugin(m_plugins[name])) {
		return false;
	}

	if (!configurePlugin(m_plugins[name])) {
		return false;
	}

	return true;
}


void PluginManager::destroyAll()
{
	for (auto& plugin : m_plugins) {
		if (!plugin.second->Unload()) {
			std::cout << "Warning: unable to destroy " << plugin.first << '\n';
		}
	}

}

