#include "stdafx.h"
#include "PluginApis.h"

bool PluginApis::SupportsApi(Apis api)
{
	return m_apis.find(api) != m_apis.end();
}
