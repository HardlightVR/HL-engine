#include "stdafx.h"
#include "HardwareCommandVisitor.h"
#include "FirmwareInterface.h"
HardwareCommandVisitor::HardwareCommandVisitor(FirmwareInterface& f) : firmware(f)
{

}

void HardwareCommandVisitor::operator()(const PlaySingle & single) 
{
	firmware.PlayEffect(single.Area, single.Effect, single.Strength);
}

void HardwareCommandVisitor::operator()(const PlayCont & cont)
{
	firmware.PlayEffectContinuous(cont.Area, cont.Effect, cont.Strength);
}

void HardwareCommandVisitor::operator()(const PlayVol& vol)
{
	firmware.PlayRtp(vol.Area, vol.Volume);
}

void HardwareCommandVisitor::operator()(const EnableIntrig & intrig)
{
	firmware.EnableIntrigMode(intrig.Area);
}

void HardwareCommandVisitor::operator()(const EnableRtp & rtp)
{
	firmware.EnableRtpMode(rtp.Area);
}

void HardwareCommandVisitor::operator()(const Halt& halt)
{
	firmware.HaltEffect(halt.Area);
}
