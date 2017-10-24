#pragma once
#include <cstdint>
#include "better_enum.h"
BETTER_ENUM(HL_Unit, uint8_t, 
	None = 0x00, //! HL_UNIT_NONE - Ignore this address
	Required = 0x01, //! HL_UNIT_REQUIRED - Complain if not found in a scan
	Optional = 0x02, //! HL_UNIT_OPTIONAL - Be silent if not found in a scan
	Installed = 0x04, //! HL_UNIT_INSTALLED -  Was found in a scan once upon a time
	Ready = 0x08, //! HL_UNIT_READY -  Successfully Initialize and Ready
	Error = 0x10, //! HL_UNIT_ERROR -  TImed out at least once upon a time
	Busy = 0x20, //! HL_UNIT_BUSY -  Busy during the last scan
	Mode = 0x40, //! HL_UNIT_MODE - 0 = Normal, 1 = Continuous
	Enabled = 0x80 //! HL_UNIT_ENABLED - add to scan or ignore it
);