#include "stdafx.h"
#include <string>
#include "PluginDummy.h"

#include "DeviceManager.h"



int PluginDummy::Configure(nsvr_core* core)
{
	int x = 4;
	nsvr_directory data_dir = { 0 };
	nsvr_filesystem_getdatadirectory(core, &data_dir);



	m_plugin = std::make_unique<DeviceManager>(std::string(data_dir.path));
	return m_plugin->configure(core);
}