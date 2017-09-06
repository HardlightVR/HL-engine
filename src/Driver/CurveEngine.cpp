#include "stdafx.h"
#include "CurveEngine.h"
#include "EventDispatcher.h"

void CurveEngine::GenerateCurve(uint64_t id, const NullSpaceIPC::CurveHaptic & haptic)
{

}

void CurveEngine::Update(float dt)
{
}

CurveEngine::CurveEngine(SubmissionCallback cb)
	: m_callback(cb)
{
}

