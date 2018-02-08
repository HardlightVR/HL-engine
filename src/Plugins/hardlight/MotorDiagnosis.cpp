#include "stdafx.h"
#include "MotorDiagnosis.h"

//MotorStatus extractDrvData(uint8_t data) {
//	bool over_current = data & 0x01;
//	bool over_temperature = data & 0x02;
//		
//	bool mode = data & (0x07);
//	
//	bool standby = data & 0b00100000;
//	
//	
//	if (over_current || over_temperature) {
//		return MotorStatus::Bad;
//	}
//
//	return MotorStatus::Good;
//	
//		
//}

void MotorDiagnosis::setStatusBits(uint8_t motorId, uint8_t statusBits)
{
	m_statusBits[motorId] = static_cast<HL_Unit::_enumerated>(statusBits);
}



bool MotorDiagnosis::all_ok() const
{
	return std::all_of(m_statusBits.begin(), m_statusBits.end(), [](const auto& kvp) {
		auto status = kvp.second;
		return status & HL_Unit::Ready;
	});
}

MotorProblems MotorDiagnosis::get_problems() const
{
	
	bool all_left_good = std::all_of(m_statusBits.begin(), m_statusBits.end(), [](const auto& kvp) {
		if (kvp.first >= 0x10 || kvp.first <= 0x17) {
			return (bool) (kvp.second & HL_Unit::Ready);
		}

		return true;
	});

	bool all_right_good = std::all_of(m_statusBits.begin(), m_statusBits.end(), [](const auto& kvp) {
		if (kvp.first >= 0x18 || kvp.first <= 0x1f) {
			return (bool) (kvp.second & HL_Unit::Ready);
		}

		return true;
	});

	MotorProblems probs = MotorProblems::None;
	if (!all_left_good) {
		probs |= MotorProblems::LeftDriverBoardNotFound;
	}

	if (all_right_good) {
		probs |= MotorProblems::RightDriverBoardNotFound;
	}

	return probs;
}
