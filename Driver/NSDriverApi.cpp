#include "stdafx.h"
#include "NSDriverApi.h"
#include "Driver.h"
#include "Locator.h"
#include "EnumTranslator.h"
#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

NS_DRIVER_API NSVR_Driver_Context_t* __stdcall NSVR_Driver_Create()
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());

	return AS_TYPE(NSVR_Driver_Context_t, new Driver());
}

NS_DRIVER_API void __stdcall NSVR_Driver_Destroy(NSVR_Driver_Context_t* ptr)
{
	if (!ptr) {
		return;
	}

	delete AS_TYPE(Driver, ptr);
}

NS_DRIVER_API bool __stdcall NSVR_Driver_Shutdown(NSVR_Driver_Context_t * ptr)
{
	return AS_TYPE(Driver, ptr)->Shutdown();
}

NS_DRIVER_API void __stdcall NSVR_Driver_StartThread(NSVR_Driver_Context_t * ptr)
{
	AS_TYPE(Driver, ptr)->StartThread();
}

NS_DRIVER_API unsigned int __stdcall NSVR_Driver_GetVersion(void)
{
	return NS_DRIVER_API_VERSION;
}

NS_DRIVER_API int __stdcall NSVR_Driver_IsCompatibleDLL(void)
{
	unsigned int major = NSVR_Driver_GetVersion() >> 16;
	return major == NS_DRIVER_API_VERSION_MAJOR;
}
