#pragma once


#ifdef NS_UNREAL_SDK_EXPORTS  
#define NS_UNREAL_SDK_API __declspec(dllexport)
#else  
#define NS_UNREAL_SDK_API __declspec(dllimport)   
#endif  

#include "NSEngine.h"


extern "C" {
	NS_UNREAL_SDK_API NSEngine* __stdcall NSEngine_Create();
	NS_UNREAL_SDK_API void __stdcall NSEngine_Destroy(NSEngine* ptr);
	NS_UNREAL_SDK_API void __stdcall NSEngine_Update(NSEngine* ptr);
	NS_UNREAL_SDK_API bool __stdcall NSEngine_Shutdown(NSEngine* ptr);
}