#pragma once
#include <vector>
#include <string>
#include <memory>
#include "PluginInstance.h"

class DeviceContainer;
class PluginManager {
public:
	PluginManager(DeviceContainer& coordinator,std::vector<std::string> plugins);
	PluginManager(const PluginInstance&) = delete;
	const PluginManager& operator=(const PluginManager&) = delete;

	bool LoadAll();
	bool UnloadAll();

private:
	bool linkAll();
	bool instantiateAll();
	bool configureAll();

	bool destroyAll();
	std::vector<std::string> m_pluginNames;
	std::unordered_map<std::string, std::shared_ptr<PluginInstance>> m_plugins;
	DeviceContainer& m_deviceContainer;
	
};

