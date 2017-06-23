#pragma once
#include "PluginInstance.h"
#include <array>
class RegionRegistry;
class PluginManager {
public:
	PluginManager(std::vector<std::string> plugins);
	PluginManager(const PluginInstance&) = delete;
	const PluginManager& operator=(const PluginManager&) = delete;

	bool LoadAll(RegionRegistry&);
	bool UnloadAll();
	std::vector<PluginInstance*> GetPlugins();
private:
	bool linkAll();
	bool instantiateAll();
	bool configureAll(RegionRegistry&);

	bool destroyAll();
	std::vector<std::string> m_pluginNames;
	std::unordered_map<std::string, PluginInstance> m_plugins;

	
};