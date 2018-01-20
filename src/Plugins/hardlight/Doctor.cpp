#include "stdafx.h"
#include "Doctor.h"
#include <numeric>
#include "PacketDispatcher.h"
#include "suit_packet.h"



Doctor::Status Doctor::query_patient() const
{
	if (!m_currentPatient) {
		return Status::Unplugged;
	}

	if (!all_finished()) {
		return Status::CheckingDiagnostics;
	}
	
	if (all_succeeded()) {
		if (m_motorDiagnosis.all_ok()) {
			return Status::OkDiagnostics;
		}
		else {
	
			return Status::BadMotors;
		}
	}
	else {
		return Status::SomeMotorsDidntRespond;
	}

	
}


Doctor::Doctor(boost::asio::io_service & io) 
	: m_io(io)
	, m_currentPatient{boost::none}
{
	

}



void Doctor::release_patient()
{
	m_currentPatient = boost::none;
	cancel_all_senders();

}

void Doctor::accept_patient(nsvr_device_id id, PacketDispatcher* dispatcher, boost::lockfree::spsc_queue<uint8_t>* device_outgoing_data)
{
	release_patient();

	m_currentPatient = id;

	reinitialize_senders(device_outgoing_data);


	dispatcher->AddConsumer(inst::Id::GET_MOTOR_STATUS, [this](Packet packet) {
		uint8_t motorId = packet[4];
		m_motorDiagnosis.setStatusBits(motorId, packet[3]);
		auto it = m_motorSenders.find(motorId);
		if (it != m_motorSenders.end()) {
			it->second->end();
		}
	});
}


void Doctor::cancel_all_senders() {
	for (auto& kvp : m_motorSenders) {
		kvp.second->end();
	}
}

void Doctor::reinitialize_senders(boost::lockfree::spsc_queue<uint8_t>* device_outgoing_data) {

	m_motorSenders.clear();

	for (uint8_t i = 0x10; i <= 0x1f; i++) {
		auto packet = inst::Build(inst::get_motor_status(inst::motor(i)));
		m_motorSenders.insert(std::make_pair(i, std::make_unique<RetrySender>(m_io, packet, device_outgoing_data)));
	}

	for (auto& kvp : m_motorSenders) {
		kvp.second->begin();
	}
}

bool Doctor::all_finished() const
{
	return std::all_of(m_motorSenders.begin(), m_motorSenders.end(), [](const auto& kvp) {
		RetryStatus status = kvp.second->get_status();
		return status == RetryStatus::UserCanceled || status == RetryStatus::ExceededMaxTries;
	});
}

bool Doctor::all_succeeded() const {
	return std::all_of(m_motorSenders.begin(), m_motorSenders.end(), [](const auto& kvp) {
		RetryStatus status = kvp.second->get_status();
		return status == RetryStatus::UserCanceled;
	});
}
