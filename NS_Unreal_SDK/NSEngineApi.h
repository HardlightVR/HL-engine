#pragma once


#ifdef NS_UNREAL_SDK_EXPORTS  
#define NS_UNREAL_SDK_API __declspec(dllexport)
#else  
#define NS_UNREAL_SDK_API __declspec(dllimport)   
#endif  

struct NSEngineStruct;
typedef struct NSEngineStruct* NSEnginePtr;

extern "C" {
	NS_UNREAL_SDK_API NSEnginePtr __stdcall NSEngine_Create();
	NS_UNREAL_SDK_API void __stdcall NSEngine_Destroy(NSEnginePtr ptr);
	NS_UNREAL_SDK_API void __stdcall NSEngine_Update(NSEnginePtr ptr);
}