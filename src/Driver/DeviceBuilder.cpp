#include "stdafx.h"
#include "DeviceBuilder.h"
#include "FakeBodygraph.h"
#include "FakeDiscoverer.h"
#include "FakeTrackingProvider.h"
#include "SharedTypes.h"


//Temporarily sticking this in here until I know how the fake will work
class FakePlayback : public PlaybackController {
public:
	void CreateEventRecord(uint64_t request_id, std::vector<NodeId<local>> nodes) override {}
	void Cancel(uint64_t request_id) override {}
	void Pause(uint64_t request_id) override {}
	void Resume(uint64_t request_id)override {}
};

class FakeHaptics : public HapticInterface {
public: 
	void SubmitSimpleHaptic(uint64_t request_id, nsvr_node_id id, SimpleHaptic data) override {}
};

DeviceBuilder::DeviceBuilder()
	: m_description()
{
}

DeviceBuilder & DeviceBuilder::WithDefaultBodygraph()
{
	m_bodygraph = std::shared_ptr<FakeBodygraph>(FakeBodygraphBuilder().Build());
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultNodeDiscovery()
{
	m_discoverer = FakeDiscovererBuilder().Build();
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultTracking()
{
	m_tracking = FakeTrackingBuilder().Build();
	return *this;
}

DeviceBuilder& DeviceBuilder::WithDefaultHaptics()
{
	m_haptics = std::make_unique<FakeHaptics>();
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultPlaybackControl()
{
	m_playback = std::make_unique<FakePlayback>();
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultDescription()
{
	DeviceDescriptor desc;
	desc.concept = nsvr_device_concept_controller;
	desc.displayName = "Generic Controller Device";
	desc.id = 0;
	m_description = desc;
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDefaultOriginatingPlugin()
{
	m_originatingPlugin = "Fake Plugin";
	return *this;
}

DeviceBuilder & DeviceBuilder::WithBodygraph(std::unique_ptr<BodyGraphCreator> bodygraph)
{
	m_bodygraph = std::move(bodygraph);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithNodeDiscoverer(std::unique_ptr<NodeDiscoverer> discoverer)
{
	m_discoverer = std::move(discoverer);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithTracking(std::unique_ptr<TrackingProvider> tracking)
{
	m_tracking = std::move(tracking);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithPlayback(std::unique_ptr<PlaybackController> playback)
{
	m_playback = std::move(playback);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithHapticInterface(std::unique_ptr<HapticInterface> haptics)
{
	m_haptics = std::move(haptics);
	return *this;
}

DeviceBuilder & DeviceBuilder::WithDescriptor(DeviceDescriptor deviceDescription)
{
	m_description = deviceDescription;
	return *this;
}

DeviceBuilder & DeviceBuilder::WithOriginatingPlugin(std::string pluginName)
{
	m_originatingPlugin = pluginName;
	return *this;
}

std::unique_ptr<Device> DeviceBuilder::Build()
{
	if (!m_bodygraph) {
		WithDefaultBodygraph();
	}

	if (!m_discoverer) {
		WithDefaultNodeDiscovery();
	}

	if (!m_tracking) {
		WithDefaultTracking();
	}

	if (!m_playback) {
		WithDefaultPlaybackControl();
	}

	if (!m_description) {
		WithDefaultDescription();
	}

	if (!m_originatingPlugin) {
		WithDefaultOriginatingPlugin();
	}
	return std::make_unique<Device>(
		*m_originatingPlugin,
		*m_description,
		m_bodygraph,
		std::move(m_discoverer),
		std::move(m_playback),
		std::move(m_haptics),
		std::move(m_tracking)
	);
	
}
