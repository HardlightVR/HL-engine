#pragma once

#ifdef NSVR_PLUGIN_EXPORTS 
#define NSVR_CORE_API __declspec(dllimport) 
#define NSVR_PLUGIN_NOMANGLING comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
#else  
#define NSVR_CORE_API __declspec(dllexport)
#define NSVR_PLUGIN_NOMANGLING
#endif  

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

#define NSVR_RETURN(ReturnType) NSVR_PLUGIN_API ReturnType __stdcall



#ifdef __cplusplus
extern "C" {
#endif

	enum NSVR_Region {NSVR_Region_None, NSVR_Region_LeftChest, NSVR_Region_RightChest};

	typedef struct NSVR_Plugin_t NSVR_Plugin;

	int NSVR_Init(NSVR_Plugin** pluginPtr);
	int NSVR_Free(NSVR_Plugin** ptr);

//	int NSVR_ReceiveCommand(NSVR_Plugin* ptr, const char* command, const char* data, unsigned int length);
	int NSVR_RegisterRegions(NSVR_Plugin* plugin, NSVR_Region* requestedRegions);
	int NSVR_DirectControl(NSVR_Plugin* plugin, NSVR_Region region, const char* data, unsigned int length);
	
#ifdef __cplusplus
}
#endif