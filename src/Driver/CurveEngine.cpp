#include "stdafx.h"
#include "CurveEngine.h"
#include "HardwareInterface.h"

void CurveEngine::GenerateCurve(uint64_t id, const NullSpaceIPC::CurveHaptic & haptic)
{
}

void CurveEngine::Update(float dt)
{
}

CurveEngine::CurveEngine(PluginManager & manager, HardwareInterface & hardware): m_dispatcher(manager)
{
	hardware.InstallFilter(
		[](const NullSpaceIPC::HighLevelEvent& ev) {
			return ev.events_case() == NullSpaceIPC::HighLevelEvent::kCurveHaptic;
		},
		[&](const NullSpaceIPC::HighLevelEvent& ev) {
			changePlaybackState(ev.parent_id(), ev.playback_event().command());
		}
	);


}

void CurveEngine::changePlaybackState(uint64_t id, NullSpaceIPC::PlaybackEvent_Command)
{
	
}
