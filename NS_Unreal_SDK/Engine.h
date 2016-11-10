#pragma once

#include "SuitHardwareInterface.h"
#include "HapticCache2.h"
#include "PacketDispatcher.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
#include "InstructionSet.h"
#include "ImuConsumer.h"

struct suit_status {

};
class Engine
{
public:
	Engine(std::shared_ptr<IoService> io, EncodingOperations& encoder, zmq::socket_t& socket);
	void PlaySequence(const NullSpace::HapticFiles::HapticPacket& packet);
	void PlayPattern(const NullSpace::HapticFiles::HapticPacket& packet);
	void PlayExperience(const NullSpace::HapticFiles::HapticPacket& packet);
	void PlayEffect(const NullSpace::HapticFiles::HapticPacket& packet);
	void HandleCommand(const NullSpace::HapticFiles::HapticPacket& packet);
	void EnableOrDisableTracking(const NullSpace::HapticFiles::HapticPacket& packet);
	void Update(float dt);
	bool SuitConnected() const;
	~Engine();
private:
	std::shared_ptr<InstructionSet> _instructionSet; //order dependency
	std::shared_ptr<ICommunicationAdapter> _adapter; //order dependency
	HapticCache2<
		std::vector<JsonSequenceAtom>, 
		std::vector<JsonSequenceAtom>, 
		std::vector<JsonSequenceAtom>> 
	_hapticCache;
	std::shared_ptr<PacketDispatcher> _packetDispatcher; //order dependency
	Synchronizer _streamSynchronizer; //order dependency
	HapticsExecutor _executor;//order dependency

	std::shared_ptr<ImuConsumer> _imuConsumer;
	boost::asio::deadline_timer _trackingUpdateTimer;
	boost::posix_time::milliseconds _trackingUpdateInterval = boost::posix_time::milliseconds(100);

	EncodingOperations& _encoder;
	zmq::socket_t& _socket;
	void sendTrackingUpdate();

	//todo: App Sessions
	bool _userRequestsTracking;
};

