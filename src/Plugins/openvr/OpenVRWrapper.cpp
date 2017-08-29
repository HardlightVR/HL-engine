#include "stdafx.h"
#include "OpenVRWrapper.h"

#include <cassert>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>


OpenVRWrapper::OpenVRWrapper() 
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


	
}

OpenVRWrapper::~OpenVRWrapper()
{
	
	if (system)
	{
		vr::VR_Shutdown();
		system = NULL;
	}

}

void OpenVRWrapper::Configure(nsvr_core* core)
{


	
	nsvr_plugin_waveform_api waves;
	waves.client_data = this;
	waves.activate_handler = [](uint64_t request_id, uint64_t device_id, nsvr_waveform* wave, void* ud) {
		static_cast<OpenVRWrapper*>(ud)->triggerPreset(device_id, wave);
	};
	nsvr_register_waveform_api(core, &waves);

	nsvr_plugin_device_api devices;
	devices.client_data = this;
	devices.enumeratedevices_handler = [](nsvr_device_ids* ids, void* ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		wrapper->enumerateDevices(ids);
	};
	devices.enumeratenodes_handler = [](uint32_t device_id, nsvr_node_ids* ids, void* ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		wrapper->enumerateNodesForDevice(device_id, ids);
	};
	devices.getdeviceinfo_handler = [](uint32_t id, nsvr_device_info* info, void *ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		return wrapper->getDeviceInfo(id, info);
	};

	devices.getnodeinfo_handler = [](uint64_t node_id, nsvr_node_info* info, void *ud) {
		OpenVRWrapper* wrapper = static_cast<OpenVRWrapper*>(ud);
		return wrapper->getNodeInfo(node_id, info);
	};

	nsvr_register_device_api(core, &devices);

	nsvr_plugin_buffered_api buffered_api;
	buffered_api.client_data = this;

	buffered_api.getmaxsamples_handler = [](uint32_t* outMaxSamples, void* ud) {
		*outMaxSamples = 200;
	};
	buffered_api.getsampleduration_handler = [](double* outDuration, void* ud) {
		*outDuration = 5;
	};
	buffered_api.submit_handler = [](uint64_t device_id, double* samples, uint32_t count, void* ud) {
		AS_TYPE(OpenVRWrapper, ud)->bufferedHaptics(device_id, samples, count);
	};

	nsvr_register_buffered_api(core, &buffered_api);

	this->core = core;


	nsvr_plugin_bodygraph_api bodygraph;
	bodygraph.client_data = this;
	bodygraph.setup_handler = [](nsvr_bodygraph* graph, void * cd) {
		AS_TYPE(OpenVRWrapper, cd)->configureBodyGraph(graph);
	};

	nsvr_register_bodygraph_api(core, &bodygraph);



	nsvr_plugin_updateloop_api update;
	update.client_data = this;
	update.update_handler = [](uint64_t dt, void* cd) {
		AS_TYPE(OpenVRWrapper, cd)->update();
	};
	nsvr_register_updateloop_api(core, &update);
}

void OpenVRWrapper::configureBodyGraph(nsvr_bodygraph * graph)
{
	nsvr_bodygraph_region* controllerPalmRegion;
	nsvr_bodygraph_region_create(&controllerPalmRegion);
	nsvr_bodygraph_region_setboundingboxdimensions(controllerPalmRegion, 4, 4);

	nsvr_bodygraph_region_setlocation(controllerPalmRegion, nsvr_bodypart_palm_left, 0.5, 0);
	nsvr_bodygraph_createnode(graph, "Palm Left Zone", controllerPalmRegion);
	
	nsvr_bodygraph_region_setlocation(controllerPalmRegion, nsvr_bodypart_palm_right, 0.5, 0);
	nsvr_bodygraph_createnode(graph, "Palm Right Zone", controllerPalmRegion);


	nsvr_bodygraph_region_destroy(&controllerPalmRegion);

	this->graph = graph;
}

