#include "stdafx.h"
#include "DriverMessenger.h"

#include "Encoder.h"
DriverMessenger::DriverMessenger(boost::asio::io_service& io):
_running{true},
	_process([](void const*, std::size_t) {}),
	m_hapticsData("ns-haptics-data", 100, 256),
	m_trackingData("ns-tracking-data"),
	m_suitConnectionInfo("ns-suit-data"),
	m_loggingStream("ns-logging-data", 500, 512),
	m_sentinal("ns-sentinal"),

	m_sentinalTimer(io),
	m_sentinalInterval(1000)

{
	TrackingUpdate nullTracking = {};
	SuitsConnectionInfo nullSuits = {};
	m_trackingData.Write(nullTracking);
	m_suitConnectionInfo.Write(nullSuits);

	startSentinal();
}


void DriverMessenger::startSentinal() {
	m_sentinalTimer.expires_from_now(m_sentinalInterval);
	m_sentinalTimer.async_wait(boost::bind(&DriverMessenger::sentinalHandler, this, boost::asio::placeholders::error));
}
void DriverMessenger::sentinalHandler(const boost::system::error_code& ec) {
	if (!ec) {
		m_sentinal.Write(std::time(nullptr));
		startSentinal();
	}
}

DriverMessenger::~DriverMessenger()
{




}

void DriverMessenger::WriteTracking(TrackingUpdate t)
{
	m_trackingData.Write(t);
}

void DriverMessenger::WriteSuits(SuitsConnectionInfo s)
{
	m_suitConnectionInfo.Write(s);
}

boost::optional<ExecutionCommand> DriverMessenger::ReadHaptics()
{
	auto data = m_hapticsData.Pop();
	flatbuffers::Verifier verifier(reinterpret_cast<uint8_t const*>(data.data()), data.size());
	if (NullSpace::HapticFiles::VerifyExecutionCommandBuffer(verifier)) {
		auto packet = NullSpace::HapticFiles::GetExecutionCommand(data.data());
		return Encoder::Decode(packet);
	}

	return boost::optional<ExecutionCommand>();
}




void DriverMessenger::Disconnect()
{
	_running.store(false);
	m_sentinalTimer.cancel();
}


