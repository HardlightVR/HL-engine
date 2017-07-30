#include "stdafx.h"
#include "PluginApis.h"

bool PluginApis::SupportsApi(const std::string & name)
{
	return m_apis.find(name) != m_apis.end();
}
