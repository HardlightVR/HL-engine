#include "stdafx.h"
#include "RegionRegistry.h"
#include "PluginManager.h"
#include <bitset>

RegionRegistry::RegionRegistry(PluginManager & pluginManager):
	m_pluginManager(pluginManager)
{
}


int RegionRegistry::RegisterNode(NSVR_RegParams params)
{
	m_interfaces[params.Region][params.Interface].insert(params.Provider);

	return 1;
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
