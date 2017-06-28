#include "stdafx.h"
#include "PluginInstance.h"
#include <boost/type_index.hpp>
#include <iostream>
PluginInstance::PluginInstance(std::string fileName, HardwareDataModel& model) :
	m_fileName(fileName), 
	m_loaded{ false },
	m_model(model)
{
}

int PluginInstance::UpdateTracking(const char* region, const NSVR_Core_Quaternion* update)
{
	m_model.Update(region, *update);
	return 1;
}

int PluginInstance::UpdateDeviceStatus(bool connected)
{
	if (connected) {
		m_model.SetDeviceConnected();
	}
	else {
		m_model.SetDeviceDisconnected();
	}

	return 1;
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



PluginInstance::ClientData::ClientData(std::string iface, NSVR_Consumer_Handler_t cb, void * ud):
	Interface(iface),
	Callback(cb),
	Data(ud)
{
}
