#include "stdafx.h"
#include "Doctor.h"
#include <numeric>
#include "PacketDispatcher.h"
#include "suit_packet.h"
#include "MotorDiagnostic.h"
#include "ImuDiagnostic.h"

Doctor::Report Doctor::query_patient() const
{
	if (!m_patient) {
		return Status::Unplugged;
	}

	if (!all_finished()) {
		return Status::InProgress;
	}

	if (all_succeeded()) {
		return Status::NoErrors;
	}
	else {
		return aggregate_failures();
	}
	
}

const std::vector<uint8_t> allMotors = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
const std::vector<uint8_t> allSensors = {0x3a, 0x3c, 0x39};

Doctor::Doctor(boost::asio::io_service & io) 
	: m_io(io)
	, m_patient{boost::none}
{

	m_tests.push_back(std::make_unique<MotorDiagnostic>(m_io, allMotors));
	m_tests.push_back(std::make_unique<ImuDiagnostic>(m_io, allSensors));

}



void Doctor::release_patient()
{
	m_patient = boost::none;
	cancel_all_tests();

}

void Doctor::accept_patient(nsvr_device_id id, PacketDispatcher* dispatcher, boost::lockfree::spsc_queue<uint8_t>* device_outgoing_data)
{
	release_patient();

	m_patient = id;

	restart_tests(dispatcher, device_outgoing_data);

}


void Doctor::cancel_all_tests() {
	for (auto& test : m_tests) {
		test->cancel();
	}
}

void Doctor::restart_tests(PacketDispatcher* dispatcher, boost::lockfree::spsc_queue<uint8_t>* device_outgoing_data) {

	for (auto& diag : m_tests) {
		diag->run(dispatcher, device_outgoing_data);
	}
}

bool Doctor::all_finished() const
{
	return std::all_of(m_tests.begin(), m_tests.end(), [](const auto& diag) {
		TestProgress progress = diag->get_progress();
		return progress == TestProgress::Finished || progress == TestProgress::FinishedPartialResult;
	});
	
}

bool Doctor::all_succeeded() const {
	return std::all_of(m_tests.begin(), m_tests.end(), [](const auto& diag) {
		return diag->get_results() == HardwareFailures::None;
	});
}

HardwareFailures Doctor::aggregate_failures() const
{
	return std::accumulate(m_tests.begin(), m_tests.end(), HardwareFailures::None, [](HardwareFailures failures, const auto& test) {
		return failures | test->get_results();
	});
}
