#pragma once

#include "ICommunicationAdapter.h"
#include "InstructionBuilder.h"
#include "Enums.h"
#include "Locator.h"
#include <boost\asio.hpp>
#include <mutex>
#include <boost\lockfree\spsc_queue.hpp>
#include <queue>
class SuitHardwareInterface
{
public:
	SuitHardwareInterface(std::shared_ptr<ICommunicationAdapter>, std::shared_ptr<InstructionSet> iset, std::shared_ptr<boost::asio::io_service> io);
	~SuitHardwareInterface();
	void SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter);
	void PlayEffect(Location location, Effect effect);
	void HaltEffect(Location location);
	void PlayEffectContinuous(Location location, Effect effect);
	void HaltAllEffects();
	void Flush();
	void PingSuit();
	void UseImmediateMode();
	void UseDeferredMode();
private:
	std::shared_ptr<boost::asio::io_service> _io;
	std::shared_ptr<ICommunicationAdapter> adapter;
	void executeImmediately(Packet packet);
	void executeLater( Packet packet);
	void chooseExecutionStrategy( Packet packet);
	InstructionBuilder builder;
	bool _useDeferredWriting;
	std::queue<uint8_t> _preWriteBuffer;
	std::vector<uint8_t> _writeBuffer;
//	boost::asio::deadline_timer _writeTimer;
	boost::posix_time::milliseconds _writeInterval = boost::posix_time::milliseconds(5);
	void writeBuffer(const boost::system::error_code, std::size_t t);

	bool _needsFlush;
	std::mutex _needsFlushMutex;

	
};

