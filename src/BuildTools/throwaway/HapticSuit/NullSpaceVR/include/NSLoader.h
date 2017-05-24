#pragma once

#include "NSLoader_Errors.h"

#include <stdint.h>

#ifdef NSLOADER_EXPORTS
#define NSLOADER_API __declspec(dllexport) 
#else
#define NSLOADER_API __declspec(dllimport) 
#endif

#define NSLOADER_API_VERSION_MAJOR 0
#define NSLOADER_API_VERSION_MINOR 4
#define NSLOADER_API_VERSION ((NSLOADER_API_VERSION_MAJOR << 16) | NSLOADER_API_VERSION_MINOR)


#define NSVR_RETURN(ReturnType) NSLOADER_API ReturnType __stdcall



#ifdef __cplusplus
extern "C" {
#endif

	// System represents the NSVR plugin context. 
	typedef struct NSVR_System_ NSVR_System;

	// Events tell the hardware to do certain things, like play a haptic effect. 
	typedef struct NSVR_Event_ NSVR_Event;

	// Timelines are event containers, where each event has a specific time offset. 
	typedef struct NSVR_Timeline_ NSVR_Timeline;

	// A PlaybackHandle is used to start, stop, and reset timelines which have been transmitted to the
	// system. 
	typedef struct NSVR_PlaybackHandle_ NSVR_PlaybackHandle;


	typedef enum NSVR_Effect_ {
		NSVR_Effect_Bump = 1,
		NSVR_Effect_Buzz = 2,
		NSVR_Effect_Click = 3,
		NSVR_Effect_Fuzz = 5,
		NSVR_Effect_Hum = 6,
		NSVR_Effect_Pulse = 8,
		NSVR_Effect_Tick = 11,
		NSVR_Effect_Double_Click = 4,
		NSVR_Effect_Triple_Click = 16,
		NSVR_Effect_Max = 4294967295
	} NSVR_Effect;

	struct NSVR_Quaternion {
		float w;
		float x;
		float y;
		float z;
	};


	struct NSVR_TrackingUpdate {
		NSVR_Quaternion chest;
		NSVR_Quaternion left_upper_arm;
		NSVR_Quaternion left_forearm;
		NSVR_Quaternion right_upper_arm;
		NSVR_Quaternion right_forearm;
	};


	typedef enum NSVR_PlaybackCommand_
	{
		NSVR_PlaybackCommand_Play = 0,
		NSVR_PlaybackCommand_Pause,
		NSVR_PlaybackCommand_Reset
	} NSVR_PlaybackCommand;

	

	typedef enum NSVR_EventType_ {
		NSVR_EventType_BasicHapticEvent = 1,
		NSVR_EventType_Max = 65535
	} NSVR_EventType;


	typedef struct NSVR_DeviceInfo_ {
		char ProductName[128];
		short FirmwareMajor;
		short FirmwareMinor;
		//tracking capabilities?
	} NSVR_DeviceInfo;


	typedef struct NSVR_ServiceInfo_ {
		unsigned int ServiceMajor;
		unsigned int ServiceMinor;
	} NSVR_ServiceInfo;

	
	//Instantiates a new NSVR system context
	NSVR_RETURN(NSVR_Result) NSVR_System_Create(NSVR_System** systemPtr);

	//Returns true if this API supports the feature specified by a null-terminated string
	NSVR_RETURN(int) NSVR_Version_HasFeature(const char* feature);

	//Returns the version of this plugin, in the format ((Major << 16) | Minor)
	NSVR_RETURN(unsigned int) NSVR_Version_Get(void);
	
	//Returns true if the plugin is compatible with this header, false otherwise
	NSVR_RETURN(int) NSVR_Version_IsCompatibleDLL(void);

	
	NSVR_RETURN(void) NSVR_System_Release(NSVR_System** ptr);

	


	//Returns true if a suit is plugged in and the service is running, else false
	NSVR_RETURN(NSVR_Result) NSVR_System_GetServiceInfo(NSVR_System* systemPtr, NSVR_ServiceInfo* infoPtr);

	/* Haptics engine */ 
	NSVR_RETURN(NSVR_Result) NSVR_System_Haptics_Pause(NSVR_System* ptr);
	NSVR_RETURN(NSVR_Result) NSVR_System_Haptics_Resume(NSVR_System* ptr);
	NSVR_RETURN(NSVR_Result) NSVR_System_Haptics_Destroy(NSVR_System* ptr);


	/* Devices */
	NSVR_RETURN(NSVR_Result) NSVR_System_GetDeviceInfo(NSVR_System* systemPtr, NSVR_DeviceInfo* infoPtr);

	/* Tracking */
	NSVR_RETURN(NSVR_Result) NSVR_System_Tracking_Poll(NSVR_System* ptr, NSVR_TrackingUpdate* updatePtr);
	NSVR_RETURN(NSVR_Result) NSVR_System_Tracking_Enable(NSVR_System* ptr);
	NSVR_RETURN(NSVR_Result) NSVR_System_Tracking_Disable(NSVR_System* ptr);
	
	

	/* ---- Timeline API ---- */

	/* Events */
	NSVR_RETURN(NSVR_Result) NSVR_Event_Create(NSVR_Event** eventPtr, NSVR_EventType type);
	NSVR_RETURN(void)		 NSVR_Event_Release(NSVR_Event** event);
	NSVR_RETURN(NSVR_Result) NSVR_Event_SetFloat(NSVR_Event* event, const char* key, float value);
	NSVR_RETURN(NSVR_Result) NSVR_Event_SetInteger(NSVR_Event* event, const char* key, int value);


	/* Timelines */
	NSVR_RETURN(NSVR_Result) NSVR_Timeline_Create(NSVR_Timeline** eventListPtr, NSVR_System* systemPtr);
	NSVR_RETURN(void)		 NSVR_Timeline_Release(NSVR_Timeline** listPtr);
	NSVR_RETURN(NSVR_Result) NSVR_Timeline_AddEvent(NSVR_Timeline* list, NSVR_Event* event);
	NSVR_RETURN(NSVR_Result) NSVR_Timeline_Transmit(NSVR_Timeline* timeline, NSVR_PlaybackHandle* handlePr);

	/* Playback */
	NSVR_RETURN(NSVR_Result) NSVR_PlaybackHandle_Create(NSVR_PlaybackHandle** handlePtr);
	NSVR_RETURN(NSVR_Result) NSVR_PlaybackHandle_Command(NSVR_PlaybackHandle* handlePtr, NSVR_PlaybackCommand command);
	NSVR_RETURN(void)		 NSVR_PlaybackHandle_Release(NSVR_PlaybackHandle** handlePtr);




#ifdef __cplusplus
}
#endif

