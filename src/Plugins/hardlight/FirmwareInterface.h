#pragma once
#include "BoostSerialAdapter.h"
#include "InstructionBuilder.h"
#include "Enums.h"
#include <boost/asio/io_service.hpp>
#include "zone_logic/HardwareCommands.h"
class FirmwareInterface
{
	
public:
	
	struct AudioOptions {
		int AudioMax;
		int AudioMin;
		int PeakTime;
		int Filter;
	};
	FirmwareInterface(const std::string& data_dir, std::unique_ptr<BoostSerialAdapter>& adapter, boost::asio::io_service& io);
	~FirmwareInterface();

	void Execute(const CommandBuffer& buffer);
	void PlayEffect(Location location, uint32_t effect, float strength);
	void HaltEffect(Location location);
	void PlayEffectContinuous(Location location, uint32_t effect, float strength);


	void EnableTracking();
	void DisableTracking();
	void RequestSuitVersion();
	void ReadDriverData(Location loc, Register reg);
	void ResetDrivers();

	void EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions&);
	void EnableIntrigMode(Location pad);
	void EnableRtpMode(Location pad);
	void PlayRtp(Location location, int strength);
	void Ping();
	void RawCommand(const uint8_t* bytes, std::size_t length);

	std::size_t GetTotalBytesSent() const;
private:
	inline void VerifyThenExecute(InstructionBuilder& builder);
	std::shared_ptr<InstructionSet> m_instructionSet;
	void chooseExecutionStrategy(const std::vector<uint8_t>& packet);
	std::unique_ptr<BoostSerialAdapter>& _adapter;
	InstructionBuilder _builder;
	boost::asio::deadline_timer _writeTimer;
	boost::asio::deadline_timer _batchingDeadline;
	boost::posix_time::milliseconds _batchingTimeout;
	boost::posix_time::milliseconds _writeInterval;
	void writeBuffer();
	boost::lockfree::spsc_queue<uint8_t> _lfQueue;
	const unsigned int BATCH_SIZE;
	bool _isBatching;

	std::size_t m_totalBytesSent;
	
};

