#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/asio/io_service.hpp>

#include "PluginInstance.h"
#include "ScheduledEvent.h"


class DeviceContainer;

std::vector<boost::filesystem::path> findManifests(const boost::filesystem::path&);


class PluginManager {
public:
	//When PluginManager configures and instantiates all the plugins, 
	//each plugin may provide some devices for use in the system.
	//PluginManager needs somewhere to put them, but doesn't care beyond that (we deal with devices on a higher level than the Manager).

	PluginManager(boost::asio::io_service& io, DeviceContainer& devices);
	PluginManager(const PluginManager&) = delete;
	const PluginManager& operator=(const PluginManager&) = delete;

	//Look for any manifest files present and attempt to parse them, figuring out the .dll name, etc.
	void Discover();

	//Links, instantiates, and configures all the plugins discovered previously
	void LoadAll();

	//Unloads the plugins that are currently present
	void UnloadAll();

	//Runs each plugin's event loop once with a given ms delta time
	//It is a fatal error if this method returns false. The program should be terminated as gracefully as possible. 
	bool TickOnce(uint64_t dt);
	bool Reload(const std::string& name);

	void OnFatalError(std::function<void()> handler);

private:

	std::function<void()> m_fatalErrorHandler;
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

