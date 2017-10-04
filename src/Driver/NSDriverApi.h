#pragma once


#ifdef NS_DRIVER_EXPORTS  
#define HVR_PLATFORM_API __declspec(dllexport)
#else  
#define HVR_PLATFORM_API __declspec(dllimport)   
#endif  


#define HVR_PLATFORM_API_VERSION_MAJOR 0
#define HVR_PLATFORM_API_VERSION_MINOR 4
#define HVR_PLATFORM_API_VERSION ((HVR_PLATFORM_API_VERSION_MAJOR << 16) | HVR_PLATFORM_API_VERSION_MINOR)
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
	HVR_RETURN(bool) hvr_platform_isdllcompatible(void);

	struct hvr_diagnostics_ui {
		typedef void(*make_keyval)(const char* key, const char* val);
		typedef bool(*make_button)(const char* label);

		make_keyval keyval;
		make_button button;

	};



	HVR_PLATFORM_API int  __cdecl hvr_platform_setupdiagnostics(hvr_platform* ptr, hvr_diagnostics_ui* api);
	HVR_PLATFORM_API int __cdecl hvr_platform_updatediagnostics(hvr_platform* ptr);

#ifdef __cplusplus
}
#endif