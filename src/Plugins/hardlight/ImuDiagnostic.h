#pragma once

#include "RetrySender.h"
#include "HL_Firmware_Defines.h"
#include "HardwareDiagnostic.h"


class ImuDiagnostic : public HardwareDiagnostic {
public:
	ImuDiagnostic(boost::asio::io_service& io, std::vector<uint8_t> imus);
	// Inherited via HardwareDiagnostic
	virtual void run(HardwareIO* io) override;
	virtual void cancel() override;
	virtual TestProgress get_progress() const override;
	virtual HardwareFailures get_results() const override;
private:
	boost::asio::io_service& m_io;
	std::vector<uint8_t> m_imus;
	std::unordered_map<uint8_t, std::unique_ptr<RetrySender>> m_senders;
	std::unordered_map<uint8_t, HL_Unit::_enumerated> m_statusBits;
};