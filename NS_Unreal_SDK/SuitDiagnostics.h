#pragma once
#include "SuitHardwareInterface.h"

class SuitStatusConsumer;
class SuitInfoConsumer;
class FifoConsumer;
class IPacketConsumer;
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

	struct VersionInfo {
		unsigned int Major;
		unsigned int Minor;
		VersionInfo(unsigned int major, unsigned int minor) :Major(major), Minor(minor) {}
	};

	typedef std::function<void(const VersionInfo&)> SuitVersionCallback;
	typedef std::function<void(const SuitDiagnosticInfo&)> SuitDiagnosticsCallback;

	SuitDiagnostics();
	~SuitDiagnostics();
	void ReceiveDiagnostics(const SuitDiagnosticInfo& info);
	void ReceiveVersion(const VersionInfo& info);
	//todo: badly named
	//status is only received on suit connection, whereas info could be received at any time
	const std::unique_ptr<IPacketConsumer>& StatusConsumer();
	const std::unique_ptr<IPacketConsumer>& InfoConsumer();
	const std::unique_ptr<IPacketConsumer>& OverflowConsumer();

	void OnReceiveVersion(SuitVersionCallback);


private:
	SuitDiagnosticsCallback _receiveDiagnostics;

	std::unique_ptr<IPacketConsumer> _statusConsumer;
	std::unique_ptr<IPacketConsumer> _infoConsumer;
	std::unique_ptr<IPacketConsumer> _fifoConsumer;

	SuitVersionCallback _callback;
};

