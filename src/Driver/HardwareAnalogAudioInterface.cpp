#include "stdafx.h"
#include "HardwareAnalogAudioInterface.h"

HardwareAnalogAudioInterface::HardwareAnalogAudioInterface(analogaudio_api * api) : m_api(api)
{
}

void HardwareAnalogAudioInterface::open(nsvr_node_id node)
{
	m_api->submit_open(node);
}

void HardwareAnalogAudioInterface::close(nsvr_node_id node)
{
	m_api->submit_close(node);
}