void OpenVRWrapper::update()
{
	
	if (system) {
		vr::VREvent_t event;
			
		while (system->PollNextEvent(&event, sizeof(event))) {
			process(event);
		}

	}
	feedBufferedHaptics();

	
}

void OpenVRWrapper::triggerHapticPulse(vr::TrackedDeviceIndex_t device, float strength)
{
	assert(strength >= 0 && strength <= 1.0);
	//4000microseconds = strongest..
	short durationMicroSec = short(3999 * strength);
	vr::EVRButtonId buttonId = vr::EVRButtonId::k_EButton_SteamVR_Touchpad;
	auto axisId = (uint32_t)buttonId - (uint32_t)vr::EVRButtonId::k_EButton_Axis0;
	if (system) {
		system->TriggerHapticPulse(device, axisId, durationMicroSec);
	}
}

void OpenVRWrapper::bufferedHaptics(uint64_t device_id, double * amps, uint32_t count)
{
	std::lock_guard<std::mutex> guard(sampleLock);
	for (uint32_t i = 0; i < count; i++) {
		this->samples[device_id].push(amps[i] );
	}
}


void OpenVRWrapper::process(const vr::VREvent_t&  event)
{
	switch (event.eventType) {
	case vr::VREvent_TrackedDeviceActivated:
		handleDeviceActivated(event);
		break;
	case vr::VREvent_TrackedDeviceDeactivated:
		handleDeviceDeactivated(event);
		break;
	default:
		break;
	}
}

void OpenVRWrapper::feedBufferedHaptics()
{
	std::lock_guard<std::mutex> guard(sampleLock);

	for (auto& sampleQueue : samples) {
		
		if (sampleQueue.second.empty()) {
			return;
		}

		auto lastSampleSent = sampleTimestamps[sampleQueue.first];
		
		if ((std::chrono::high_resolution_clock::now() - lastSampleSent) > std::chrono::milliseconds(7)) {
			//std::cout << "Triggering on " << sampleQueue.first << " str: " << sampleQueue.second.front() << '\n';
			triggerHapticPulse(sampleQueue.first, sampleQueue.second.front());
			sampleQueue.second.pop();
			sampleTimestamps[sampleQueue.first] = std::chrono::high_resolution_clock::now();
		}
	}
}

void OpenVRWrapper::handleDeviceActivated(const vr::VREvent_t& event)
{
	nsvr_device_event_raise(core, nsvr_device_event_device_connected, event.trackedDeviceIndex);

	if (event.trackedDeviceIndex == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand)) {
		nsvr_bodygraph_clearassociations(graph, event.trackedDeviceIndex);
		nsvr_bodygraph_associate(graph, "Palm Left Zone", event.trackedDeviceIndex);
	}
	else if (event.trackedDeviceIndex == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand)) {
		nsvr_bodygraph_clearassociations(graph, event.trackedDeviceIndex);
		nsvr_bodygraph_associate(graph, "Palm Right Zone", event.trackedDeviceIndex);
	}
}

void OpenVRWrapper::handleDeviceDeactivated(const vr::VREvent_t& event)
{
	nsvr_device_event_raise(core, nsvr_device_event_device_disconnected, event.trackedDeviceIndex);

}

