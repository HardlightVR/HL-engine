#pragma once
#include <stdint.h>

#ifdef NS_DRIVER_EXPORTS  
#define HVR_PLATFORM_API __declspec(dllexport)
#else  
#define HVR_PLATFORM_API __declspec(dllimport)   
#endif  


#define HVR_PLATFORM_API_VERSION_MAJOR 1
#define HVR_PLATFORM_API_VERSION_MINOR 0
#define HVR_PLATFORM_API_VERSION_PATCH 6

#define HVR_PLATFORM_API_VERSION ((HVR_PLATFORM_API_VERSION_MAJOR << 24) | (HVR_PLATFORM_API_VERSION_MINOR << 16) | HVR_PLATFORM_API_VERSION_PATCH)

#if !defined(HLVR_TOSTRING)
#define	HLVR_TOSTRINGH(x) #x
#define HLVR_TOSTRING(x) HLVR_TOSTRINGH(x)
#endif

#if !defined(HVR_VERSION_STRING)
#define HVR_VERSION_STRING HLVR_TOSTRING(HVR_PLATFORM_API_VERSION_MAJOR.HVR_PLATFORM_API_VERSION_MINOR.HVR_PLATFORM_API_VERSION_PATCH)
#endif



#define HVR_RETURN(ReturnType) HVR_PLATFORM_API ReturnType __cdecl

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct hvr_platform hlvr_platform;

	HVR_RETURN(int) hvr_platform_create(hvr_platform** platform);
	HVR_RETURN(void) hvr_platform_destroy(hvr_platform** ptr);
	HVR_RETURN(int) hvr_platform_shutdown(hvr_platform* ptr);
	HVR_RETURN(int) hvr_platform_startup(hvr_platform* ptr);
	HVR_RETURN(unsigned) hvr_platform_getversion(void);
	static bool hvr_platform_isdllcompatible(void)
	{
		unsigned int major = hvr_platform_getversion() >> 24;
		return major == HVR_PLATFORM_API_VERSION_MAJOR;
	}


	struct hvr_diagnostics_ui {
		typedef void(*make_keyval)(const char* key, const char* val);
		typedef bool(*make_slider_int)(const char* key, int* v, int min, int max);
		typedef bool(*make_button)(const char* label);
		typedef void(*push_log)(const char* msg);

		make_keyval keyval;
		make_button button;
		push_log log;
		make_slider_int slider_int;

	};

	//Core platform abstractions:

	//Devices - Nodes - Plugins

	typedef uint32_t hvr_plugin_id;

	struct hvr_plugin_list {
		hvr_plugin_id ids[128];
		uint32_t count;
	};

	struct hvr_plugin_info {
		char name[512];

	};
	HVR_RETURN(int) hvr_platform_enumerateplugins(hvr_platform* platform, hvr_plugin_list* outPlugins);

	HVR_RETURN(int) hvr_platform_getplugininfo(hvr_platform* platform, hvr_plugin_id id, hvr_plugin_info* outInfo);
	HVR_RETURN(int) hvr_platform_setupdiagnostics(hvr_platform* ptr, hvr_diagnostics_ui* api);
	HVR_RETURN(int) hvr_platform_updatediagnostics(hvr_platform* ptr, hvr_plugin_id pluginId);


	
	HVR_RETURN(int) hvr_platform_getcurrentsuitstate(hvr_platform* ptr, int* outState);



	typedef struct hvr_quaternion {
		float w;
		float x;
		float y;
		float z;
	} hvr_quaternion;

	typedef void(*hvr_device_tracking_datasource)(uint32_t, hvr_quaternion* quat);

#ifdef __cplusplus
}
#endif