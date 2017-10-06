#include "stdafx.h"
#include "PluginApis.h"





bool PluginApis::Supports(Apis api) const
{
	return m_apis.find(api) != m_apis.end();
}

void PluginApis::Each(std::function<void(Apis, plugin_api*)> fn)
{
	for (auto& kvp : m_apis) {
		fn(kvp.first, kvp.second.get());
	}
}
