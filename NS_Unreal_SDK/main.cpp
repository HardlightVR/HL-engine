#include "StdAfx.h"

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

#include "Engine.h"


#define SHOW_CONSOLE


//this variable and the following function should probably be part of Engine, but Engine is becoming
//a monolith. 
//This controls how often the engine sends a "I'm plugged in" message to a game. 
const auto suit_status_update_interval = boost::posix_time::milliseconds(250);

void sendSuitStatusMsg(const boost::system::error_code& ec, Engine* e, EncodingOperations* encoder, zmq::socket_t* socket, boost::asio::deadline_timer* t) {
	NullSpace::Communication::SuitStatus status = e->SuitConnected() ?
		NullSpace::Communication::SuitStatus::SuitStatus_Connected :
		NullSpace::Communication::SuitStatus::SuitStatus_Disconnected;

	encoder->Finalize(encoder->Encode(status),
		[&](uint8_t* data, int size) {
		Wire::sendTo(*socket, data, size);
	});
	t->expires_at(t->expires_at() + suit_status_update_interval);
	t->async_wait(boost::bind(sendSuitStatusMsg, boost::asio::placeholders::error, e, encoder, socket, t));
}
int main() {
	#ifndef SHOW_CONSOLE
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif
	
	using namespace std::chrono;
	EncodingOperations _encoder;

	//Keep the underlying io_service in its own thread, which is encapsulated by the IoService object. 
	//This allows us to do io which is not controlled by our update loop, but totally independent
	auto io = std::make_shared<IoService>();
	io->Start();

	//We use the ZeroMQ library to communicate between plugin and engine. 
	//Right now we have a server_updates pub socket, where the Engine publishes anything that a plugin needs to know. 
	//We have a haptic_requests sub socket, which will listen to anyone who is publishing on it. Here we receive haptic information.
	//For example, the plugin may send a Sequence over the wire, and we receive it from this socket. In the future, we will need to keep
	//track of all the "clients", and have a context for each. This context will be sent along with each request. This enables things like
	//switching between games, more advanced caching, etc.
	zmq::context_t context(1);
	zmq::socket_t server_updates(context, ZMQ_PUB);
	zmq::socket_t haptic_requests(context, ZMQ_SUB);

	//Engine is responsible for executing haptics, and sending updates. The engine handles each haptic
	//as it's own entity, capable of playing, pausing, resetting. They are referenced by handles which are passed
	//along with the requests. 
	Engine engine(io, _encoder, server_updates);

	//Start up a timer to execute the update function we defined way at the top. This should probably be part of engine.
	boost::asio::deadline_timer suitStatusTimer(*io->GetIOService(), suit_status_update_interval);
	suitStatusTimer.async_wait(boost::bind(sendSuitStatusMsg, boost::asio::placeholders::error, &engine, &_encoder, &server_updates, &suitStatusTimer));
	

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

	//This is only for seeing FPS
	#pragma region Chrono setup
	auto previousTime = std::chrono::high_resolution_clock::now();
	int framecount = 0;
	typedef std::chrono::duration<float, std::ratio<1, 1>> duration;
	float total = 0.0f;
	#pragma endregion
	

	while (true) {
		
		#pragma region Chrono update
		framecount++;

		auto currentTime = std::chrono::high_resolution_clock::now();
		duration elapsed = currentTime - previousTime;
		previousTime = currentTime;
		total += elapsed.count();

		if (total >= 1.0f) {
			std::cout << "FPS: " << (framecount) << "\n";
			total = 0.0f;
			framecount = 0;
		}
		#pragma endregion

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

		//Update the state of any playing haptics, reset adapter if it needs to be reset,
		//parse packets from the suit
		engine.Update(elapsed.count());
			
	}
	
	
	return 0;

}
