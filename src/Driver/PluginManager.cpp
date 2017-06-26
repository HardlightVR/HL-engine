#include "stdafx.h"
#include "PluginManager.h"
#include <iostream>
#include <bitset>
PluginManager::PluginManager(std::vector<std::string> plugins) 
	: m_pluginNames(std::move(plugins))
	, m_plugins()
{

}

bool PluginManager::LoadAll()
{
	linkAll();
	instantiateAll();
	configureAll();
	return true;
}


bool PluginManager::UnloadAll()
{
	destroyAll();
	return true;
}





bool PluginManager::linkAll()
{
	for (const std::string& pluginName : m_pluginNames) {
		m_plugins.emplace(std::make_pair(pluginName, std::make_shared<PluginInstance>(pluginName)));
		if (!m_plugins.at(pluginName)->Link()) {
			std::cout << "Warning: unable to link " << pluginName << '\n';
		}
	}

	return true;
}

bool PluginManager::instantiateAll()
{
	for (auto& plugin : m_plugins) {
		if (!plugin.second->Load()) {
			std::cout << "Warning: unable to instantiate " << plugin.first << '\n';
		}
	}

	return true;
}

bool PluginManager::configureAll()
{
	for (auto& plugin : m_plugins) {
		if (!plugin.second->Configure()) {
			std::cout << "Warning: unable to configure " << plugin.first << '\n';

		}
	}

	return true;
}

bool PluginManager::destroyAll()
{
	for (auto& plugin : m_plugins) {
		if (!plugin.second->Unload()) {
			std::cout << "Warning: unable to destroy " << plugin.first << '\n';
		}
	}
	return true;
}

