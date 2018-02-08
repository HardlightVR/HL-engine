#pragma once

#include <cstdint>

#include <unordered_map>

#include "Device.h"


enum class MotorProblems : int{
	None = 0,
	LeftDriverBoardNotFound = 1 << 1,
	RightDriverBoardNotFound = 1 << 2
};


using T = std::underlying_type_t <MotorProblems>;

inline MotorProblems operator | (MotorProblems lhs, MotorProblems rhs)

{
	return (MotorProblems)(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline MotorProblems& operator |= (MotorProblems& lhs, MotorProblems rhs)
{
	lhs = (MotorProblems)(static_cast<T>(lhs) | static_cast<T>(rhs));
	return lhs;
}

class MotorDiagnosis {

public:
	void setStatusBits(uint8_t motorId, uint8_t statusBits);

	bool all_ok() const;

	MotorProblems get_problems() const;

private:
	std::unordered_map<uint8_t, HL_Unit::_enumerated> m_statusBits;


};