#pragma once

#include <stdint.h>

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
#define nsvr_plugin_API
#endif


#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#define NSVR_CORE_RETURN(ReturnType) NSVR_CORE_API ReturnType __cdecl
#define NSVR_PLUGIN_RETURN(ReturnType) NSVR_PLUGIN_API ReturnType __cdecl


#ifdef __cplusplus
extern "C" {
#endif

	// Opaque type representing your hardware plugin
	typedef struct nsvr_plugin nsvr_plugin;

	// Opaque type representing the core API provided to you
	typedef struct nsvr_core nsvr_core;



	// As a plugin author, you may implement three levels of functionality.
	 
	// If you implement the Basic Requirements, we will command your hardware with the simplest primitives
	// available. This is appropriate for devices like controllers, or if your API only supports buffered or 
	// "preset" haptic capabilities.
	
	// If you implement the Standard Requirements, we will pass more complex events to your plugin for processing 
	// that we cannot perform. For example, the haptic transition from a long-lived effect to a short, abrupt effect
	// requires hardware and firmware-specific knowledge. 

	// If you implement the Extended Requirements, your plugin will deliver complex playback capabilities, 
	// such as starting, stopping, and canceling effects. Additionally, this will allow the core to perform ducking, 
	// layering, and compositing operations.



	/////////////////////////
	// Plugin Registration //
	/////////////////////////

	typedef struct nsvr_plugin_api {
		typedef int(*nsvr_plugin_init)(nsvr_plugin** plugin);
		typedef int(*nsvr_plugin_configure)(nsvr_plugin* plugin, nsvr_core* core);
		typedef int(*nsvr_plugin_free)(nsvr_plugin* plugin);

		nsvr_plugin_init init;
		nsvr_plugin_configure configure;
		nsvr_plugin_free free;
	} nsvr_plugin_api;


	// This is the only function that your plugin must export.
	NSVR_PLUGIN_RETURN(int) nsvr_plugin_register(nsvr_plugin_api* api);

	
	typedef enum nsvr_region {
		nsvr_region_unknown = 0,

		nsvr_region_chest = 1000,
			nsvr_region_chest_left,
			nsvr_region_chest_right,

		nsvr_region_abs = 2000,
			nsvr_region_abs_left,
				nsvr_region_abs_upper_left,
				nsvr_region_abs_middle_left,
				nsvr_region_abs_lower_left,
			nsvr_region_abs_right,
				nsvr_region_abs_upper_right,
				nsvr_region_abs_middle_right,
				nsvr_region_abs_lower_right,

		nsvr_region_arm_left = 3000,
			nsvr_region_forearm_left,
			nsvr_region_upperarm_left,

		nsvr_region_arm_right = 4000,
			nsvr_region_forearm_right,
			nsvr_region_upperarm_right,

		nsvr_region_shoulder_left = 5000,
		nsvr_region_shoulder_right = 6000,

		nsvr_region_back = 7000,
			nsvr_region_back_left,
			nsvr_region_back_right,

		nsvr_region_hand_left = 8000,
		nsvr_region_hand_right = 9000,
		nsvr_region_leg_left = 10000,
		nsvr_region_leg_right = 11000
	} nsvr_region;

	///////////////////////////
	// Raising device events //
	///////////////////////////

	// Communication with the core is achieved by raising events.
	// For instance, when a device is connected, you may raise the device_connected event.

	typedef struct nsvr_device_event nsvr_device_event;

	enum nsvr_device_event_type {
		nsvr_device_event_unknown = 0,
		nsvr_device_event_device_connected,
		nsvr_device_event_device_disconnected,
	}; 

	NSVR_CORE_RETURN(int) nsvr_device_event_raise(nsvr_core* core, nsvr_device_event_type type, uint64_t id);
	


	typedef struct nsvr_quaternion {
		float w;
		float x;
		float y;
		float z;
	} nsvr_quaternion;

	

	typedef struct nsvr_device_ids {
		uint64_t ids[128];
		unsigned int device_count;
	} nsvr_device_ids;

	typedef enum nsvr_device_capability {
		nsvr_device_capability_none = 0,
		nsvr_device_capability_preset = 1 << 0,
		nsvr_device_capability_buffered = 1 << 1,
		nsvr_device_capability_dynamic = 1 << 2
	} nsvr_device_capability;

	typedef enum nsvr_device_type {
		nsvr_device_type_unknown = 0,
		nsvr_device_type_haptic,
		nsvr_device_type_led
	} nsvr_device_type;

	typedef struct nsvr_device_basic_info {
		uint64_t id;
		uint32_t type;
		uint32_t capabilities;
		char name[128];
		nsvr_region region;
	} nsvr_device_basic_info;

	typedef struct nsvr_device_request nsvr_device_request;
	typedef struct nsvr_plugin_device_api {
		typedef void(*nsvr_device_enumerateids)(nsvr_device_ids*, void*);
		typedef void(*nsvr_device_getinfo)(uint64_t id, nsvr_device_basic_info* info, void*);
		nsvr_device_enumerateids enumerateids_handler;
		nsvr_device_getinfo getinfo_handler;
		void* client_data;
	} nsvr_plugin_device_api;

	NSVR_CORE_RETURN(int) nsvr_register_device_api(nsvr_core* core, nsvr_plugin_device_api* api);
	
	
	//////////////////////////
	// Basic Implementation //
	//////////////////////////

	

	// If you have a buffered-style API with calls similar to SubmitHapticData(void* amplitudes, int length),
	// implement the buffer_api interface


	typedef struct nsvr_plugin_buffer_api {
		typedef void(*nsvr_buffered_submit)(uint64_t device_id, double* amplitudes, uint32_t count, void*);
		typedef void(*nsvr_buffered_getsampleduration)(double* outSampleDuration, void*);
		typedef void(*nsvr_buffered_getmaxsamples)(uint32_t* outMaxSamples, void*);
		nsvr_buffered_submit submit_handler;
		nsvr_buffered_getmaxsamples getmaxsamples_handler;
		nsvr_buffered_getsampleduration getsampleduration_handler;
		void* client_data;
	} nsvr_plugin_buffer_api;

	NSVR_CORE_RETURN(int) nsvr_register_buffer_api(nsvr_core* core, nsvr_plugin_buffer_api* api);

	// If you have a "preset"-style API with calls similar to TriggerSpecialEffect() or TriggerPulse(int microseconds),
	// implement the preset_api interface

	typedef struct nsvr_preset_request nsvr_preset_request;

	typedef struct nsvr_plugin_preset_api {
		typedef void(*nsvr_preset_handler)(nsvr_preset_request*, void*);
		nsvr_preset_handler preset_handler;
		void* client_data;
	} nsvr_plugin_preset_api;

	NSVR_CORE_RETURN(int) nsvr_register_preset_api(nsvr_core* core, nsvr_plugin_preset_api* api);
	

	// A preset family specifies a certain feeling which your haptic device produces. 
	typedef enum nsvr_preset_family {
		nsvr_preset_family_unknown = 0,
		nsvr_preset_family_bump = 1,
		nsvr_preset_family_click = 2
	} nsvr_preset_family;

	// To retrieve information about a preset request, use these functions

	NSVR_CORE_RETURN(int) nsvr_preset_request_getfamily(nsvr_preset_request* req, nsvr_preset_family* outFamily);
	
	NSVR_CORE_RETURN(int) nsvr_preset_request_getstrength(nsvr_preset_request* req, float* outStrength);

	
	

	/////////////////////////////
	// Standard Implementation //
	/////////////////////////////

	// We request your software to perform complex haptic behavior through the following event types:
	enum nsvr_request_type {
		nsvr_request_type_unknown = 0,
		nsvr_request_type_lasting_haptic = 1,
	};

	typedef struct nsvr_request nsvr_request;

	typedef struct nsvr_plugin_request_api {
		typedef void(*nsvr_request_handler)(nsvr_request*, void*);
		nsvr_request_handler request_handler;
		nsvr_request_type request_type;
		void* client_data;
	} nsvr_request_api;
	
	NSVR_CORE_RETURN(int) nsvr_register_request_api(nsvr_core* core, nsvr_plugin_request_api* api);
	
	NSVR_CORE_RETURN(int) nsvr_request_gettype(nsvr_request* cevent, nsvr_request_type* outType);

	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_geteffect(nsvr_request* cevent, uint32_t* outEffect);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getstrength(nsvr_request* cevent, float* outStrength);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getduration(nsvr_request* cevent, float* outDuration);
	NSVR_CORE_RETURN(int) nsvr_request_lastinghaptic_getregion(nsvr_request* cevent, nsvr_region* outRegion);


	
	//Question: do we want to sample a device or sample a region?

	typedef struct nsvr_sampling_sample {
		float data_0;
		float data_1;
		float data_2;
		float intensity;
	} nsvr_sampling_sample;
	typedef struct nsvr_plugin_sampling_api {
		typedef void(*nsvr_sampling_querystate)(nsvr_region region, nsvr_sampling_sample* outSample, void* client_data);
		nsvr_sampling_querystate query_handler;
		void* client_data;
	} nsvr_plugin_sampling_api;
	NSVR_CORE_RETURN(int) nsvr_register_sampling_api(nsvr_core* core, nsvr_plugin_sampling_api* api);


	/////////////////////////////
	// Extended Implementation //
	/////////////////////////////


	
	typedef struct nsvr_plugin_playback_api {
		typedef void(*nsvr_playback_pause)(uint64_t request_id, void* client_data);
		typedef void(*nsvr_playback_unpause)(uint64_t request_id, void* client_data);
		typedef void(*nsvr_playback_cancel)(uint64_t request_id, void* client_data);

		nsvr_playback_pause pause_handler;
		nsvr_playback_unpause unpause_handler;
		nsvr_playback_cancel cancel_handler;
		void* client_data;
	} nsvr_plugin_playback_api;

	NSVR_CORE_RETURN(int) nsvr_register_playback_api(nsvr_core* core, nsvr_plugin_playback_api* api);
	NSVR_CORE_RETURN(int) nsvr_request_getid(nsvr_request* request, uint64_t* request_id);



	
	typedef struct nsvr_plugin_rawcommand_api {
		typedef void(*nsvr_rawcommand_send)(uint8_t* bytes, unsigned int length, void* client_data);
		nsvr_rawcommand_send send_handler;
		void* client_data;
	} nsvr_plugin_rawcommand_api;

	NSVR_CORE_RETURN(int) nsvr_register_rawcommand_api(nsvr_core* core, nsvr_plugin_rawcommand_api* api);


	typedef struct nsvr_tracking_stream nsvr_tracking_stream;

	typedef struct nsvr_plugin_tracking_api {
		typedef void(*nsvr_tracking_beginstreaming)(nsvr_tracking_stream* stream, nsvr_region region, void* client_data);
		typedef void(*nsvr_tracking_endstreaming)(nsvr_region region, void* client_data);

		nsvr_tracking_beginstreaming beginstreaming_handler;
		nsvr_tracking_endstreaming endstreaming_handler;
		void* client_data;
	} nsvr_plugin_tracking_api;
	
	NSVR_CORE_RETURN(int) nsvr_register_tracking_api(nsvr_core* core, nsvr_plugin_tracking_api* api);
	NSVR_CORE_RETURN(int) nsvr_tracking_stream_push(nsvr_tracking_stream* stream, nsvr_quaternion* quaternion);



#ifdef __cplusplus
}
#endif


