#include "stdafx.h"
#include "MotorDiagnostic.h"

MotorDiagnostic::MotorDiagnostic(boost::asio::io_service& io, std::vector<uint8_t> listOfMotors)
	: m_io(io)
	, m_motors(std::move(listOfMotors))
{
}

void MotorDiagnostic::run(IncomingData * in, OutgoingData * out)
{
	in->AddConsumer(inst::Id::GET_MOTOR_STATUS, [this](Packet packet) {
		uint8_t motorId = packet[4];
		m_statusBits[motorId] = static_cast<HL_Unit::_enumerated>(packet[3]);

		auto it = m_motorSenders.find(motorId);
		if (it != m_motorSenders.end()) {
			it->second->end();
		}
	});

	m_motorSenders.clear();

	for (uint8_t motorId : m_motors) {
		auto packet = inst::Build(inst::get_motor_status(inst::motor(motorId)));
		m_motorSenders.insert(std::make_pair(motorId, std::make_unique<RetrySender>(m_io, packet, out)));
		m_motorSenders.at(motorId)->begin();
	}

}


HardwareFailures MotorDiagnostic::get_results() const
{
	bool all_left_good = std::all_of(m_statusBits.begin(), m_statusBits.end(), [](const auto& kvp) {
		if (kvp.first >= 0x10 || kvp.first <= 0x17) {
			return (bool)(kvp.second & HL_Unit::Ready);
		}

		return true;
	});

	bool all_right_good = std::all_of(m_statusBits.begin(), m_statusBits.end(), [](const auto& kvp) {
		if (kvp.first >= 0x18 || kvp.first <= 0x1f) {
			return (bool)(kvp.second & HL_Unit::Ready);
		}

		return true;
	});


	HardwareFailures failures = HardwareFailures::None;
	if (!all_left_good) {
		failures = failures | HardwareFailures::LeftDriverBoardConnection;
	}

	if (!all_right_good) {
		failures = failures | HardwareFailures::RightDriverBoardConnection;
	}

	return failures;
}

TestProgress MotorDiagnostic::get_progress() const
{
	bool anyFailures = std::any_of(m_motorSenders.begin(), m_motorSenders.end(), [](const auto& kvp) {
		return kvp.second->get_status() == RetryStatus::ExceededMaxTries;
	});

	bool totalSuccesss = std::all_of(m_motorSenders.begin(), m_motorSenders.end(), [](const auto& kvp) {
		return kvp.second->get_status() == RetryStatus::UserCanceled;
	});

	if (totalSuccesss) {
		return TestProgress::Finished;
	} else if (anyFailures) {
		return TestProgress::FinishedPartialResult;
	}
	else {
		return TestProgress::InProgress;
	}
	
}

void MotorDiagnostic::cancel()
{
	for (auto& motorSender : m_motorSenders) {
		motorSender.second->end();
	}
}
