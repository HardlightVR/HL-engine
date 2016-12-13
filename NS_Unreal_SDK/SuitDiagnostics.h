#pragma once
#include "SuitHardwareInterface.h"
class SuitDiagnostics
{
public:

	struct SuitDiagnosticInfo {
		enum class SuitStatus
		{
			BeginInit = 0x00,
			MpuBeginInit = 0x01,
			MpuStartOk = 0x02,
			MpuConnectOk = 0x03,
			MpuDmpInitOk = 0x04,
			MpuConfigOk = 0x05,
			MpuConfigFailMsg = 0x06,
			MpuDmpEnableOk = 0x07,
			MpuDmpReadyOk = 0x08,
			MpuEndInit = 0x09
		};
		enum class DeviceType {
			Imu,
			Drv
		};
		SuitStatus Stage;
		DeviceType Device;
		unsigned int Count;
		uint32_t Message;
		uint32_t Response;
		uint32_t Param1;
		uint32_t Param2;
		SuitDiagnosticInfo() {}
	};
	typedef std::function<void(const SuitDiagnosticInfo& info)> SuitDiagnosticsCallback;
	SuitDiagnostics();
	~SuitDiagnostics();
	void ReceiveDiagnostics(const SuitDiagnosticInfo& info);
private:
	//std::unordered_map<SuitStatusUpdate::SuitStatus, std::unordered_map<unsigned int, std::string>> _errorCodes;
	SuitDiagnosticsCallback _receiveDiagnostics;
};

