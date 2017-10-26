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
	
	PluginManager(boost::asio::io_service& io);
	PluginManager(const PluginManager&) = delete;
	const PluginManager& operator=(const PluginManager&) = delete;
	~PluginManager() {
		std::cout << "PLUGIN MANAGER DESTRUCTOR\n";

	}
	//Look for any manifest files present and attempt to parse them, figuring out the .dll name, etc.
	void Discover();

	//Links, instantiates, and configures all the plugins discovered previously
	void LoadAll();

	//Unloads the plugins that are currently present
	void UnloadAll();

	PluginInstance* MakeVirtualPlugin();

	//Runs each plugin's event loop once with a given ms delta time
	//It is a fatal error if this method returns false. The program should be terminated as gracefully as possible. 
	bool TickOnce(uint64_t dt);
	bool Reload(const std::string& name);

	void OnFatalError(std::function<void()> handler);

	std::vector<uint32_t> GetPluginIds() const;
	void DrawDiagnostics(uint32_t id, nsvr_diagnostics_ui* ui);

	struct PluginInfo;
	boost::optional<std::pair<std::string, PluginInfo>> GetPluginInfo(uint32_t id);

	struct PluginInfo {
		using PluginName = std::string;
		Parsing::ManifestDescriptor Descriptor;
		std::string DllPath;
	};
	void SetDeviceContainer(DeviceContainer* devices);
private:

	std::function<void()> m_fatalErrorHandler;
	std::unique_ptr<PluginInstance> linkPlugin(const std::string& name);
	bool instantiatePlugin(PluginInstance* plugin);
	bool configurePlugin(PluginInstance* plugin);

	bool LoadPlugin(const std::string& searchDirectory, const std::string& dllName);

	
	std::unordered_map<PluginInfo::PluginName, PluginInfo> m_pluginInfo;

	void destroyAll();


	std::unordered_map<std::string, std::shared_ptr<PluginInstance>> m_plugins;
	DeviceContainer* m_deviceContainer;
	boost::asio::io_service& m_io;
	ScheduledEvent m_pluginEventLoop;

	void instantiateVirtualDevice(const std::string& plugin, const Parsing::VirtualDeviceDescriptor& device);
};

