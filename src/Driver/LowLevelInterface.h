#pragma once
#include "PluginAPI.h"

#include <boost/optional.hpp>

template<typename T, typename Argument>
struct callback {
	T handler;
	void* user_data;
	
	callback(T handler, void* ud);
	callback();
	void invoke(Argument argument);
	bool initialized() { return handler != nullptr; }
};



class HardwareCoordinator;

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

class LowLevelInterface {
public:

//way too strong coupling here..
	LowLevelInterface(HardwareCoordinator& coordinator);

	//void Direct(nsvr_direct_request* request);

	void Preset(nsvr_preset_request* request);
	void Buffered(nsvr_buffered_request* request);
	void Playback(uint32_t command, uint64_t handle);


	void Register(nsvr_plugin_buffer_api* api);
	void Register(nsvr_plugin_preset_api* api);
	void Register(nsvr_plugin_playback_api* api);

	

private:
	
	HardwareCoordinator& coordinator;
	
	buffered_api buffered_impl;
	preset_api preset_impl;
	playback_api playback_impl;
	
};


template<typename T, typename Argument>
inline callback<T, Argument>::callback(T handler, void * ud) : handler(handler), user_data(ud)
{
}

template<typename T, typename Argument>
inline callback<T, Argument>::callback() : handler(nullptr), user_data(nullptr)
{
}

template<typename T, typename Argument>
inline void callback<T, Argument>::invoke(Argument  argument)
{
	handler(argument, user_data);
}

