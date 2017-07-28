#pragma once
#include <string>

class IHardwareDevice {
public:
	virtual ~IHardwareDevice() {}

	bool has_capability(const std::string& capability);

	virtual void BufferedPlayback() {}

};