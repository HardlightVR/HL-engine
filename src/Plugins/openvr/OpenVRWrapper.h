#pragma once

#include "PluginAPI.h"
#include <iostream>
#include "openvr.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include <unordered_map>
class OpenVRWrapper {
public:
	OpenVRWrapper();
	~OpenVRWrapper();
	void Configure(nsvr_core* core);
	void configureBodyGraph(nsvr_bodygraph* graph);
	void update();
	void triggerHapticPulse(vr::TrackedDeviceIndex_t device, float strength);
	void bufferedHaptics(uint64_t device_id, double* samples, uint32_t count);
	void enumerateNodesForDevice(uint32_t device_id, nsvr_node_ids* ids);
	void getDeviceInfo(uint32_t id, nsvr_device_info* info);
	void getNodeInfo(uint64_t id, nsvr_node_info* info);

	void triggerPreset(uint64_t device,nsvr_waveform* req);
	void enumerateDevices(nsvr_device_ids* ids);
private:
	vr::IVRSystem* system;
	nsvr_core* core;
	nsvr_bodygraph* graph;
	void process(const vr::VREvent_t& event);
	void feedBufferedHaptics();
	std::unordered_map<uint64_t, std::queue<double>> samples;
	using LastSampleTimeSent = std::chrono::time_point<std::chrono::high_resolution_clock>;

	std::unordered_map<uint64_t, LastSampleTimeSent> sampleTimestamps;
	std::mutex sampleLock;

	

	void handleDeviceActivated(const vr::VREvent_t& event);
	void handleDeviceDeactivated(const vr::VREvent_t& event);
};