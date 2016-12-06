#include "NSEngineApi.h"
#include "NSEngine.h"

extern "C" {
	NS_UNREAL_SDK_API NSEnginePtr __stdcall NSEngine_Create()
	{
		return  reinterpret_cast<NSEnginePtr>(new NSEngine());
	}

	NS_UNREAL_SDK_API void __stdcall NSEngine_Destroy(NSEnginePtr ptr)
	{
		delete reinterpret_cast<NSEngine*>(ptr);
	}

	NS_UNREAL_SDK_API void __stdcall NSEngine_Update(NSEnginePtr ptr)
	{
		reinterpret_cast<NSEngine*>(ptr)->Update();
	}
}