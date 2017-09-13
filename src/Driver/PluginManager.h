#pragma once
#include <vector>
#include <string>
#include <memory>
#include "PluginInstance.h"
#include <boost/asio/io_service.hpp>
#include "ScheduledEvent.h"
class DeviceContainer;


std::vector<boost::filesystem::path> findManifests(const boost::filesystem::path&);


class PluginManager {
public:
	PluginManager(boost::asio::io_service& io, DeviceContainer& coordinator);
	PluginManager(const PluginInstance&) = delete;
	const PluginManager& operator=(const PluginManager&) = delete;

	void Discover();

	void LoadAll();
	void UnloadAll();

	void TickOnce(uint64_t dt);
	bool Reload(const std::string& name);

private:

	bool linkPlugin(const std::string& name);
	bool instantiatePlugin(PluginInstance* plugin);
	bool configurePlugin(PluginInstance* plugin);

	bool LoadPlugin(const std::string& name);

	std::unordered_map<std::string, Parsing::ManifestDescriptor> m_pluginManifests;

	void destroyAll();


	std::unordered_map<std::string, std::shared_ptr<PluginInstance>> m_plugins;
	DeviceContainer& m_deviceContainer;
	boost::asio::io_service& m_io;
	ScheduledEvent m_pluginEventLoop;

};

