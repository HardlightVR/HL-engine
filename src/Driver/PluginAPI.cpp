#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"
#include "pevent.h"
#include "HardwareCoordinator.h"
#include "cevent_internal.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include "nsvr_preset.h"
#define NULL_ARGUMENT_CHECKS




#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#ifdef NULL_ARGUMENT_CHECKS
#define RETURN_IF_NULL(ptr) do { if (ptr == nullptr) { return (int) -1; }} while (0)
#else
#define RETURN_IF_NULL(ptr)
#endif


enum returnTypes {
	NSVR_SUCCESS = 1
};
NSVR_CORE_RETURN(int) nsvr_request_gettype(nsvr_request * cevent, nsvr_request_type * outType)
{
	*outType = AS_CTYPE(nsvr::cevents::request_base, cevent)->getType();
	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) nsvr_request_briefhaptic_geteffect(nsvr_request* cevent, uint32_t* outEffect) {
	*outEffect = AS_TYPE(nsvr::cevents::BriefHaptic, cevent)->effect;
	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) nsvr_request_briefhaptic_getstrength(nsvr_request* cevent, float* outStrength) {
	*outStrength = AS_TYPE(nsvr::cevents::BriefHaptic, cevent)->strength;
	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) nsvr_request_briefhaptic_getregion(nsvr_request* cevent, char* outRegion) {
	//precondition: buffer outRegion must be able to hold 32 characters
	//postcondition: will write a string into the buffer not exceeding 32 characters, and will be null terminated
	auto brief = AS_TYPE(nsvr::cevents::BriefHaptic, cevent);
	if (strcpy_s(outRegion, 32, brief->region) == 0) {
		
		return NSVR_SUCCESS;
	}
	else {
		return -1;
	}
}


NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getid(nsvr_request* cevent, uint64_t* outId) {
	*outId = AS_TYPE(nsvr::cevents::LastingHaptic, cevent)->parent_id;
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


NSVR_CORE_RETURN(int) nsvr_request_playback_statechange_getid(nsvr_request * cevent, uint64_t * outId)
{
	*outId = AS_TYPE(nsvr::cevents::PlaybackStateChange, cevent)->parent_id;
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_request_playback_statechange_getcommand(nsvr_request* cevent, nsvr_playback_statechange_command* outCommand) {
	*outCommand = AS_TYPE(nsvr::cevents::PlaybackStateChange, cevent)->command;
	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getregion(nsvr_request* cevent, char* outRegion)
{
	auto lasting = AS_TYPE(nsvr::cevents::LastingHaptic, cevent);
	if (memcpy_s(outRegion, 32, lasting->region, strlen(lasting->region)) == 0) {
		return NSVR_SUCCESS;
	}
	else {
		return -1;
	}
}

NSVR_CORE_RETURN(int) nsvr_device_event_create(nsvr_device_event ** event, nsvr_device_event_type type)
{
	using namespace nsvr::pevents;
	
	device_event* possible = device_event::make(type);

	if (possible == nullptr) {
		return -1;
	}
	else {
		*event = AS_TYPE(nsvr_device_event, possible);
		return 1;
	}
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

NSVR_CORE_RETURN(int) nsvr_register_preset_handler(nsvr_core_ctx * core, nsvr_preset_handler handler, void * client_data)
{
	auto& lowlevel = AS_TYPE(HardwareDataModel, core)->LowLevel();
	lowlevel.RegisterPreset(handler, client_data);
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_register_buffered_handler(nsvr_core_ctx * core, nsvr_buffered_handler handler, void * client_data)
{
	auto& lowlevel = AS_TYPE(HardwareDataModel, core)->LowLevel();
	lowlevel.RegisterBuffered(handler, client_data);
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_register_direct_handler(nsvr_core_ctx * core, nsvr_direct_handler handler, void * client_data)
{
	auto& lowlevel = AS_TYPE(HardwareDataModel, core)->LowLevel();
	lowlevel.RegisterDirect(handler, client_data);
	return 1;

}

NSVR_CORE_RETURN(int) nsvr_register_request_handler(nsvr_core_ctx* core, nsvr_request_type eventType,nsvr_request_callback cb){
	RETURN_IF_NULL(core);


	HardwareDataModel* model = AS_TYPE(HardwareDataModel, core);
	auto& coordinator = model->GetParentCoordinator();
	coordinator.Register(eventType, cb.handler, 1, cb.user_data);
	return 1;

}

NSVR_CORE_RETURN(int) nsvr_device_event_setdeviceid(nsvr_device_event* event, uint32_t device_id)
{
	RETURN_IF_NULL(event);

	AS_TYPE(nsvr::pevents::device_event, event)->device_id = device_id;
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_device_event_settrackingstate(nsvr_device_event * event, const char* region, NSVR_Core_Quaternion * quat)
{
	RETURN_IF_NULL(event);
	AS_TYPE(nsvr::pevents::tracking_update, event)->region = std::string(region);
	AS_TYPE(nsvr::pevents::tracking_update, event)->quat = *quat;
	return NSVR_SUCCESS;
}


NSVR_CORE_RETURN(int) nsvr_device_event_raise(nsvr_core_ctx* core, nsvr_device_event* event) {
	RETURN_IF_NULL(core);
	RETURN_IF_NULL(event);
	
	AS_TYPE(HardwareDataModel, core)->Raise(*AS_TYPE(nsvr::pevents::device_event, event));
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_device_event_destroy(nsvr_device_event** event) {
	RETURN_IF_NULL(event);

	
	delete AS_TYPE(nsvr::pevents::device_event, *event);
	*event = nullptr;
	return 1;

}

NSVR_CORE_RETURN(int) nsvr_querystate_register(nsvr_querystate * querystate, nsvr_core_ctx * core)
{
	HardwareDataModel* model = AS_TYPE(HardwareDataModel, core);
	//todo: implement

	return NSVR_SUCCESS;
}
NSVR_CORE_RETURN(int) NSVR_Configuration_GetCallback(const NSVR_Configuration * config, const char * name, nsvr_callback* callback)
{

	RETURN_IF_NULL(config);
	RETURN_IF_NULL(name);
	RETURN_IF_NULL(callback);

	if (config->Callbacks.find(name) != config->Callbacks.end()) {
		const auto& cb = config->Callbacks.at(name);
		callback->callback = cb.callback;
		callback->context = cb.context;
		return 1;
	}
	
	return -1;

}





struct nsvr_node {
	boost::uuids::uuid id;
	std::string displayname;
	bool active;
	nsvr_node() : id(boost::uuids::random_generator()()), active{false}
	{

	}
};



NSVR_CORE_RETURN(int) nsvr_node_create(nsvr_node ** node)
{
	*node = new nsvr_node{};
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_node_setdisplayname(nsvr_node* node, const char* name) {
	node->displayname = std::string(name);
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_node_destroy(nsvr_node** node) {
	delete *node;
	*node = nullptr;
	return NSVR_SUCCESS;
}


struct nsvr_querystate {
	struct vec3 {
		float x; float y; float z;
	};
	struct graph_node {
		vec3 location;
		nsvr_node* node;
	};
	std::vector<graph_node> nodes;
};

NSVR_CORE_RETURN(int) nsvr_querystate_create(nsvr_querystate ** querystate)
{
	*querystate = new nsvr_querystate{};
	return NSVR_SUCCESS;
}

NSVR_CORE_RETURN(int) nsvr_querystate_addnode(nsvr_querystate* state, nsvr_node * node, float x, float y, float z)
{
	nsvr_querystate::graph_node newNode;
	newNode.location = nsvr_querystate::vec3{ x, y, z };
	newNode.node = node;
	state->nodes.push_back(std::move(newNode));
	return NSVR_SUCCESS;

	
}
NSVR_CORE_RETURN(int) nsvr_querystate_updatenode(nsvr_node * node, bool active)
{
	node->active = active;
	return NSVR_SUCCESS;
}