void OpenVRWrapper::enumerateNodesForDevice(uint32_t device_id, nsvr_node_ids* ids)
{
	if (system) {
		
		int currentId = 0;
		auto deviceClass = system->GetTrackedDeviceClass(device_id);
		if (deviceClass == vr::TrackedDeviceClass_Controller) {
			ids->ids[0] = 1;
			ids->node_count = 1;

		}
		
	}
	
}
void OpenVRWrapper::getNodeInfo(uint64_t id, nsvr_node_info* info) {
	if (id == 0) {
		info->capabilities = nsvr_device_capability_preset | nsvr_device_capability_buffered;
		info->type = nsvr_device_type_haptic;
		std::string name("Haptic Thumbpad");
		std::copy(name.begin(), name.end(), info->name);

	} 
}
void OpenVRWrapper::getDeviceInfo(uint32_t id, nsvr_device_info* info)
{
	if (system) {
		auto deviceClass = system->GetTrackedDeviceClass(id);
		if (deviceClass == vr::TrackedDeviceClass_Controller) {
			if (id == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand)) {
				std::string name("Controller (Left Hand)");
				std::copy(name.begin(), name.end(), info->name);
				
				
			}
			else if (id == system->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand)) {
				std::string name("Controller (Right Hand)");
				std::copy(name.begin(), name.end(), info->name);
			
			}
			else {
				char manufacturerName[64];
				system->GetStringTrackedDeviceProperty(id, vr::ETrackedDeviceProperty::Prop_ManufacturerName_String, manufacturerName, 64);
				char model[64];
				system->GetStringTrackedDeviceProperty(id, vr::ETrackedDeviceProperty::Prop_ModelNumber_String, model, 64);

				
				std::string name = std::string(manufacturerName) + " " + std::string(model);
				std::copy(name.begin(), name.end(), info->name);
				
			}
		}
	}
}

void sinsample(std::vector<double>& samples, float maxStrength, std::size_t numSamples) {
	float clampedStrength = std::max(0.0f, std::min(1.0f, maxStrength));

	float total = (M_PI/numSamples);
	for (std::size_t i = 0; i < numSamples; i++) {
		
		float s = std::sin((float)i*total)*clampedStrength;
		if (s > 0) {
			samples.push_back(s);
		}
		else {
			samples.push_back(0);
		}
	}

}

void constant(std::vector<double>& samples, float strength, std::size_t numsamples) {
	float clampedStrength = std::max(0.0f, std::min(1.0f, strength));
	for (std::size_t i = 0; i < numsamples; i++) {
		samples.push_back(clampedStrength);
	}
}
std::vector<double> generateWaveform(float strength, nsvr_default_waveform family) {
	if (nsvr_preset_family_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 10);
		return samples;
	}else
	if (nsvr_preset_family_pulse == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 50);
		constant(samples, 0, 5);

		return samples;
	}
	else
	if (nsvr_preset_family_tick == family) {
		std::vector<double> samples;
		constant(samples, strength, 5);
		return samples;
	}
	else
	if (nsvr_preset_family_buzz == family) {
		std::vector<double> samples;
		constant(samples, strength, 5);
		constant(samples,0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		constant(samples, strength, 5);
		constant(samples, 0, 3);
		return samples;
	}
	else
	if (nsvr_preset_family_double_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 26);
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 20);

		return samples;

	}
	else
	if (nsvr_preset_family_triple_click == family) {
		std::vector<double> samples;
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 26);
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 20);
		sinsample(samples, strength, 8);
		constant(samples, 0.0f, 20);
		return samples;

	}
	else
	 if (nsvr_preset_family_hum == family) {
		std::vector<double> samples;
		constant(samples, strength, 50);
		return samples;
	}

	 else {
		 std::vector<double> samples;
		 constant(samples, strength, 8);
		 return samples;
	 }
	
}
void OpenVRWrapper::triggerPreset(uint64_t device, nsvr_waveform* req)
{
	nsvr_default_waveform waveform;
	nsvr_waveform_getname(req, &waveform);

	float strength;
	nsvr_waveform_getstrength(req, &strength);

	std::size_t repetitions;
	nsvr_waveform_getrepetitions(req, &repetitions);


	auto wave = generateWaveform(strength, waveform);
	auto copy = wave;
	if (repetitions > 0) {
		for (auto i = 0; i < repetitions; i++) {
			wave.insert(wave.end(), copy.begin(), copy.end());
		}
	}
	bufferedHaptics(device, wave.data(), wave.size());
}

void OpenVRWrapper::enumerateDevices(nsvr_device_ids* ids)
{
	if (system) {
		int index = 0;
		for (auto i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			auto deviceClass = system->GetTrackedDeviceClass(i);
			if (deviceClass == vr::TrackedDeviceClass_Controller) {
				ids->device_count++;
				ids->ids[index] = i;
				index++;
			}
		}
	}
}
