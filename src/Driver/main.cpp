
#include "stdafx.h"
#include <conio.h>
#include "NSDriverApi.h"
#include <memory>
#include <boost/dll.hpp>
#include <functional>
#include "PluginInstance.h"
#include "IdentificationService.h"
int main()
{





	using driver_create_t = std::function<hvr_platform*(void)>;
	using driver_start_t = std::function<void(NSVR_Driver_Context_t*)>;
	using driver_stop_t = std::function<void(NSVR_Driver_Context_t*)>;
	using driver_destroy_t = std::function<void(NSVR_Driver_Context_t*)>;
	using driver_version_t = std::function<unsigned int(void)>;

	
	using driver_setupdiag_t = std::function<int(NSVR_Driver_Context_t*, NSVR_Diagnostics_Menu*)>;
	using driver_drawdiag_t = std::function<int(NSVR_Driver_Context_t*)>;

	boost::system::error_code loadFailure;
	auto driver = std::make_unique<boost::dll::shared_library>("HardlightPlatform", boost::dll::load_mode::append_decorations, loadFailure);
	if (loadFailure) {
		std::cout << "Couldn't load HardlightPlatform.dll\n";
	}

	driver_create_t driver_create;

	if (!tryLoad(driver, "NSVR_Driver_Create", driver_create)) {
		std::cout << "Couldn't find NSVR_Driver_Create()\n";
	}

	driver_start_t driver_start;

	if (!tryLoad(driver, "NSVR_Driver_StartThread", driver_start)) {
		std::cout << "Couldn't find NSVR_Driver_Create()\n";
	}
	driver_stop_t driver_stop;

	if (!tryLoad(driver, "NSVR_Driver_Shutdown", driver_stop)) {
		std::cout << "Couldn't find NSVR_Driver_Shutdown()\n";
	}
	driver_destroy_t driver_destroy;

	if (!tryLoad(driver, "NSVR_Driver_Destroy", driver_destroy)) {
		std::cout << "Couldn't find NSVR_Driver_Destroy()\n";
	}

	driver_version_t driver_getversion;
	if (!tryLoad(driver, "NSVR_Driver_GetVersion", driver_getversion)) {
		std::cout << "Couldn't find NSVR_Driver_GetVersion()\n";
	}


	driver_setupdiag_t driver_setupdiag;
	if (!tryLoad(driver, "NSVR_Driver_SetupDiagnostics", driver_setupdiag)) {
		std::cout << "Couldn't find NSVR_Driver_SetupDiagnostics()\n";
	}

	driver_drawdiag_t driver_drawdiag;
	if (!tryLoad(driver, "NSVR_Driver_DrawDiagnostics", driver_drawdiag)) {
		std::cout << "Couldn't find NSVR_Driver_DrawDiagnostics()\n";
	}

	unsigned int version = driver_getversion();
	std::cout << "========= NSVREngine Version " << (version >> 16) << "." << ((version << 16) >> 16) << " =========\n";
	hvr_platform* context = driver_create();
	driver_start(context);






	std::cin.get();
	driver_stop(context);
	driver_destroy(context);
	return 0;
}

