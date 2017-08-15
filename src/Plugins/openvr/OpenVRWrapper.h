#pragma once

#include "PluginAPI.h"
#include <iostream>
#include "openvr.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
class OpenVRWrapper {
public:
	OpenVRWrapper();
	~OpenVRWrapper();
	void Configure(nsvr_core* core);
	void update();
	void triggerHapticPulse(float strength);
	void bufferedHaptics(double* samples, uint32_t count);
	void enumerateDevices(nsvr_device_ids* ids);
	void getDeviceInfo(uint64_t id, nsvr_device_basic_info* info);
private:
	std::atomic<bool> shouldShutDown;
	vr::IVRSystem* system;
	std::thread eventLoop;
	std::thread deleteMeTemporary;
	nsvr_core* core;
	void process(const vr::VREvent_t& event);
	void feedBufferedHaptics();
	std::queue<double> samples;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastSampleSent;
	std::mutex sampleLock;

	

};