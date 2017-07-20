#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>

#include "FunctionPointerTemplates.h"

#include "DriverConfigParser.h"
PluginInstance::PluginInstance(std::string fileName, HardwareDataModel& model) :
	m_fileName(fileName), 
	m_loaded{ false },
	m_model(model)
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

	m_loaded = true;

	if (m_creator) {
		return m_creator(&m_rawPtr);
	}	
	else {
		return false;
	}
	
}



//precondition: successfully loaded
bool PluginInstance::Configure()
{
	NSVR_Configuration config;

	


	if (m_configure) {
		return m_configure(m_rawPtr, reinterpret_cast<nsvr_core*>(&m_model));
	}
	else {
		return false;
	}
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

	if (!tryLoad(m_lib, "NSVR_Plugin_Init", m_creator)) {
		return false;
	}

	if (!tryLoad(m_lib, "NSVR_Plugin_Free", m_destructor)) {
		return false;
	}
	
	if (!tryLoad(m_lib, "NSVR_Plugin_Configure", m_configure)) {
		return false;
	}

	
	return true;

}

//precondition: linked successfully
bool PluginInstance::Unload()
{
	bool result = false;
	if (m_destructor) {
		result = m_destructor(&m_rawPtr);
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


