#pragma once
#include "PluginAPI.h"

#include <boost/optional.hpp>

template<typename T, typename Argument>
struct callback {
	T handler;
	void* user_data;
	
	callback(T handler, void* ud);
	callback();
	void invoke(Argument* argument);
	bool initialized() { return handler != nullptr; }
};



class HardwareCoordinator;
class LowLevelInterface {
public:
	struct direct {};
	struct preset {};
//way too strong coupling here..
	LowLevelInterface(HardwareCoordinator& coordinator);
	void RegisterDirect(nsvr_direct_handler handler, void* user_data);
	void RegisterBuffered(nsvr_buffered_handler handler, void* user_data);
	void RegisterPreset(nsvr_preset_handler handler, void* user_data);

	void Direct(nsvr_direct_request* request);

	void Preset(nsvr_preset_request* request);



private:
	
	HardwareCoordinator& coordinator;
	callback<nsvr_direct_handler, nsvr_direct_request> direct_handler;
	callback<nsvr_preset_handler, nsvr_preset_request> preset_handler;
	callback<nsvr_buffered_handler, nsvr_buffered_request> buffered_handler;

	
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
inline void callback<T, Argument>::invoke(Argument * argument)
{
	handler(argument, user_data);
}

