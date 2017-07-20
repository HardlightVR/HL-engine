#include "stdafx.h"
#include "CurveEngine.h"
#include "EventDispatcher.h"

void CurveEngine::GenerateCurve(uint64_t id, const NullSpaceIPC::CurveHaptic & haptic)
{
}

void CurveEngine::Update(float dt)
{
}

CurveEngine::CurveEngine(PluginManager & manager, EventDispatcher & hardware): m_dispatcher(manager)
{
	
	hardware.Subscribe(NullSpaceIPC::HighLevelEvent::kCurveHaptic,
		[&](const NullSpaceIPC::HighLevelEvent& ev) {
			changePlaybackState(ev.parent_id(), ev.playback_event().command());
		}
	);


}

void CurveEngine::changePlaybackState(uint64_t id, NullSpaceIPC::PlaybackEvent_Command)
{
	
}
