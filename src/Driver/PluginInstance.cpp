#include "stdafx.h"
#include "PluginInstance.h"
#include <boost/type_index.hpp>
#include <iostream>
#include "RegionRegistry.h"
PluginInstance::PluginInstance(std::string fileName) : m_fileName(fileName), m_loaded{ false }
{
}

PluginInstance::~PluginInstance()
{
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
	if (m_configure) {
		return m_configure(m_rawPtr, AS_TYPE(NSVR_Core, this));
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

	if (!tryLoad(m_lib, "NSVR_Init", m_creator)) {
		return false;
	}

	if (!tryLoad(m_lib, "NSVR_Free", m_destructor)) {
		return false;
	}
	
	if (!tryLoad(m_lib, "NSVR_Configure", m_configure)) {
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



int PluginInstance::RegisterInterface(NSVR_Consumer_Handler_t callback, const char * region, const char*iface, void * client_data)
{

	m_interfaces[region].emplace_back(iface, callback, client_data);
	return 1;
}


std::vector<PluginInstance::ClientData> PluginInstance::getProviders(const std::string& region, const std::string& iface) {
	std::vector<PluginInstance::ClientData> copiedResults;
	if (m_interfaces.find(region) != m_interfaces.end())
	{
		auto &atRegion = m_interfaces.at(region);
		for (const auto& clientData : atRegion) {
			if (clientData.Interface == iface) {
				copiedResults.push_back(clientData);
			}
		}
	}
	return copiedResults;
}


PluginInstance::PluginInstance(PluginInstance && old) : 
	  m_lib(std::move(old.m_lib))
	, m_creator(std::move(old.m_creator))
	, m_destructor(std::move(old.m_destructor))
	, m_displayName(std::move(old.m_displayName))
	, m_fileName(std::move(old.m_fileName))
	, m_loaded(std::move(old.m_loaded))
	//be sure to update when we add new function defs. Should only be about 5-10 total.
	//He whispered. Hoping. 
{
}

PluginInstance::ClientData::ClientData(std::string iface, NSVR_Consumer_Handler_t cb, void * ud):
	Interface(iface),
	Callback(cb),
	Data(ud)
{
}
