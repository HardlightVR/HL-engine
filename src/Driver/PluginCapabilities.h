#pragma once
#include "PluginAPI.h"

#include <boost/optional.hpp>
#include <unordered_map>
#include <memory>

#include <boost/log/trivial.hpp>

// Represents a user-provided callback
template<typename FnPtr, typename... Arguments>
struct callback {
	FnPtr handler;
	void* user_data;
	
	callback(FnPtr handler, void* ud);
	void invoke(Arguments... arguments);
};

template<typename FnPtr, typename ...Arguments>
inline callback<FnPtr, Arguments...>::callback(FnPtr handler, void * ud) 
	: handler(handler)
	, user_data(ud) {}


template<typename FnPtr, typename ...Arguments>
inline void callback<FnPtr, Arguments...>::invoke(Arguments ...argument)
{
	handler(std::forward<Arguments>(argument)..., user_data);
}


// Base class for internal plugin api representation
// So we register new apis at runtime
class plugin_api {
public:
	virtual ~plugin_api() {}
	virtual const char* getCapabilityName() const = 0;
};


struct buffered_api : public plugin_api {
	buffered_api(nsvr_plugin_buffer_api* api)
		: submit_buffer { api->buffered_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_buffer_api::nsvr_buffered_handler, 
		nsvr_buffered_request*
	> submit_buffer;

	const char* getCapabilityName() const override { return "buffered"; }
};

struct preset_api : public plugin_api {
	preset_api(nsvr_plugin_preset_api* api)
		: submit_preset{ api->preset_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_preset_api::nsvr_preset_handler, 
		nsvr_preset_request*
	> submit_preset;

	const char* getCapabilityName() const override { return "preset"; }
};

struct request_api : public plugin_api {
	request_api(nsvr_plugin_request_api* api)
		: submit_request{ api->request_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_request_api::nsvr_request_handler, 
		nsvr_request*
	> submit_request;

	const char* getCapabilityName() const override { return "request"; }
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

	const char* getCapabilityName() const override { return "playback"; }
};

struct sampling_api : public plugin_api{
	sampling_api(nsvr_plugin_sampling_api* api) 
		: submit_query{ api->query_handler, api->client_data } {}
	
	callback<
		nsvr_plugin_sampling_api::nsvr_sampling_querystate, 
		const char*, 
		nsvr_sampling_nodestate*
	> submit_query;

	const char* getCapabilityName() const override { return "sampling"; }
};

// Represents the capabilities of a particular plugin, e.g. a plugin supports
// the buffered api, the preset api, and the playback api.
// A plugin may register these apis indirectly using Register, 
// and we can retrieve and use it internally by calling GetApi.
class PluginCapabilities {
public:
	PluginCapabilities() = default;

	template<typename InternalApi, typename ExternalApi>
	void Register(ExternalApi* api);
	
	template<typename T>
	const T* GetApi(const std::string& name);
private:
	std::unordered_map<std::string, std::unique_ptr<plugin_api>> m_apis;
};





template<typename InternalApi, typename ExternalApi>
inline void PluginCapabilities::Register(ExternalApi * api)
{
	auto x = std::unique_ptr<plugin_api>(new InternalApi(api));
	m_apis.emplace(std::make_pair(x->getCapabilityName(), std::move(x)));
}

// Returns nullptr if the api identified by name is not found, or if the api is 
// found but the supplied type is wrong.
template<typename T>
inline const T * PluginCapabilities::GetApi(const std::string & name)
{
	if (m_apis.find(name) != m_apis.end()) {
		auto ptr = m_apis.at(name).get();
		return dynamic_cast<const T*>(ptr);
	}
	else {
		BOOST_LOG_TRIVIAL(warning) << "[PluginCapabilities] The request plugin API '" << name << "' was not found!";
	}
	return nullptr;
}
