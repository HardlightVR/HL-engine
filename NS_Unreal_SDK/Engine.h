#pragma once

#include <memory>
#include "SuitHardwareInterface.h"
#include "HapticCache2.h"
#include "PacketDispatcher.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
#include "boost\asio\io_service.hpp"
#include <boost\asio\deadline_timer.hpp>
#include "IoService.h"
#include "InstructionSet.h"
struct suit_status {

};
class Engine
{
public:
	Engine(std::shared_ptr<IoService> io);
	void PlaySequence(const NullSpace::HapticFiles::HapticPacket& packet);
	void PlayPattern(const NullSpace::HapticFiles::HapticPacket& packet);
	void PlayExperience(const NullSpace::HapticFiles::HapticPacket& packet);
	void PlayEffect(const NullSpace::HapticFiles::HapticPacket& packet);
	void Update(float dt);
	bool SuitConnected() const;
	~Engine();
private:
	std::shared_ptr<InstructionSet> _instructionSet; //order dependency

	std::shared_ptr<ICommunicationAdapter> _adapter; //order dependency
	SuitHardwareInterface _defaultInterface;

	HapticCache2 _hapticCache;
	std::shared_ptr<PacketDispatcher> _packetDispatcher; //order dependency
	Synchronizer _streamSynchronizer; //order dependency
	HapticsExecutor _executor;//order dependency

};

