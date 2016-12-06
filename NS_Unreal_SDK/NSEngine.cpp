#include "StdAfx.h"

#include "NSEngine.h"



#include "SuitHardwareInterface.h"
#include "HapticDirectoryTools.h"
#include <fstream>

#include <cassert>
#include <chrono>
#include "flatbuffers\flatbuffers.h"
#include "Sequence_generated.h"
#include "HapticEffect_generated.h"
#include "HapticPacket_generated.h"
#include "EnginePacket_generated.h"


#include "boost\program_options.hpp"


NSEngine::NSEngine():
	suit_status_update_interval(200),
	_encoder(),
	io(std::make_shared<IoService>()),
	context(1),
	server_updates(context, ZMQ_PUB),
	haptic_requests(context, ZMQ_SUB), 
	engine(io, _encoder, server_updates),
	suitStatusTimer(*io->GetIOService(), suit_status_update_interval)


{

	//The ZMQ sockets require some tweaking. For haptic_requests, we want the "high water mark" to be fairly small. Meaning,
	//if we receive more messages than the HWM, we discard them. This is because we don't want old haptics. 
	//Alternatively, we could use ZMQ_CONFLATE, which only keeps one message, but if we don't process it fast enough it's gone. 
	//The parameter for RCVHWM may arbitrarily map to an amount of messages, not sure. See ZMQ docs. 
	//Also, it must be set before binding (see docs)
	haptic_requests.setsockopt(ZMQ_RCVHWM, 16);
	haptic_requests.bind("tcp://127.0.0.1:9452");
	//Since it's a sub socket, we need a topic to subscribe to. Since we don't use multiple topics, we use "".
	haptic_requests.setsockopt(ZMQ_SUBSCRIBE, "", 0);

	//We don't want server updates buffered at all. Might get stale IMU data, disconnections, reconnections, etc. Also,
	//say the application froze - we don't want them to receive a bunch of junk data, just the most recent.
	server_updates.setsockopt(ZMQ_CONFLATE, 1);
	server_updates.bind("tcp://127.0.0.1:9453");
	lastFrameTime = std::chrono::high_resolution_clock::now();
}


NSEngine::~NSEngine()
{
}


void NSEngine::Update() {
	typedef std::chrono::duration<float, std::ratio<1, 1>> duration;

	duration elapsed = std::chrono::high_resolution_clock::now() - lastFrameTime;

	zmq::message_t msg;
	if (haptic_requests.recv(&msg, ZMQ_DONTWAIT))
	{
		auto data = msg.data();
		auto size = msg.size();
		//We are paying the price of verifying the buffer, but we don't need to if we trust the client's ability
		//to encode messages correctly. 
		flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data), size);
		if (NullSpace::HapticFiles::VerifyHapticPacketBuffer(verifier)) {
			auto packet = NullSpace::HapticFiles::GetHapticPacket(data);
			switch (packet->packet_type()) {
			case NullSpace::HapticFiles::FileType::FileType_Sequence:
				engine.PlaySequence(*packet);
				break;
			case NullSpace::HapticFiles::FileType::FileType_Pattern:
				engine.PlayPattern(*packet);
				break;
			case NullSpace::HapticFiles::FileType::FileType_Experience:
				engine.PlayExperience(*packet);
				break;
			case NullSpace::HapticFiles::FileType::FileType_Tracking:
				engine.EnableOrDisableTracking(*packet);
				break;
				//Commands like Stop, Start, Play, Reset, Pause, etc.
			case NullSpace::HapticFiles::FileType::FileType_HandleCommand:
				engine.HandleCommand(*packet);
				break;
			case NullSpace::HapticFiles::FileType::FileType_EngineCommandData:
				engine.EngineCommand(*packet);
				break;
			default:
				break;
			}
		}
		else {
			std::cout << "Bad packet" << "\n";
		}
	}

	engine.Update(elapsed.count());
}
void NSEngine::sendSuitStatusMsg(const boost::system::error_code& ec,zmq::socket_t* socket)
{
	NullSpace::Communication::SuitStatus status = engine.SuitConnected() ?
		NullSpace::Communication::SuitStatus::SuitStatus_Connected :
		NullSpace::Communication::SuitStatus::SuitStatus_Disconnected;

	_encoder.Finalize(_encoder.Encode(status),
		[&](uint8_t* data, int size) {
		Wire::sendTo(*socket, data, size);
	});
	suitStatusTimer.expires_at(suitStatusTimer.expires_at() + suit_status_update_interval);
	suitStatusTimer.async_wait(boost::bind(&NSEngine::sendSuitStatusMsg, this, boost::asio::placeholders::error, socket));
}
