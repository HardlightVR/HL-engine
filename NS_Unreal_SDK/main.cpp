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

	auto io = std::make_shared<IoService>();
	io->Start();

	zmq::context_t context(1);
	zmq::socket_t server_updates(context, ZMQ_PUB);
	zmq::socket_t haptic_requests(context, ZMQ_SUB);

	Engine engine(io, _encoder, server_updates);

	boost::asio::deadline_timer suitStatusTimer(*io->GetIOService(), suit_status_update_interval);
	suitStatusTimer.async_wait(boost::bind(sendSuitStatusMsg, boost::asio::placeholders::error, &engine, &_encoder, &server_updates, &suitStatusTimer));
		//haptic_requests.setsockopt(ZMQ_CONFLATE, 16);
		haptic_requests.setsockopt(ZMQ_RCVHWM, 16);
		haptic_requests.bind("tcp://127.0.0.1:9452");
		haptic_requests.setsockopt(ZMQ_SUBSCRIBE, "", 0);

		server_updates.setsockopt(ZMQ_CONFLATE, 1);
		
		server_updates.bind("tcp://127.0.0.1:9453");

		#pragma region Chrono setup
		auto previousTime = std::chrono::high_resolution_clock::now();
		int framecount = 0;
		typedef std::chrono::duration<float, std::ratio<1, 1>> duration;
		float total = 0.0f;
		#pragma endregion
	

		while (true) {
		
			//boost::this_thread::sleep(boost::posix_time::millisec(1));
			//if (io->stopped()) {
			//	std::cout << "resetting io " << '\n';
			//	io->reset();
		//	}
			//io->poll();

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
			//	std::cout << "got something" << "\n";
				auto data = msg.data();
				auto size = msg.size();
				flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data), size);
				if (NullSpace::HapticFiles::VerifyHapticPacketBuffer(verifier)) {
					//std::cout << "	it's a packet" << "\n";
					auto packet = NullSpace::HapticFiles::GetHapticPacket(data);
					switch (packet->packet_type()) {
					case NullSpace::HapticFiles::FileType::FileType_Sequence: 
						engine.PlaySequence(*packet);
						break;
					case NullSpace::HapticFiles::FileType::FileType_Pattern: 
						engine.PlayPattern(*packet);
						break;	
					case NullSpace::HapticFiles::FileType::FileType_HapticEffect:
						engine.PlayEffect(*packet);
						break;
					case NullSpace::HapticFiles::FileType::FileType_Experience:
						engine.PlayExperience(*packet);
						break;
					case NullSpace::HapticFiles::FileType::FileType_Tracking:
						engine.EnableOrDisableTracking(*packet);
						break;
					case NullSpace::HapticFiles::FileType::FileType_HandleCommand:
						engine.HandleCommand(*packet);
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
	
	
	return 0;

}
