#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>


#include "DriverConfigParser.h"
PluginInstance::PluginInstance(std::string fileName) :
	m_fileName(fileName), 
	m_loaded{ false },
	m_pluginApi{},
	register_fn{},
	m_registry(),
	m_eventHandler(),
	m_facade(m_registry, m_eventHandler)
	
{
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
		HardwareDescriptor descriptor = DriverConfigParser::ParseConfig(manifestFilename);
	}
	catch (const std::exception& error) {
		std::cout << "Unable to parse descriptor for " << m_fileName << '\n';
		return false;
	}

	return true;

}


//Precondition: linked successfully
//does nothing if already loaded. May want to reinstantiate instead. We'll see.
bool PluginInstance::Load()
{
	if (m_loaded) {
		return true;
	}


	if (register_fn) {
		register_fn(&m_pluginApi);

		m_loaded = m_pluginApi.init(&m_rawPtr);
		
	}	
	
	return m_loaded;
	
	
}



//precondition: successfully loaded
bool PluginInstance::Configure()
{
	if (m_pluginApi.configure) {
		return m_pluginApi.configure(m_rawPtr, reinterpret_cast<nsvr_core*>(&m_facade));
	}
	
	return false;
}

bool PluginInstance::Link()
{
	boost::system::error_code loadFailure;
	m_lib = std::make_unique<boost::dll::shared_library>(m_fileName, boost::dll::load_mode::append_decorations, loadFailure);
	
	if (loadFailure) {
		std::cout << "Failed to load " << m_fileName << ": " << loadFailure.message() << ".\n";
		return false;
	}

	std::cout << "Loaded " << m_fileName << ".\n";

	if (!tryLoad(m_lib, "nsvr_plugin_register", register_fn)) {
		return false;
	}
	
	return true;

}

//precondition: linked successfully
bool PluginInstance::Unload()
{
	bool result = false;
	if (m_pluginApi.free) {
		result = m_pluginApi.free(m_rawPtr);
		m_rawPtr = nullptr;
	}
	m_lib.reset();

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


