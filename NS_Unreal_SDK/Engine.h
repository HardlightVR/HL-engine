#pragma once

#include <memory>
#include "SuitHardwareInterface.h"
#include "HapticCache2.h"
#include "PacketDispatcher.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
class Engine
{
public:
	Engine(std::shared_ptr<boost::asio::io_service> io);
	~Engine();
private:
	std::shared_ptr<SuitHardwareInterface> _suitHardware;
	std::shared_ptr<ICommunicationAdapter> _adapter;
	HapticCache2 _hapticCache;
	PacketDispatcher _packetDispatcher;
	Synchronizer _streamSynchronizer;
	HapticsExecutor _executor;

};

