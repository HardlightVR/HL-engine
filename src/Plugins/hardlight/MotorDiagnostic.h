#pragma once


#include "HardwareDiagnostic.h"
#include "RetrySender.h"
#include "HL_Firmware_Defines.h"
#include <vector>
#include <memory>

class MotorDiagnostic : public HardwareDiagnostic {

public:
	MotorDiagnostic(boost::asio::io_service& io, std::vector<uint8_t> listOfMotors);
	// Inherited via HardwareDiagnostic
	void run(IncomingData * in, OutgoingData * out) override;
	HardwareFailures get_results() const override;
	TestProgress get_progress() const override;
	void cancel() override;

private:
	boost::asio::io_service& m_io;
	std::vector<uint8_t> m_motors;
	std::unordered_map<uint8_t, std::unique_ptr<RetrySender>> m_motorSenders;
	std::unordered_map<uint8_t, HL_Unit::_enumerated> m_statusBits;

};