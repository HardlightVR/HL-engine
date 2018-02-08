#include "stdafx.h"
#include "Diagnostics.h"
#include "DeviceManager.h"
#include "PacketDispatcher.h"
diagnostics::diagnostics(Doctor * doctor, hardlight_device_version version)
	: doctor(doctor)
	, version(version)
{
}

void diagnostics::begin(HardwareIO * io, PacketDispatcher * dispatcher)
{
	auto self(shared_from_this());
	dispatcher->AddConsumer(inst::Id::GET_MOTOR_STATUS, [this, self](Packet packet) {
		m_motorStatusBits[packet[4]] = packet[3];
	});

	for (int i = 0x10; i <= 0x1f; i++) {
		auto packet = inst::Build(inst::get_motor_status(inst::motor(i)));
		io->outgoing_queue()->push(packet.data(), packet.size());
	}
}

void diagnostics::queue_motor_requests()
{
	for (int i = 0x10; i <= 0x1f; i++) {
		auto packet = inst::Build(inst::get_motor_status(inst::motor(i)));
		io->outgoing_queue()->push(packet.data(), packet.size());
	}
}
