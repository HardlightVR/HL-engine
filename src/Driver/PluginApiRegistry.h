#pragma once
#include "PluginAPI.h"

#include <boost/optional.hpp>

template<typename T, typename... Arguments>
struct callback {
	T handler;
	void* user_data;
	
	callback(T handler, void* ud);
	callback();
	void invoke(Arguments... argument);
	bool initialized() { return handler != nullptr; }
};




struct buffered_api {
	callback<nsvr_plugin_buffer_api::nsvr_buffered_handler, nsvr_buffered_request*> submit_buffer;
	bool initialized() { return submit_buffer.initialized(); }
};

struct preset_api {
	callback<nsvr_plugin_preset_api::nsvr_preset_handler, nsvr_preset_request*> submit_preset;
	bool initialized() { return submit_preset.initialized(); }
};


struct playback_api {
	callback<nsvr_plugin_playback_api::nsvr_playback_cancel, uint64_t> submit_cancel;
	callback<nsvr_plugin_playback_api::nsvr_playback_pause, uint64_t> submit_pause;
	callback<nsvr_plugin_playback_api::nsvr_playback_unpause, uint64_t> submit_unpause;
	bool initialized() { return submit_cancel.initialized() && submit_pause.initialized() && submit_unpause.initialized(); }
};

struct sampling_api {
	callback<nsvr_plugin_sampling_api::nsvr_sampling_querystate, const char*, nsvr_sampling_nodestate*> submit_query;
	bool initialized() { return submit_query.initialized(); }

};
class PluginApiRegistry {
public:

	PluginApiRegistry();

	//void Direct(nsvr_direct_request* request);

	void Preset(nsvr_preset_request* request);
	void Buffered(nsvr_buffered_request* request);
	void Playback(uint32_t command, uint64_t handle);

	bool QueryNode(const char* which, nsvr_sampling_nodestate* outState);

	void Register(nsvr_plugin_buffer_api* api);
	void Register(nsvr_plugin_preset_api* api);
	void Register(nsvr_plugin_playback_api* api);
	void Register(nsvr_plugin_sampling_api* api);
	

private:
	
	
	buffered_api buffered_impl;
	preset_api preset_impl;
	playback_api playback_impl;
	sampling_api sampling_impl;
	
};



template<typename T, typename ...Arguments>
inline callback<T, Arguments...>::callback(T handler, void * ud) : handler(handler), user_data(ud)
{
}

template<typename T, typename ...Arguments>
inline callback<T, Arguments...>::callback() : handler(nullptr), user_data(nullptr)
{
}

template<typename T, typename ...Arguments>
inline void callback<T, Arguments...>::invoke(Arguments ...argument)
{
	handler(std::forward<Arguments>(argument)..., user_data);
}
