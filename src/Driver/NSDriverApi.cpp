#include "stdafx.h"
#include "NSDriverApi.h"
#include "Driver.h"
#include "Locator.h"
#include "EnumTranslator.h"
#include "logging_initialization.h"

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)



HVR_RETURN(int) hvr_platform_create(hvr_platform** platform)
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());
	
	initialize_logging();

	*platform = AS_TYPE(hvr_platform, new Driver());
	return 1;
}

HVR_RETURN(void) hvr_platform_destroy(hvr_platform** ptr)
{
	delete AS_TYPE(Driver, *ptr);
	*ptr = nullptr;
}

HVR_RETURN(int) hvr_platform_shutdown(hvr_platform * ptr)
{
	return AS_TYPE(Driver, ptr)->Shutdown();
}

HVR_RETURN(int) hvr_platform_startup(hvr_platform * ptr)
{
	AS_TYPE(Driver, ptr)->StartThread();
}

HVR_RETURN(unsigned int) hvr_platform_getversion(void)
{
	return HVR_PLATFORM_API_VERSION;
}

HVR_RETURN(bool) hvr_platform_isdllcompatible(void)
{
	unsigned int major = hvr_platform_getversion() >> 16;
	return major == HVR_PLATFORM_API_VERSION_MAJOR;
}

HVR_RETURN(int) hvr_platform_setupdiagnostics(hvr_platform* ptr, hvr_diagnostics_ui * api)
{
	AS_TYPE(Driver, ptr)->ProvideRenderingApi(api);
	return 1;
}

HVR_RETURN(int) hvr_platform_updatediagnostics(hvr_platform* ptr)
{
	AS_TYPE(Driver, ptr)->DrawDiagnostics();
	return 1;
}
