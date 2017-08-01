#pragma once
#include "PluginAPI.h"

#include <boost/optional.hpp>
#include <unordered_map>
#include <memory>
#include <boost/optional.hpp>
#include <boost/log/trivial.hpp>

// Represents a user-provided callback
template<typename FnPtr, typename... Arguments>
struct callback {
	FnPtr handler;
	void* user_data;
	
	callback(FnPtr handler, void* ud);
	void operator()(Arguments... arguments);
};

template<typename FnPtr, typename ...Arguments>
inline callback<FnPtr, Arguments...>::callback(FnPtr handler, void * ud) 
	: handler(handler)
	, user_data(ud) {}


template<typename FnPtr, typename ...Arguments>
inline void callback<FnPtr, Arguments...>::operator()(Arguments ...argument)
{
	handler(std::forward<Arguments>(argument)..., user_data);
}


// Base class for internal plugin api representation
// So we register new apis at runtime
class plugin_api {
public:
	virtual ~plugin_api() {}
};


enum class Apis {
	Unknown = 0,
	Buffered,
	Preset,
	Request,
	Playback,
	Sampling,
	Device,
	RawCommand,
	Tracking
};

struct buffered_api : public plugin_api {
	buffered_api(nsvr_plugin_buffer_api* api)
		: submit_buffer { api->buffered_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_buffer_api::nsvr_buffered_handler, 
		nsvr_buffered_request*
	> submit_buffer;
	static Apis getApiType() { return Apis::Buffered; }
};

struct preset_api : public plugin_api {
	preset_api(nsvr_plugin_preset_api* api)
		: submit_preset{ api->preset_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_preset_api::nsvr_preset_handler, 
		nsvr_preset_request*
	> submit_preset;

	static Apis getApiType() { return Apis::Preset; }
};

struct request_api : public plugin_api {
	request_api(nsvr_plugin_request_api* api)
		: submit_request{ api->request_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_request_api::nsvr_request_handler, 
		nsvr_request*
	> submit_request;

	static Apis getApiType() { return Apis::Request; }
};

struct playback_api : public plugin_api {
	playback_api(nsvr_plugin_playback_api* api)
		: submit_pause{ api->pause_handler, api->client_data }
		, submit_cancel{ api->cancel_handler, api->client_data }
		, submit_unpause{ api->unpause_handler, api->client_data } {}

	callback<
		nsvr_plugin_playback_api::nsvr_playback_cancel, 
		uint64_t
	> submit_cancel;

	callback<
		nsvr_plugin_playback_api::nsvr_playback_pause, 
		uint64_t
	> submit_pause;

	callback<
		nsvr_plugin_playback_api::nsvr_playback_unpause,
		uint64_t
	> submit_unpause;

	static  Apis getApiType() { return Apis::Playback; }
};

struct sampling_api : public plugin_api{
	sampling_api(nsvr_plugin_sampling_api* api) 
		: submit_query{ api->query_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_sampling_api::nsvr_sampling_querystate, 
		const char*, 
		nsvr_sampling_nodestate*
	> submit_query;

	static Apis getApiType() { return Apis::Sampling; }
};


struct device_api : public plugin_api {
	device_api(nsvr_plugin_device_api* api)
		: submit_enumerateids{ api->enumerateids_handler, api->client_data }
		, submit_getinfo{ api->getinfo_handler, api->client_data } {}

	callback<
		nsvr_plugin_device_api::nsvr_device_enumerateids,
		nsvr_device_ids*
	> submit_enumerateids;

	callback<
		nsvr_plugin_device_api::nsvr_device_getinfo,
		uint64_t,
		nsvr_device_basic_info*
	> submit_getinfo;

	static  Apis getApiType() { return Apis::Device; }

};

struct rawcommand_api : public plugin_api {
	rawcommand_api(nsvr_plugin_rawcommand_api* api)
		: submit_rawcommand{ api->send_handler, api->client_data } {}

	callback<
		nsvr_plugin_rawcommand_api::nsvr_rawcommand_send,
		uint8_t*,
		unsigned int
	> submit_rawcommand;

	static Apis getApiType() { return Apis::RawCommand; }
};

struct tracking_api : public plugin_api {
	tracking_api(nsvr_plugin_tracking_api* api)
		: submit_beginstreaming{ api->beginstreaming_handler, api->client_data }
		, submit_endstreaming{ api->endstreaming_handler, api->client_data } {}

	callback<
		nsvr_plugin_tracking_api::nsvr_tracking_beginstreaming,
		nsvr_tracking_stream*,
		const char*
	> submit_beginstreaming;

	callback<
		nsvr_plugin_tracking_api::nsvr_tracking_endstreaming,
		const char*
	> submit_endstreaming;

	static Apis getApiType() { return Apis::Tracking; }
};

// Represents the capabilities of a particular plugin, e.g. a plugin supports
// the buffered api, the preset api, and the playback api.
// A plugin may register these apis indirectly using Register, 
// and we can retrieve and use it internally by calling GetApi.
class PluginApis {
public:

	template<typename InternalApi, typename ExternalApi>
	void Register(ExternalApi* api);
	
	template<typename T>
	T* GetApi();

	bool SupportsApi(Apis name);
private:
	std::unordered_map<Apis, std::unique_ptr<plugin_api>> m_apis;
};





template<typename InternalApi, typename ExternalApi>
inline void PluginApis::Register(ExternalApi * api)
{
	auto x = std::unique_ptr<plugin_api>(new InternalApi(api));
	m_apis.emplace(std::make_pair(InternalApi::getApiType(), std::move(x)));
}

// Returns nullptr if the api identified by name is not found, or if the api is 
// found but the supplied type is wrong.
template<typename T>
inline T* PluginApis::GetApi()
{
	if (m_apis.find(T::getApiType()) != m_apis.end()) {
		auto ptr = m_apis.at(T::getApiType()).get();
		if (T* derived_ptr = dynamic_cast<T*>(ptr)) {
			return derived_ptr;
		}
	}
	else {
		BOOST_LOG_TRIVIAL(warning) << "[PluginApis] The request plugin API '" << (int)T::getApiType() << "' was not found!";
	}
	return nullptr;
}
