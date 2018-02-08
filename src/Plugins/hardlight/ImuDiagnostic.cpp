#include "stdafx.h"
#include "ImuDiagnostic.h"

ImuDiagnostic::ImuDiagnostic(boost::asio::io_service & io, std::vector<uint8_t> imus)
	: m_io(io)
	, m_imus(std::move(imus))
{
}

void ImuDiagnostic::run(IncomingData * in, OutgoingData * out)
{
	in->AddConsumer(inst::Id::GET_TRACK_STATUS, [this](Packet packet) {
		uint8_t sensor = packet[4];
		m_statusBits[sensor] = static_cast<HL_Unit::_enumerated>(packet[3]);

		auto it = m_senders.find(sensor);
		if (it != m_senders.end()) {
			it->second->end();
		}
	});

	m_senders.clear();

	for (uint8_t sensor : m_imus) {
		auto packet = inst::Build(inst::get_track_status(inst::sensor(sensor)));
		m_senders.insert(std::make_pair(sensor, std::make_unique<RetrySender>(m_io, packet, out)));
		m_senders.at(sensor)->begin();
	}
}

void ImuDiagnostic::cancel()
{
	for (auto& kvp : m_senders) {
		kvp.second->end();
	}
}

TestProgress ImuDiagnostic::get_progress() const
{
	bool anyFailures = std::any_of(m_senders.begin(), m_senders.end(), [](const auto& kvp) {
		return kvp.second->get_status() == RetryStatus::ExceededMaxTries;
	});

	bool totalSuccesss = std::all_of(m_senders.begin(), m_senders.end(), [](const auto& kvp) {
		return kvp.second->get_status() == RetryStatus::UserCanceled;
	});

	if (totalSuccesss) {
		return TestProgress::Finished;
	}
	else if (anyFailures) {
		return TestProgress::FinishedPartialResult;
	}
	else {
		return TestProgress::InProgress;
	}

}

const uint8_t IMU_CHEST = 0x3c;
const uint8_t IMU_RUARM = 0x39;
const uint8_t IMU_LUARM = 0x3a;

HardwareFailures ImuDiagnostic::get_results() const
{
	HardwareFailures failures = HardwareFailures::None;
	if (!(m_statusBits.at(IMU_CHEST) & HL_Unit::Ready)) {
		failures = failures | HardwareFailures::ImuChestConnection;
	}
	if (!(m_statusBits.at(IMU_RUARM) & HL_Unit::Ready)) {
		failures = failures | HardwareFailures::ImuRightConnection;
	}
	if (!(m_statusBits.at(IMU_LUARM) & HL_Unit::Ready)) {
		failures = failures | HardwareFailures::ImuLeftConnection;
	}
	return failures;
}
