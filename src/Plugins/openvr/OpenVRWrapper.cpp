#include "stdafx.h"
#include "OpenVRWrapper.h"

#include <cassert>
#include <chrono>

OpenVRWrapper::OpenVRWrapper() : shouldShutDown{false}
{
	vr::EVRInitError eError = vr::VRInitError_None;
	system = vr::VR_Init(&eError, vr::VRApplication_Background);
	if (eError != vr::VRInitError_None)
	{
		system = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		std::cout << buf << '\n';

	}

	eventLoop = std::thread([this]() { update(); });

	
}

OpenVRWrapper::~OpenVRWrapper()
{
	shouldShutDown.store(true);
	if (eventLoop.joinable()) {
		eventLoop.join();
	}
	if (system)
	{
		vr::VR_Shutdown();
		system = NULL;
	}

}

void OpenVRWrapper::Configure(nsvr_core* core)
{
	nsvr_plugin_preset_api preset;
	preset.client_data = this;
	preset.preset_handler = [](nsvr_preset_request* req, void* ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		float strength;
		nsvr_preset_request_getstrength(req, &strength);
		wrapper->triggerHapticPulse(strength);
	};

	nsvr_register_preset_api(core, &preset);
	

	nsvr_plugin_device_api devices;
	devices.client_data = this;
	devices.enumerateids_handler = [](nsvr_device_ids* ids, void* ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		wrapper->enumerateDevices(ids);
	};

	devices.getinfo_handler = [](uint64_t id, nsvr_device_basic_info* info, void *ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		wrapper->getDeviceInfo(id, info);
	};

	nsvr_register_device_api(core, &devices);

	this->core = core;
}

void OpenVRWrapper::update()
{
	if (!shouldShutDown.load()) {
		if (system) {
			vr::VREvent_t event;
			while (system->PollNextEvent(&event, sizeof(event))) {
				process(event);
			}
		}
	}
}

void OpenVRWrapper::triggerHapticPulse(float strength)
{
	assert(strength >= 0 && strength <= 1.0);
	//4000microseconds = strongest..
	short durationMicroSec = short(4000 * strength);
	vr::EVRButtonId buttonId = vr::EVRButtonId::k_EButton_SteamVR_Touchpad;
	auto axisId = (uint32_t)buttonId - (uint32_t)vr::EVRButtonId::k_EButton_Axis0;
	if (system) {
		system->TriggerHapticPulse(0, axisId, durationMicroSec);
	}
}


void OpenVRWrapper::process(const vr::VREvent_t&  event)
{
	switch (event.eventType) {
	case vr::VREvent_TrackedDeviceActivated:
	{
		nsvr_device_event* devent;
		nsvr_device_event_create(&devent, nsvr_device_event_device_connected);
		nsvr_device_event_setid(devent, event.trackedDeviceIndex);
		nsvr_device_event_raise(core, devent);
		nsvr_device_event_destroy(&devent);
		break;
	}
	case vr::VREvent_TrackedDeviceDeactivated:
	{
	
		nsvr_device_event* devent;
		nsvr_device_event_create(&devent, nsvr_device_event_device_disconnected);
		nsvr_device_event_setid(devent, event.trackedDeviceIndex);
		nsvr_device_event_raise(core, devent);
		nsvr_device_event_destroy(&devent);
		break;
	}

	default:
		break;
	}
}

void OpenVRWrapper::enumerateDevices(nsvr_device_ids* ids)
{
	if (system) {
		auto lhIndex = system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		auto rhIndex = system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		ids->device_count = 2;

		ids->ids[0] = lhIndex;
		ids->ids[1] = rhIndex;
	}
	else {
		ids->device_count = 2;
		ids->ids[0] = 1;
		ids->ids[1] = 2;
	}
}

void OpenVRWrapper::getDeviceInfo(uint64_t id, nsvr_device_basic_info* info)
{
	if (!system) {

		if (id ==1) {
			std::string name("Vive Controller Left Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			std::string region("left_upper_chest");
			strcpy_s(info->region, 128, region.c_str());
			info->type = nsvr_device_type_haptic;
		}

		else if (id == 2) {
			std::string name("Vive Controller Right Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			std::string region("right_upper_chest");
			strcpy_s(info->region, 128, region.c_str());
			info->type = nsvr_device_type_haptic;
		}
		else if (id == 5) {
			std::string name("Vive Controller Awesome Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			std::string region("right_upper_yolo");
			strcpy_s(info->region, 128, region.c_str());
			info->type = nsvr_device_type_haptic;
		}
	}
	else {

		if (id == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand)) {
			std::string name("Vive Controller Left Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			std::string region("left_upper_chest");
			strcpy_s(info->region, 128, region.c_str());
			info->type = nsvr_device_type_haptic;
		}

		else if (id == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand)) {
			std::string name("Vive Controller Right Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			std::string region("right_upper_chest");
			strcpy_s(info->region, 128, region.c_str());
			info->type = nsvr_device_type_haptic;
		}
	}
}
