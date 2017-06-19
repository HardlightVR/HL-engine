#pragma once
#include <vector>
#include <unordered_map>
#include "../include/PluginAPI.h"
class PluginManager;
class PluginInstance;
class RegionRegistry {
public:
	typedef std::size_t RegionIndex;

	RegionRegistry(PluginManager& pluginManager);
	void RegisterPluginRegions();
	std::vector<PluginInstance*> GetPluginSet(NSVR_Region regions);
private:
	PluginManager& m_pluginManager;
	std::unordered_map<RegionIndex, std::vector<PluginInstance*>> m_regionRegistry;
};