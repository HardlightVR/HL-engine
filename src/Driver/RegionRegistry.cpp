#include "stdafx.h"
#include "RegionRegistry.h"
#include "PluginManager.h"
#include <bitset>

RegionRegistry::RegionRegistry(PluginManager & pluginManager):
	m_pluginManager(pluginManager)
{
}

void RegionRegistry::RegisterPluginRegions()
{
	m_regionRegistry.clear();

	std::vector<PluginInstance*> plugins = m_pluginManager.GetPlugins();
	for (auto& plugin : plugins) {
		NSVR_Region requestedRegions = NSVR_Region::NSVR_Region_None;

		if (plugin->RegisterRegions(&requestedRegions)) {
			std::bitset<32> regionSet(requestedRegions);
			for (RegionIndex i = 0; i < regionSet.size(); i++) {
				if (regionSet.test(i)) {
					m_regionRegistry[i].push_back(plugin);
				}
			}
		}
	}

}

//should return IPlugin
std::vector<PluginInstance*> RegionRegistry::GetPluginSet(NSVR_Region regions)
{
	std::vector<PluginInstance*> results;
	std::bitset<32> regionSet(regions);
	for (std::size_t i = 0; i < regionSet.size(); i++) {
		if (regionSet.test(i)) {
			auto resultsForSingleRegion = m_regionRegistry[i];
			results.insert(results.end(), resultsForSingleRegion.begin(), resultsForSingleRegion.end());
		}
	}

	std::sort(results.begin(), results.end());
	results.erase(std::unique(results.begin(), results.end()), results.end());

	return results;
}
