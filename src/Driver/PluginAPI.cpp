#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"
#include "pevent.h"
#include "HardwareCoordinator.h"
#include "cevent_internal.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include "nsvr_preset.h"
#include "nsvr_playback_handle.h"
#include <boost/variant/get.hpp>
#include "IHardwareDevice.h"

#define NULL_ARGUMENT_CHECKS




#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#ifdef NULL_ARGUMENT_CHECKS
#define RETURN_IF_NULL(ptr) do { if (ptr == nullptr) { return (int) -1; }} while (0)
#else
#define RETURN_IF_NULL(ptr)
#endif

#define REGISTER_API(name) \
AS_TYPE(CoreFacade, core)->RegisterPluginApi<##name##>(api); \
return NSVR_SUCCESS;




enum returnTypes {
	NSVR_SUCCESS = 1
};


NSVR_CORE_RETURN(int) nsvr_request_gettype(nsvr_request * cevent, nsvr_request_type * outType)
{
	*outType = AS_CTYPE(nsvr::cevents::request_base, cevent)->type();
	return NSVR_SUCCESS;
}


NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_geteffect(nsvr_request* cevent, uint32_t* outEffect)
{
	*outEffect = AS_TYPE(nsvr::cevents::LastingHaptic, cevent)->effect;
	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getstrength(nsvr_request* cevent, float* outStrength)
{
	*outStrength = AS_TYPE(nsvr::cevents::LastingHaptic, cevent)->strength;
	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getduration(nsvr_request* cevent, float* outDuration)
{
	*outDuration = AS_TYPE(nsvr::cevents::LastingHaptic, cevent)->duration;
	return NSVR_SUCCESS;
}


NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getregion(nsvr_request* cevent,nsvr_region* outRegion)
{
	auto lasting = AS_TYPE(nsvr::cevents::LastingHaptic, cevent);
//todo: reimplement region 
	//if (strcpy_s(outRegion, 32, lasting->region) == 0) {
	//	return NSVR_SUCCESS;
//	} else {
		return -1;
	//}
}

//NSVR_CORE_RETURN(int) nsvr_region_tostring(nsvr_region region, char * outRegion, uint32_t length)
//{
//	const auto& str = Locator::Translator().ToRegionString(region);
//	int retcode = strcpy_s(outRegion, length, str.data());
//	if (retcode == 0) {
//		return NSVR_SUCCESS;
//	}
//	else {
//		//should actually return NSVR_Error_something
//		return retcode;
//	}
//}


NSVR_CORE_RETURN(int) nsvr_device_event_raise(nsvr_core* core, nsvr_device_event_type type, uint64_t id)
{
	AS_TYPE(CoreFacade, core)->RaisePluginEvent(type, id);
}

NSVR_CORE_RETURN(int) nsvr_device_event_create(nsvr_device_event ** event, nsvr_device_event_type type)
{
	using namespace nsvr::pevents;

	auto newEvent = std::make_unique<device_event>( type );
	if (newEvent->valid()) {
		*event = AS_TYPE(nsvr_device_event, newEvent.release());
		return 1;
	}
	return -1;
	
}


NSVR_CORE_RETURN(int) nsvr_preset_request_getfamily(nsvr_preset_request * req, nsvr_preset_family * outFamily)
{
	*outFamily = req->family;
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_preset_request_getstrength(nsvr_preset_request * req, float * outStrength)
{
	*outStrength = req->strength;
	return NSVR_SUCCESS;
}


NSVR_CORE_RETURN(int) nsvr_playback_handle_getid(nsvr_playback_handle * handle, uint64_t * outId)
{
	*outId = handle->id;
	return NSVR_SUCCESS;
}



NSVR_CORE_RETURN(int) nsvr_request_getid(nsvr_request * request, uint64_t* request_id)
{
	*request_id  = AS_TYPE(nsvr::cevents::request_base, request)->getHandle();
	return NSVR_SUCCESS;

}

NSVR_CORE_RETURN(int) nsvr_register_rawcommand_api(nsvr_core * core, nsvr_plugin_rawcommand_api * api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<rawcommand_api>(api);
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_register_tracking_api(nsvr_core * core, nsvr_plugin_tracking_api * api)
{
	REGISTER_API(tracking_api)
}

NSVR_CORE_RETURN(int) nsvr_tracking_stream_push(nsvr_tracking_stream * stream, nsvr_quaternion * quaternion)
{
	AS_TYPE(TrackingNode, stream)->DeliverTracking(quaternion);
	return 1;
}



NSVR_CORE_RETURN(int) nsvr_register_playback_api(nsvr_core * core, nsvr_plugin_playback_api * api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<playback_api>(api);
	return NSVR_SUCCESS;
}



NSVR_CORE_RETURN(int) nsvr_register_buffer_api(nsvr_core * core, nsvr_plugin_buffer_api * api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<buffered_api>(api);
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_register_preset_api(nsvr_core * core, nsvr_plugin_preset_api * api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<preset_api>(api);
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int)  nsvr_register_request_api(nsvr_core* core, nsvr_plugin_request_api* api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<request_api>(api);
	return NSVR_SUCCESS;
}


NSVR_CORE_RETURN(int) nsvr_register_sampling_api(nsvr_core * core, nsvr_plugin_sampling_api * api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<sampling_api>(api);
	return 1;
}


NSVR_CORE_RETURN(int) nsvr_register_device_api(nsvr_core* core, nsvr_plugin_device_api* api)
{
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<device_api>(api);
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_device_event_setdeviceid(nsvr_device_event* event, uint32_t device_id)
{
	RETURN_IF_NULL(event);

	AS_TYPE(nsvr::pevents::device_event, event)->device_id = device_id;
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_device_event_settrackingstate(nsvr_device_event * event, nsvr_region region, nsvr_quaternion * quat)
{
	RETURN_IF_NULL(event);
	nsvr::pevents::device_event* realEvent = AS_TYPE(nsvr::pevents::device_event, event);
	
	try {
		nsvr::pevents::tracking_event& t = boost::get<nsvr::pevents::tracking_event>(realEvent->event);
		t.quat = *quat;
		t.region = region;
		return NSVR_SUCCESS;
	}
	catch (const boost::bad_get&) {
		return -1;
	}

}


NSVR_CORE_RETURN(int) nsvr_device_event_raise(nsvr_core* core, nsvr_device_event* event) {
	RETURN_IF_NULL(core);
	RETURN_IF_NULL(event);
	
	AS_TYPE(CoreFacade, core)->RaisePluginEvent(*AS_TYPE(nsvr::pevents::device_event, event));
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_device_event_destroy(nsvr_device_event** event) {
	RETURN_IF_NULL(event);

	
	delete AS_TYPE(nsvr::pevents::device_event, *event);
	*event = nullptr;
	return 1;

}

NSVR_CORE_RETURN(int) nsvr_device_event_setid(nsvr_device_event * event, uint64_t id)
{
	AS_TYPE(nsvr::pevents::device_event, event)->device_id = id;
	return 1;
}





