#include "StdAfx.h"
#define NOMINMAX

#include "NSEngine.h"



#include "SuitHardwareInterface.h"
#include "HapticDirectoryTools.h"
#include <fstream>

#include <cassert>
#include <chrono>

#include "flatbuffers\flatbuffers.h"
#include "Sequence_generated.h"
#include "HapticEffect_generated.h"
#include "Node_generated.h"
#include "HapticPacket_generated.h"
#include "EnginePacket_generated.h"


#include "boost\program_options.hpp"


NSEngine::NSEngine():
	suit_status_update_interval(200),
	_encoder(),
	io(new IoService()),
	context(1),
	server_updates(context, ZMQ_PUB),
	haptic_requests(context, ZMQ_SUB), 
	engine(io, _encoder, server_updates),
	suitStatusTimer(*io->GetIOService(), suit_status_update_interval),
	_running(false)


{


	//The ZMQ sockets require some tweaking. For haptic_requests, we want the "high water mark" to be fairly small. Meaning,
	//if we receive more messages than the HWM, we discard them. This is because we don't want old haptics. 
	//Alternatively, we could use ZMQ_CONFLATE, which only keeps one message, but if we don't process it fast enough it's gone. 
	//The parameter for RCVHWM may arbitrarily map to an amount of messages, not sure. See ZMQ docs. 
	//Also, it must be set before binding (see docs)
	haptic_requests.setsockopt(ZMQ_RCVHWM, 16);
	haptic_requests.setsockopt(ZMQ_SUBSCRIBE, "", 0);

	haptic_requests.bind("tcp://127.0.0.1:9452");
	//Since it's a sub socket, we need a topic to subscribe to. Since we don't use multiple topics, we use "".

	//We don't want server updates buffered at all. Might get stale IMU data, disconnections, reconnections, etc. Also,
	//say the application froze - we don't want them to receive a bunch of junk data, just the most recent.
	int confl = 1;

	server_updates.bind("tcp://127.0.0.1:9453");
	server_updates.setsockopt(ZMQ_CONFLATE, &confl, sizeof(confl));

	lastFrameTime = std::chrono::high_resolution_clock::now();

	suitStatusTimer.async_wait(boost::bind(&NSEngine::sendSuitStatusMsg,this, boost::asio::placeholders::error, &server_updates));

}


NSEngine::~NSEngine()
{
}

void NSEngine::StartThread()
{
	_running = true;
	_workThread = std::thread(&NSEngine::_UpdateLoop, this);
}


void NSEngine::Update() {
	
	typedef std::chrono::duration<float, std::ratio<1, 1>> duration;
	auto timeNow = std::chrono::high_resolution_clock::now();
	duration elapsed = timeNow - lastFrameTime;
	lastFrameTime = timeNow;
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
			case NullSpace::HapticFiles::FileType::FileType_Node:
				engine.Play(*packet);
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
	
}
bool NSEngine::Shutdown()
{
	_running = false;

	if (_workThread.joinable()) {
		_workThread.join();
	}
	//After this point, we have stopped doing updates, 
	//so it's safe to cancel the status timer and stop the Io service
	suitStatusTimer.cancel();

	io->Stop();

	//Send one last update 
	_encoder.AquireEncodingLock();
	_encoder.Finalize(_encoder.Encode(NullSpace::Communication::SuitStatus::SuitStatus_Disconnected),
		[&](uint8_t* data, int size) {
		Wire::sendTo(server_updates, data, size);
	});
	_encoder.ReleaseEncodingLock();
	return true;
}
void NSEngine::sendSuitStatusMsg(const boost::system::error_code& ec,zmq::socket_t* socket)
{
	
	NullSpace::Communication::SuitStatus status = engine.SuitConnected() ?
		NullSpace::Communication::SuitStatus::SuitStatus_Connected :
		NullSpace::Communication::SuitStatus::SuitStatus_Disconnected;
	_encoder.AquireEncodingLock();
	_encoder.Finalize(_encoder.Encode(status),
		[&](uint8_t* data, int size) {
		Wire::sendTo(*socket, data, size);
	});
	_encoder.ReleaseEncodingLock();
	suitStatusTimer.expires_at(suitStatusTimer.expires_at() + suit_status_update_interval);
	suitStatusTimer.async_wait(boost::bind(&NSEngine::sendSuitStatusMsg, this, boost::asio::placeholders::error, socket));
}
	
}

void NSEngine::_UpdateLoop()
{
	while (_running) {
		this->Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
