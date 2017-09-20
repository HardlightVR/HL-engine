#include "stdafx.h"
#include "PluginDummy.h"
#include <string>
PluginDummy::PluginDummy()
	: m_plugin()
{
}

int PluginDummy::Configure(nsvr_core* core)
{
	nsvr_directory data_dir = { 0 };
	nsvr_filesystem_getdatadirectory(core, &data_dir);

	m_plugin = std::make_unique<HardlightPlugin>(std::string(data_dir.path));

	return m_plugin->Configure(core);
}
