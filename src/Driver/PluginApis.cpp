#include "stdafx.h"
#include "PluginApis.h"


static const std::unordered_map<Apis, const char*> PrintableApiNames = {
	{Apis::Buffered, "Buffered"},
	{Apis::Device, "Device"},
	{Apis::Playback, "Playback"},
	{Apis::Preset, "Preset"},
	{Apis::RawCommand, "RawCommand"},
	{Apis::Request, "Request"},
	{Apis::Sampling, "Sampling"},
	{Apis::Tracking, "Tracking"}
};

bool PluginApis::Supports(Apis api) const
{
	return m_apis.find(api) != m_apis.end();
}
