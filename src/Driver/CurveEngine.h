#pragma once
#include "protobuff_defs/HighLevelEvent.pb.h"
#include <stdint.h>
#include <unordered_map>

#include "SimulatedHapticNode.h"
class PluginManager;
class EventDispatcher;



class PluginDispatcher;


class CurveEngine {
public:

	using SubmissionCallback = std::function<void(const Waveform&)>;
	void GenerateCurve(uint64_t id, const NullSpaceIPC::CurveHaptic& haptic);
	void Update(float dt);

	CurveEngine(SubmissionCallback cb);
private:
	SubmissionCallback m_callback;

};