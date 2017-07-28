#include "stdafx.h"
#include "PluginApiRegistry.h"
#include "HardwareCoordinator.h"
#include "cevent_internal.h"
#include "nsvr_preset.h"
PluginApiRegistry::PluginApiRegistry()
{
}


bool PluginApiRegistry::QueryNode(const char* node, nsvr_sampling_nodestate* state) {
	if (sampling_impl.initialized()) {
		sampling_impl.submit_query.invoke(node, state);
		return true;
	}
	return false;
}
void PluginApiRegistry::Preset(nsvr_preset_request * request)
{
	if (preset_impl.initialized()) {
		preset_impl.submit_preset.invoke(request);
	}
}

void PluginApiRegistry::Buffered(nsvr_buffered_request * request)
{
	if (buffered_impl.initialized()) {
		buffered_impl.submit_buffer.invoke(request);
	}
}

void PluginApiRegistry::Playback(uint32_t command, uint64_t handle)
{
	if (!playback_impl.initialized()) { return; }
	
	switch (command) {
	case 1:
		playback_impl.submit_pause.invoke(handle);
		break;
	case 2:
		playback_impl.submit_unpause.invoke(handle);
		break;
	case 3:
		playback_impl.submit_cancel.invoke(handle);
		break;
	default:
		break;
	}
}

void PluginApiRegistry::Register(nsvr_plugin_buffer_api* api)
{
	buffered_impl.submit_buffer = { api->buffered_handler, api->client_data };
}

void PluginApiRegistry::Register(nsvr_plugin_preset_api* api)
{
	preset_impl.submit_preset = { api->preset_handler, api->client_data };
}

void PluginApiRegistry::Register(nsvr_plugin_sampling_api* api) {
	sampling_impl.submit_query = { api->query_handler, api->client_data };
}

void PluginApiRegistry::Register(nsvr_plugin_playback_api* api)
{
	playback_impl.submit_pause = { api->pause_handler, api->client_data };
	playback_impl.submit_cancel = { api->cancel_handler, api->client_data };
	playback_impl.submit_unpause = { api->unpause_handler, api->client_data };
}
