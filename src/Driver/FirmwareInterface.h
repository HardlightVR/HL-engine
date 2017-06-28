#pragma once
#include "ICommunicationAdapter.h"
#include "InstructionBuilder.h"
#include "Enums.h"


class FirmwareInterface
{
	
public:
	struct AudioOptions {
		int AudioMax;
		int AudioMin;
		int PeakTime;
		int Filter;
	};
	FirmwareInterface(std::unique_ptr<ICommunicationAdapter>& adapter, boost::asio::io_service& io);
	~FirmwareInterface();

	void PlayEffect(Location location, uint32_t effect, float strength);
	void HaltEffect(Location location);
	void PlayEffectContinuous(Location location, uint32_t effect, float strength);


	void EnableTracking();
	void DisableTracking();
	void RequestSuitVersion();
	void ReadDriverData(Location loc, Register reg);

	void EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions&);
	void EnableIntrigMode(Location pad);
	void EnableRtpMode(Location pad);
	void PlayRtp(Location location, int strength);
	void Ping();
	void RawCommand(const uint8_t* bytes, std::size_t length);
private:
	inline void VerifyThenExecute(InstructionBuilder& builder);
	std::shared_ptr<InstructionSet> m_instructionSet;
	void chooseExecutionStrategy(const Packet&  packet);
	std::unique_ptr<ICommunicationAdapter>& _adapter;
	InstructionBuilder _builder;
	boost::asio::deadline_timer _writeTimer;
	boost::asio::deadline_timer _batchingDeadline;
	boost::posix_time::milliseconds _batchingTimeout;
	boost::posix_time::milliseconds _writeInterval;
	void writeBuffer();
	boost::lockfree::spsc_queue<uint8_t> _lfQueue;
	const unsigned int BATCH_SIZE;
	bool _isBatching;

	
};

