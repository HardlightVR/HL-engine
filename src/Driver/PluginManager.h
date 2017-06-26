#pragma once
#include "PluginInstance.h"
#include <array>
#include <memory>
class RegionRegistry;
class PluginManager {
public:
	PluginManager(std::vector<std::string> plugins);
	PluginManager(const PluginInstance&) = delete;
	const PluginManager& operator=(const PluginManager&) = delete;

	bool LoadAll();
	bool UnloadAll();

	template<typename THapticType>
	void Dispatch(const std::string& region, const std::string& iface, const THapticType* input);

private:
	bool linkAll();
	bool instantiateAll();
	bool configureAll();

	bool destroyAll();
	std::vector<std::string> m_pluginNames;
	std::unordered_map<std::string, std::shared_ptr<PluginInstance>> m_plugins;

	
};

template<typename THapticType>
inline void PluginManager::Dispatch(const std::string& region, const std::string& iface, const THapticType * input)
{
	for (auto& plugin : m_plugins) {
		plugin.second->Dispatch(region, iface, input);
	}
}
