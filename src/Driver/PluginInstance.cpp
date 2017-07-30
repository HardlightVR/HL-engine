#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>
#include "IHardwareDevice.h"
#include "DeviceContainer.h"
PluginInstance::PluginInstance(std::string fileName, DeviceContainer& coord) :
	m_devices(coord),
	m_fileName(fileName), 
	m_loaded{ false },
	m_pluginFunctions{},
	m_pluginRegisterFunction{},
	m_apis(),
	m_eventHandler(),
	m_facade(m_apis, m_eventHandler)
	
{

	m_eventHandler.Subscribe(nsvr_device_event_device_connected, [&](const auto& event) {
//		m_devices.AddDevice(m_descriptor.displayName, device_factories::createDevice(m_descriptor, m_apis, m_eventHandler));
		
		
		
		std::cout << "A device was connected! inside " << m_fileName << '\n';
	});

	m_eventHandler.Subscribe(nsvr_device_event_device_disconnected, [&](const auto& event) {
	//	m_devices.RemoveDevice(m_descriptor.displayName);
		std::cout << "A device was disconnected! inside " << m_fileName << '\n';
	});
}


PluginInstance::~PluginInstance()
{
}

bool PluginInstance::ParseManifest()
{
	std::string manifestFilename = m_fileName + "_manifest.json";
	if (!DriverConfigParser::IsValidConfig(manifestFilename)) {
		std::cout << manifestFilename << " is not a valid config file\n";
		return false;
	}

	try {
		m_descriptor = DriverConfigParser::ParseConfig(manifestFilename);

		
	}
	catch (const std::exception& error) {
		std::cout << "Unable to parse descriptor for " << m_fileName << '\n';
		return false;
	}

	return true;

}

void PluginInstance::InstantiateDevices()
{
	m_devices.AddDevice(device_factories::createDevice(m_descriptor, m_apis, m_eventHandler));
}


//Precondition: linked successfully
//does nothing if already loaded. May want to reinstantiate instead. We'll see.
bool PluginInstance::Load()
{
	if (m_loaded) {
		return true;
	}


	if (m_pluginRegisterFunction) {
		m_pluginRegisterFunction(&m_pluginFunctions);

		m_loaded = m_pluginFunctions.init(&m_pluginPointer);
		
	}	
	
	return m_loaded;
	
	
}



//precondition: successfully loaded
bool PluginInstance::Configure()
{
	if (m_pluginFunctions.configure) {
		return m_pluginFunctions.configure(m_pluginPointer, reinterpret_cast<nsvr_core*>(&m_facade));
	}
	
	return false;
}

bool PluginInstance::Link()
{
	boost::system::error_code loadFailure;
	m_dll = std::make_unique<boost::dll::shared_library>(m_fileName, boost::dll::load_mode::append_decorations, loadFailure);
	
	if (loadFailure) {
		std::cout << "Failed to load " << m_fileName << ": " << loadFailure.message() << ".\n";
		return false;
	}

	std::cout << "Loaded " << m_fileName << ".\n";

	if (!tryLoad(m_dll, "nsvr_plugin_register", m_pluginRegisterFunction)) {
		return false;
	}
	
	return true;

}

//precondition: linked successfully
bool PluginInstance::Unload()
{
	bool result = false;
	if (m_pluginFunctions.free) {
		result = m_pluginFunctions.free(m_pluginPointer);
		m_pluginPointer = nullptr;
	}
	m_dll.reset();

	m_loaded = false;
	return result;
}

bool PluginInstance::IsLoaded() const
{
	return m_loaded;
}

std::string PluginInstance::GetFileName() const
{
	return m_fileName;
}

std::string PluginInstance::GetDisplayName() const
{
	return m_displayName;
}


