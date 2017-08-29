#include "stdafx.h"
#include "PluginManager.h"
#include <iostream>
#include <bitset>
#include "DeviceContainer.h"
PluginManager::PluginManager(boost::asio::io_service& io, DeviceContainer& hw, std::vector<std::string> plugins) 
	: m_pluginNames(std::move(plugins))
	, m_plugins()
	, m_deviceContainer(hw)
	, m_io(io)
{
	
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
	auto instance = std::make_shared<PluginInstance>(m_io, name, m_deviceContainer);
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
	if (!plugin->Configure()) {
		BOOST_LOG_TRIVIAL(warning) << "Couldn't configure " << plugin->GetDisplayName();
		return false;
	}

	if (!plugin->ParseManifest()) {
		BOOST_LOG_TRIVIAL(warning) << "Couldn't parse manifest of " << plugin->GetDisplayName();

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

