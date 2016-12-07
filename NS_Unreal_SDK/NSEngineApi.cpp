#include "NSEngineApi.h"
#include "NSEngine.h"

extern "C" {
	NS_UNREAL_SDK_API NSEngine* __stdcall NSEngine_Create()
	{
		return  new NSEngine();
	}

	NS_UNREAL_SDK_API void __stdcall NSEngine_Destroy(NSEngine* ptr)
	{
		delete ptr;
	}

	NS_UNREAL_SDK_API void __stdcall NSEngine_Update(NSEngine* ptr)
	{
		ptr->Update();
	}

	NS_UNREAL_SDK_API bool __stdcall NSEngine_Shutdown(NSEngine* ptr) {
		return ptr->Shutdown();
	}
	NS_UNREAL_SDK_API void __stdcall NSEngine_StartThread(NSEngine * ptr)
	{
		ptr->StartThread();
	}
}