#pragma once
#include "ICommunicationAdapter.h"
#include "InstructionBuilder.h"
#include "Enums.h"
class FirmwareInterface
{
public:
	FirmwareInterface(std::unique_ptr<ICommunicationAdapter>& adapter, boost::asio::io_service& io);
	~FirmwareInterface();

	void PlayEffect(Location location, std::string effect, float strength);
	void HaltEffect(Location location);
	//void RequestSuitVersion();
	void PlayEffectContinuous(Location location, std::string effect, float strength);
	//void HaltAllEffects();
	//void PingSuit();
	//void EnableIMUs();
	//void DisableIMUs();
private:
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
	const int BATCH_SIZE;
	bool _isBatching;
public:
	void EnableTracking();
	void DisableTracking();
	void RequestSuitVersion();
};

