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


#ifdef __cplusplus
extern "C" {
#endif

	enum NSVR_Region {NSVR_Region_None, NSVR_Region_LeftChest, NSVR_Region_RightChest};

	typedef struct NSVR_Provider_t NSVR_Provider;

	NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Provider** pluginPtr);
	NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Provider** ptr);

	NSVR_PLUGIN_RETURN(int) NSVR_RegisterRegions(NSVR_Provider* plugin, NSVR_Region* requestedRegions);
	
#ifdef __cplusplus
}
#endif