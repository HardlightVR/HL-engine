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
	typedef struct NSVR_Plugin_t NSVR_Plugin;
	typedef struct NSVR_Core_t NSVR_Core;
	
	typedef struct NSVR_RegParams_t {
		const char* Region;
		const char* Interface;
		NSVR_Provider* Provider;
	} NSVR_RegParams;


	//Right now, temporary until we have auto json-configuration
	NSVR_PLUGIN_RETURN(int) NSVR_Configure(NSVR_Plugin* pluginPtr, NSVR_Core* core);
	NSVR_CORE_RETURN(int) NSVR_Core_RegisterNode(NSVR_Core_t* core, NSVR_RegParams params);


	NSVR_PLUGIN_RETURN(int) NSVR_Init(NSVR_Plugin** pluginPtr);

	NSVR_PLUGIN_RETURN(int) NSVR_Free(NSVR_Plugin** ptr);



#ifdef __cplusplus
}
#endif


#ifdef __cplusplus 
#ifdef NSVR_BUILDING_CORE
template <typename T>
constexpr const char* getSymbolName(void) { return "unknown"; }
#define consumer_function(type) "NSVR_Provider_Consume_" type

#define REGISTER_INTERFACE(name) \
template <> \
constexpr const char* getSymbolName<NSVR_##name>(void) { \
	return consumer_function(#name); \
}
#else
#define REGISTER_INTERFACE(name)
#endif
#else 
#define REGISTER_INTERFACE(name)
#endif