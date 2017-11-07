#pragma once


#include "PluginApis.h"
class HardwareAnalogAudioInterface {
public:
	HardwareAnalogAudioInterface(analogaudio_api* api);
	void open(nsvr_node_id node);
	void close(nsvr_node_id node);
private:
	analogaudio_api* m_api;
};