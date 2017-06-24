#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
HardlightPlugin::HardlightPlugin() :
	m_io(std::make_shared<IoService>()),
	m_dispatcher(),
	m_adapter(std::make_unique<BoostSerialAdapter>(m_io->GetIOService())),
	m_firmware(m_adapter, m_io->GetIOService()),
	m_monitor(std::make_shared<KeepaliveMonitor>(m_io->GetIOService(), m_firmware)),
	m_synchronizer(std::make_unique<Synchronizer>(m_adapter->GetDataStream(), m_dispatcher, m_io->GetIOService()))

{
	m_adapter->SetMonitor(m_monitor);
	m_adapter->Connect();
	m_synchronizer->BeginSync();
}

HardlightPlugin::~HardlightPlugin()
{
	m_io->Shutdown();

}




int HardlightPlugin::Configure(NSVR_Core* core)
{
	NSVR_RegParams params = { 0 };
	params.Provider = AS_TYPE(NSVR_Provider, this);
	params.Interface = "brief-taxel";

	std::vector<std::string> regions = {
		"left_back",
		"right_back",
		"left_shoulder",
		"right_shoulder",
		"left_upper_arm",
		"right_upper_arm",
		"left_forearm",
		"right_forearm",
		"left_upper_chest",
		"right_upper_chest",
		"left_upper_ab",
		"right_upper_ab",
		"left_mid_ab",
		"right_mid_ab",
		"left_lower_ab",
		"right_lower_ab"
	};
	for (const auto& region : regions) {
		params.Region = region.c_str();
		NSVR_Core_RegisterNode(core, params);
	}

	return 1;
}

int HardlightPlugin::PlayBrief(Location loc, uint32_t effect, float strength)
{
	m_firmware.PlayEffect(loc, effect, strength);
	return 1;
}


