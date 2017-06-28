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

	typedef struct NSVR_Core_Quaternion_t  {
		float w;
		float x;
		float y;
		float z;
	} NSVR_Core_Quaternion;


	typedef struct NSVR_Provider_t NSVR_Provider;
	typedef struct NSVR_Plugin_t NSVR_Plugin;
	typedef struct NSVR_Core_t NSVR_Core;
	


	//Right now, temporary until we have auto json-configuration
	NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Plugin* pluginPtr, NSVR_Core* core);

	typedef struct NSVR_GenericEvent_t NSVR_GenericEvent;

	typedef void(*NSVR_Consumer_Handler_t)(
		void* client_data,
		const char* region,
		const char* iface,
		const NSVR_GenericEvent* event
	);

	NSVR_CORE_RETURN(int) NSVR_Core_RegisterNode(
		NSVR_Core* core,
		NSVR_Consumer_Handler_t callback,
	
		const char* region,
		const char* iface,
		void* client_data 
	);

	NSVR_CORE_RETURN(int) NSVR_Core_Tracking_Submit(NSVR_Core* core, const char* region, const NSVR_Core_Quaternion* update);
	NSVR_CORE_RETURN(int) NSVR_Core_ConnectionStatus_Submit(NSVR_Core* core, bool isDeviceConnected);
	
	NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Plugin** pluginPtr);

	NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Plugin** ptr);



#ifdef __cplusplus
}
#endif


