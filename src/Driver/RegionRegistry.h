#pragma once
#include <vector>
#include <unordered_map>
#include "../include/PluginAPI.h"
#include <set>
#include "PluginInstance.h"
#include "PluginManager.h"
class RegionRegistry {
public:
	typedef std::size_t RegionIndex;

	RegionRegistry(PluginManager& pluginManager);
	int RegisterNode(NSVR_RegParams params);

	template<class THapticType>
	int Activate(const std::string& region, const std::string& iface, const THapticType* args);

	std::vector<PluginInstance*> GetPluginSet(NSVR_Region regions);
private:
	PluginManager& m_pluginManager;
	std::unordered_map<RegionIndex, std::vector<PluginInstance*>> m_regionRegistry;
	typedef std::unordered_map<std::string, std::set<NSVR_Provider*>> ProviderMap;
	std::unordered_map<std::string, ProviderMap> m_interfaces;
};

template<class THapticType>
int RegionRegistry::Activate(const std::string& region, const std::string& iface, const THapticType* args)
{
	if (m_interfaces.find(region) == m_interfaces.end()) {
		return -1;
	}

	if (m_interfaces.at(region).find(iface) == m_interfaces.at(region).end()) {
		return -1;
	}

	auto foundInterfaces = m_interfaces.at(region).at(iface);
	auto actualPlugins = m_pluginManager.GetPlugins();
	std::set<NSVR_Provider*> actualInterfaces;
	for (PluginInstance* p : actualPlugins) {
		actualInterfaces.insert(p->GetRawHandle());
	}
	std::vector<NSVR_Provider*> result(actualInterfaces.size());
	auto it = std::set_intersection(foundInterfaces.begin(), foundInterfaces.end(), 
		actualInterfaces.begin(), actualInterfaces.end(), result.begin ());

	result.resize(it - result.begin());




	for (const auto& a : result) {
		std::cout << "Found plugin that matched: " << a << '\n';
		for (PluginInstance* plugin : actualPlugins) {
			if (plugin->GetRawHandle() == a) {
				plugin->Dispatch(region.c_str(), args);
			}
		}
	}
}
