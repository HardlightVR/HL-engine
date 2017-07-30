#pragma once

#include "PluginAPI.h"
#include <iostream>
#include "openvr.h"
#include <thread>
#include <atomic>
class OpenVRWrapper {
public:
	OpenVRWrapper();
	~OpenVRWrapper();
	void Configure(nsvr_core* core);
	void update();
	void triggerHapticPulse(float strength);

private:
	std::atomic<bool> shouldShutDown;
	vr::IVRSystem* system;
	std::thread eventLoop;
	std::thread deleteMeTemporary;
	nsvr_core* core;
	void process(const vr::VREvent_t& event);
public:
	void enumerateDevices(nsvr_device_ids* ids);
	void getDeviceInfo(uint64_t id, nsvr_device_basic_info* info);
};