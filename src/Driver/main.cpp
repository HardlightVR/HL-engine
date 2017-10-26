
#include "stdafx.h"
#include <conio.h>
#include "runtime_include/NSDriverApi.h"
#include <memory>
#include <boost/dll.hpp>
#include <functional>
#include "PluginInstance.h"
#include "IdentificationService.h"
int main()
{





	using driver_create_t = std::function<int(hvr_platform**)>;
	using driver_start_t = std::function<int(hvr_platform*)>;
	using driver_stop_t = std::function<int(hvr_platform*)>;
	using driver_destroy_t = std::function<void(hvr_platform**)>;
	using driver_version_t = std::function<unsigned int(void)>;

	
	using driver_setupdiag_t = std::function<int(hvr_platform*, hvr_diagnostics_ui*)>;
	using driver_drawdiag_t = std::function<int(hvr_platform*)>;

	boost::system::error_code loadFailure;
	auto driver = std::make_unique<boost::dll::shared_library>("HardlightPlatform", boost::dll::load_mode::append_decorations, loadFailure);
	if (loadFailure) {
		std::cout << "Couldn't load HardlightPlatform.dll\n";
	}

	driver_create_t driver_create;

	if (!tryLoad(driver, "hvr_platform_create", driver_create)) {
		std::cout << "Couldn't find NSVR_Driver_Create()\n";
	}

	driver_start_t driver_start;

	if (!tryLoad(driver, "hvr_platform_startup", driver_start)) {
		std::cout << "Couldn't find NSVR_Driver_Create()\n";
	}
	driver_stop_t driver_stop;

	if (!tryLoad(driver, "hvr_platform_shutdown", driver_stop)) {
		std::cout << "Couldn't find NSVR_Driver_Shutdown()\n";
	}
	driver_destroy_t driver_destroy;

	if (!tryLoad(driver, "hvr_platform_destroy", driver_destroy)) {
		std::cout << "Couldn't find NSVR_Driver_Destroy()\n";
	}

	driver_version_t driver_getversion;
	if (!tryLoad(driver, "hvr_platform_getversion", driver_getversion)) {
		std::cout << "Couldn't find NSVR_Driver_GetVersion()\n";
	}


	driver_setupdiag_t driver_setupdiag;
	if (!tryLoad(driver, "hvr_platform_setupdiagnostics", driver_setupdiag)) {
		std::cout << "Couldn't find NSVR_Driver_SetupDiagnostics()\n";
	}

	driver_drawdiag_t driver_drawdiag;
	if (!tryLoad(driver, "hvr_platform_updatediagnostics", driver_drawdiag)) {
		std::cout << "Couldn't find NSVR_Driver_DrawDiagnostics()\n";
	}

	std::cout << "========= NSVREngine Version " << HVR_VERSION_STRING << " =========\n";
	hvr_platform* context = nullptr;
	driver_create(&context);
	driver_start(context);






	std::cin.get();
	driver_stop(context);
	driver_destroy(&context);
	assert(context == nullptr);
	return 0;
}

