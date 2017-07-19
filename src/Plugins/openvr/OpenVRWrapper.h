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
	void Configure(nsvr_core_ctx* core);
	void update();
	void triggerHapticPulse(float strength);

private:
	std::atomic<bool> shouldShutDown;
	vr::IVRSystem* system;
	std::thread eventLoop;
	nsvr_core_ctx* core;
	void process(const vr::VREvent_t& event);
};