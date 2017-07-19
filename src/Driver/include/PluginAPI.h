#pragma once

#include <stdint.h>
//NOTE: We will support 32 bit only for now.

#ifdef _WIN32
#ifdef NSVR_BUILDING_CORE
#define NSVR_CORE_API __declspec(dllexport)
#define NSVR_PLUGIN_API 
#else  
#define NSVR_CORE_API __declspec(dllimport) 
#define NSVR_PLUGIN_API __declspec(dllexport)
#endif  
#else
#define NSVR_CORE_API
#define NSVR_PLUGIN_API
#endif


#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#define NSVR_CORE_RETURN(ReturnType) NSVR_CORE_API ReturnType __cdecl
#define NSVR_PLUGIN_RETURN(ReturnType) NSVR_PLUGIN_API ReturnType __cdecl

#define NSVR_CORE_EVENT(name, version) \
struct nsvr_request_##name##_v##version 


#define NSVR_CORE_EVENT_LATEST(name, version) \
const unsigned int nsvr_request_##name##_latest = version; \
typedef struct nsvr_request_##name##_v##version nsvr_request_##name

#ifdef __cplusplus
extern "C" {
#endif


	enum NSVR_Region { NSVR_Region_None, NSVR_Region_LeftChest, NSVR_Region_RightChest };

	typedef struct NSVR_Core_Quaternion_t {
		float w;
		float x;
		float y;
		float z;
	} NSVR_Core_Quaternion;


	typedef struct NSVR_Provider_t NSVR_Provider;
	typedef struct NSVR_Plugin_t NSVR_Plugin;
	typedef struct NSVR_Core_Ctx_s nsvr_core_ctx;


	enum nsvr_device_event_type {
		nsvr_device_event_unknown = 0,
		nsvr_device_event_device_connected = 1,
		nsvr_device_event_device_disconnected = 2,
		nsvr_device_event_tracking_update = 3
		

	};

	enum nsvr_request_type {
		nsvr_request_type_unknown = 0,
		nsvr_request_type_brief_haptic = 1,
		nsvr_request_type_lasting_haptic = 2,
		nsvr_request_type_playback_statechange = 3,
		nsvr_request_type_realtime_request = 4,
		nsvr_request_type_preset_erm_waveform = 5,
		nsvr_request_type_direct_erm_drive = 6
	

	};
	

	/*
	We have nsvr_device_event
		- a device specific event that has happened on the hardware side
		- and is now bubbling up to the runtime
	We have nsvr_request
		- a request for the hardware to perform some action
		- such as play a brief haptic effect
	
	*/

	typedef struct nsvr_device nsvr_device;
	typedef struct nsvr_node nsvr_node;

	//node_settype(nodetype::haptic?)
	NSVR_CORE_RETURN(int) nsvr_node_create(nsvr_node** node);
	NSVR_CORE_RETURN(int) nsvr_node_setdisplayname(nsvr_node* node, const char* name);
	NSVR_CORE_RETURN(int) nsvr_node_destroy(nsvr_node** node);
	
	
	typedef struct nsvr_querystate nsvr_querystate;
	NSVR_CORE_RETURN(int) nsvr_querystate_create(nsvr_querystate** querystate);
	NSVR_CORE_RETURN(int) nsvr_querystate_register(nsvr_querystate* querystate, nsvr_core_ctx* core);
		//querystate_setdevice
	NSVR_CORE_RETURN(int) nsvr_querystate_addnode(nsvr_querystate* state, nsvr_node* node, float x, float y, float z);
	NSVR_CORE_RETURN(int) nsvr_querystate_updatenode(nsvr_node* node, bool active);


	//Realization
	//Some things that we support might be like advanced Oculus or OpenVR drivers
	//they have preestablished haptics

	//other things are just low level drivers 
	//and want us to do the bulk of the work
	
	//this sort of necessitates two different api designs?


	typedef struct nsvr_request nsvr_request;


	typedef struct nsvr_device_event nsvr_device_event;

	NSVR_CORE_RETURN(int) nsvr_request_gettype(nsvr_request* cevent, nsvr_request_type* outType);
	NSVR_CORE_RETURN(int) nsvr_request_briefhaptic_geteffect(nsvr_request* cevent, uint32_t* outEffect);
	NSVR_CORE_RETURN(int) nsvr_request_briefhaptic_getstrength(nsvr_request* cevent, float* outStrength);
	NSVR_CORE_RETURN(int) nsvr_request_briefhaptic_getregion(nsvr_request* cevent, char* outRegion); 




	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getid(nsvr_request* cevent, uint64_t* outId);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_geteffect(nsvr_request* cevent, uint32_t* outEffect);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getstrength(nsvr_request* cevent, float* outStrength);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getduration(nsvr_request* cevent, float* outDuration);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getregion(nsvr_request* cevent, char* outRegion);



	enum nsvr_playback_statechange_command {
		nsvr_playback_statechange_unknown = 0,
		nsvr_playback_statechange_pause = 1,
		nsvr_playback_statechange_unpause = 2,
		nsvr_playback_statechange_cancel = 3
	};




	NSVR_CORE_RETURN(int) nsvr_request_playback_statechange_getid(nsvr_request* cevent, uint64_t* outId);
	NSVR_CORE_RETURN(int) nsvr_request_playback_statechange_getcommand(nsvr_request* cevent, nsvr_playback_statechange_command* outCommand);








	typedef void(*nsvr_request_handler)(nsvr_request* event, nsvr_request_type type, void* user_data);

	typedef struct nsvr_request_callback {
		nsvr_request_handler handler;
		void* user_data;
	} nsvr_request_callback;

	/******/
	//For plugin to receive data from core
	NSVR_CORE_RETURN(int) nsvr_register_request_handler(nsvr_core_ctx* core, nsvr_request_type eventType, nsvr_request_callback cb);


	NSVR_CORE_RETURN(int) nsvr_device_event_create(nsvr_device_event** event, nsvr_device_event_type type);

	NSVR_CORE_RETURN(int) nsvr_device_event_destroy(nsvr_device_event** event);
	NSVR_CORE_RETURN(int) nsvr_device_event_raise(nsvr_core_ctx* core, nsvr_device_event* event);
	
	typedef struct nsvr_direct_request nsvr_direct_request;
	typedef struct nsvr_buffered_request nsvr_buffered_request;
	typedef struct nsvr_preset_request nsvr_preset_request;

	/** LOWLEVEL HAPTIC DEVICE API **/
	typedef enum nsvr_preset_family {
		nsvr_preset_family_unknown = 0,
		nsvr_preset_family_bump = 1,
		nsvr_preset_family_click = 2
	} nsvr_preset_family;

	
	typedef void(*nsvr_direct_handler)(nsvr_direct_request* req, void* client_data);
	typedef void(*nsvr_buffered_handler)(nsvr_buffered_request* req, void* client_data);
	typedef void(*nsvr_preset_handler)(nsvr_preset_request* req, void* client_data);

	//should incorporate region at a lower level than x_getregion. Maybe as a param?
	NSVR_CORE_RETURN(int) nsvr_register_preset_handler(nsvr_core_ctx* core, nsvr_preset_handler handler, void* client_data);
	NSVR_CORE_RETURN(int) nsvr_register_buffered_handler(nsvr_core_ctx* core, nsvr_buffered_handler handler, void* client_data);
	NSVR_CORE_RETURN(int) nsvr_register_direct_handler(nsvr_core_ctx* core, nsvr_direct_handler, void* client_data);

	NSVR_CORE_RETURN(int) nsvr_preset_request_getfamily(nsvr_preset_request* req, nsvr_preset_family* outFamily);
	NSVR_CORE_RETURN(int) nsvr_preset_request_getstrength(nsvr_preset_request* req, float* outStrength);
	/** END LOWLEVEL API **/

	/** BEGIN HIGHLEVEL API **/
	struct nsvr_playback_system_fntable {

	};
	NSVR_CORE_RETURN(int) nsvr_register_playback_system()
	/** END HIGHLEVEL API/



	NSVR_CORE_RETURN(int) nsvr_device_event_setdeviceid(nsvr_device_event* event, uint32_t device_id);

	NSVR_CORE_RETURN(int) nsvr_device_event_settrackingstate(nsvr_device_event* event, const char* region, NSVR_Core_Quaternion* quat);
		
		
		/******/

	
	NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Plugin* pluginPtr, nsvr_core_ctx* core);

	NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Plugin** pluginPtr);


	NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Plugin** ptr);



#ifdef __cplusplus
}
#endif


