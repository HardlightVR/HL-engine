#pragma once


#include <memory>
#include "hardlight_device_version.h"
#include <unordered_map>
class Doctor;
class HardwareIO;
class PacketDispatcher;

class diagnostics : public std::enable_shared_from_this<diagnostics>  {
public:

	diagnostics(Doctor* doctor, hardlight_device_version version);

	void begin(HardwareIO* io, PacketDispatcher* dispatcher);
private:

	void queue_motor_requests();

	Doctor* doctor;
	hardlight_device_version version;
	HardwareIO* io;
	PacketDispatcher* dispatcher;
	std::unordered_map<int, int> m_motorStatusBits;
};