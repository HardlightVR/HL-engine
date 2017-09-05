#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"
#include "pevent.h"
#include "HardwareCoordinator.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include "nsvr_preset.h"
#include "nsvr_playback_handle.h"
#include <boost/variant/get.hpp>
#include "IHardwareDevice.h"
#include "BodyGraph.h"
#include "nsvr_core_errors.h"
#include "BodyRegion.h"
#include "ExceptionGuard.h"

#define NULL_ARGUMENT_CHECKS




#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#ifdef NULL_ARGUMENT_CHECKS
#define RETURN_IF_NULL(ptr) do { if (ptr == nullptr) { return nsvr_error_nullargument; }} while (0)
#else
#define RETURN_IF_NULL(ptr)
#endif

#define REGISTER_API(name) \
NSVR_CORE_RETURN(nsvr_result) nsvr_register_##name##(nsvr_core* core, nsvr_plugin_##name##* api) {\
RETURN_IF_NULL(core); \
RETURN_IF_NULL(api);\
return ExceptionGuard([core, api](){ \
	AS_TYPE(CoreFacade, core)->RegisterPluginApi<##name##>(api); \
	return nsvr_success; \
}); \
}




NSVR_CORE_RETURN(nsvr_result) nsvr_device_event_raise(nsvr_core* core, nsvr_device_event_type type, nsvr_device_id id)
{
	RETURN_IF_NULL(core);

	return ExceptionGuard([core, type, id]() {
		AS_TYPE(CoreFacade, core)->RaisePluginEvent(type, id);
		return nsvr_success;
	});
}




NSVR_CORE_RETURN(nsvr_result) nsvr_waveform_getname(nsvr_waveform * req, nsvr_default_waveform * outFamily)
{
	RETURN_IF_NULL(req);
	RETURN_IF_NULL(outFamily);

	*outFamily = req->waveform_id;
	return nsvr_success;
}

NSVR_CORE_RETURN(nsvr_result) nsvr_waveform_getstrength(nsvr_waveform * req, float * outStrength)
{

	RETURN_IF_NULL(req);
	RETURN_IF_NULL(outStrength);

	*outStrength = req->strength;
	return nsvr_success;
}

NSVR_CORE_RETURN(nsvr_result) nsvr_waveform_getrepetitions(nsvr_waveform * req, uint32_t* outReps)
{
	RETURN_IF_NULL(req);
	RETURN_IF_NULL(outReps);

	*outReps = req->repetitions;
	return nsvr_success;
}




NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_connect(nsvr_bodygraph* body, const char* nodeA, const char* nodeB)
{

	RETURN_IF_NULL(body);
	RETURN_IF_NULL(nodeA);
	RETURN_IF_NULL(nodeB);

	return ExceptionGuard([body, nodeA, nodeB]() {
		return AS_TYPE(BodyGraph, body)->ConnectNodes(nodeA, nodeB);
	});
}







NSVR_CORE_RETURN(nsvr_result) nsvr_tracking_stream_push(nsvr_tracking_stream * stream, nsvr_quaternion * quaternion)
{
	RETURN_IF_NULL(stream);
	RETURN_IF_NULL(quaternion);

	return ExceptionGuard([stream, quaternion]() {
		AS_TYPE(TrackingNode, stream)->DeliverTracking(quaternion);
		return nsvr_success;
	});
}

NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_region_create(nsvr_bodygraph_region ** region)
{
	RETURN_IF_NULL(region);

	*region = new nsvr_bodygraph_region();
	return 0;
}
NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_region_destroy(nsvr_bodygraph_region** region)
{

	RETURN_IF_NULL(region);

	delete *region;
	*region = nullptr;
	return 0;
}


NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_region_setlocation(nsvr_bodygraph_region* region, nsvr_bodypart bodypart, double segment_ratio, double rotation)
{
	RETURN_IF_NULL(region);

	region->segment_ratio = segment_ratio;
	region->bodypart = bodypart;
	region->rotation = rotation;
	return 0;
}

NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_associate(nsvr_bodygraph * body, const char * node, uint64_t device_id)
{
	RETURN_IF_NULL(body);
	RETURN_IF_NULL(node);

	return ExceptionGuard([body, node, device_id]() {
		AS_TYPE(BodyGraph, body)->Associate(node, device_id);
		return nsvr_success;
	});
	
}

NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_unassociate(nsvr_bodygraph * body, const char * node, uint64_t device_id)
{
	RETURN_IF_NULL(body);
	RETURN_IF_NULL(node);

	return ExceptionGuard([body, node, device_id]() {
		AS_TYPE(BodyGraph, body)->Unassociate(node, device_id);
		return nsvr_success;
	});
	
}

NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_clearassociations(nsvr_bodygraph * body, uint64_t device_id)
{

	RETURN_IF_NULL(body);

	return ExceptionGuard([body, device_id]() {
		AS_TYPE(BodyGraph, body)->ClearAssociations(device_id);
		return nsvr_success;
	});
	
}


NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_region_setboundingboxdimensions(nsvr_bodygraph_region* region, double width, double height)
{
	RETURN_IF_NULL(region);

	region->height_cm = height;
	region->width_cm = width;
	return 0;

}

NSVR_CORE_RETURN(nsvr_result) nsvr_bodygraph_createnode(nsvr_bodygraph * graph, const char * name, nsvr_bodygraph_region * region)
{

	RETURN_IF_NULL(graph);

	return ExceptionGuard([graph, name, region]() {
		return AS_TYPE(BodyGraph, graph)->CreateNode(name, region);
	});
}


// API registration


REGISTER_API(rawcommand_api)

REGISTER_API(bodygraph_api)

REGISTER_API(tracking_api)

REGISTER_API(playback_api)

REGISTER_API(buffered_api)

REGISTER_API(device_api)

REGISTER_API(waveform_api)

REGISTER_API(updateloop_api)



