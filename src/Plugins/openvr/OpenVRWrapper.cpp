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

	nsvr_plugin_buffer_api buffered_api;
	buffered_api.client_data = this;

	buffered_api.getmaxsamples_handler = [](uint32_t* outMaxSamples, void* ud) {
		*outMaxSamples = 200;
	};
	buffered_api.getsampleduration_handler = [](double* outDuration, void* ud) {
		*outDuration = 5;
	};
	buffered_api.submit_handler = [](double* samples, uint32_t count, void* ud) {
		AS_TYPE(OpenVRWrapper, ud)->bufferedHaptics(samples, count);
	};

	nsvr_register_buffer_api(core, &buffered_api);

	this->core = core;
}

void OpenVRWrapper::update()
{
	while (!shouldShutDown.load()) {
		if (system) {
			vr::VREvent_t event;
			while (system->PollNextEvent(&event, sizeof(event))) {
				process(event);
			}

		}
		feedBufferedHaptics();

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

void OpenVRWrapper::bufferedHaptics(double * amps, uint32_t count)
{
	std::lock_guard<std::mutex> guard(sampleLock);
	for (uint32_t i = 0; i < count; i++) {
		this->samples.push(amps[i] );
	}
}


void OpenVRWrapper::process(const vr::VREvent_t&  event)
{
	switch (event.eventType) {
	case vr::VREvent_TrackedDeviceActivated:
		nsvr_device_event_raise(core, nsvr_device_event_device_connected, event.trackedDeviceIndex);
		break;
	case vr::VREvent_TrackedDeviceDeactivated:
		nsvr_device_event_raise(core, nsvr_device_event_device_disconnected, event.trackedDeviceIndex);
		break;
	default:
		break;
	}
}

void OpenVRWrapper::feedBufferedHaptics()
{
	std::lock_guard<std::mutex> guard(sampleLock);

	if (samples.empty()) {
		return;
	}

	if ((std::chrono::high_resolution_clock::now() - lastSampleSent) > std::chrono::milliseconds(5)) {
		std::cout << "Triggering a haptic pulse with strength " << samples.front()  << '\n';
		triggerHapticPulse(samples.front());
		samples.pop();

		lastSampleSent = std::chrono::high_resolution_clock::now();
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
			info->region = nsvr_region_hand_left;
			info->type = nsvr_device_type_haptic;
		}

		else if (id == 2) {
			std::string name("Vive Controller Right Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			info->region = nsvr_region_hand_right;
			info->type = nsvr_device_type_haptic;
		}
		else if (id == 5) {
			std::string name("Vive Controller Awesome Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			info->region = nsvr_region_hand_left;
			info->type = nsvr_device_type_haptic;
		}
	}
	else {

		if (id == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand)) {
			std::string name("Vive Controller Left Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			info->region = nsvr_region_hand_left;
			info->type = nsvr_device_type_haptic;
		}

		else if (id == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand)) {
			std::string name("Vive Controller Right Hand");
			strcpy_s(info->name, 128, name.c_str());
			info->capabilities = nsvr_device_capability_preset;
			info->region = nsvr_region_hand_right;
			info->type = nsvr_device_type_haptic;
		}
	}
}
