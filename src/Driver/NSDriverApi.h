#pragma once


#ifdef NS_DRIVER_EXPORTS  
#define NS_DRIVER_API __declspec(dllexport)
#else  
#define NS_DRIVER_API __declspec(dllimport)   
#endif  


#define NS_DRIVER_API_VERSION_MAJOR 0
#define NS_DRIVER_API_VERSION_MINOR 2
#define NS_DRIVER_API_VERSION ((NS_DRIVER_API_VERSION_MAJOR << 16) | NS_DRIVER_API_VERSION_MINOR)

#ifdef __cplusplus
extern "C" {
#endif

	struct NSVR_Driver_Context;
	typedef struct NSVR_Driver_Context NSVR_Driver_Context_t;

	NS_DRIVER_API NSVR_Driver_Context_t* __cdecl NSVR_Driver_Create();
	NS_DRIVER_API void __cdecl NSVR_Driver_Destroy(NSVR_Driver_Context_t* ptr);
	NS_DRIVER_API bool __cdecl NSVR_Driver_Shutdown(NSVR_Driver_Context_t* ptr);
	NS_DRIVER_API void __cdecl NSVR_Driver_StartThread(NSVR_Driver_Context_t* ptr);
	NS_DRIVER_API unsigned int __cdecl NSVR_Driver_GetVersion(void);
	NS_DRIVER_API  int __cdecl NSVR_Driver_IsCompatibleDLL(void);


#ifdef __cplusplus
}
#endif