#include "StdAfx.h"

#include "SuitHardwareInterface.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"
#include "HapticDirectoryTools.h"
#include <fstream>
#include <boost\thread.hpp>
#include <boost\asio\placeholders.hpp>
#include <boost\asio\deadline_timer.hpp>
#include <cassert>
#include <chrono>
#include "flatbuffers\flatbuffers.h"
#include "Sequence_generated.h"
#include "HapticEffect_generated.h"
#include "HapticPacket_generated.h"
#include "EnginePacket_generated.h"
#include <memory>
#include "Wire.h"
#include <thread>
#include "Engine.h"
#include "EncodingOperations.h"
#define SHOW_CONSOLE

const auto suit_status_update_interval = boost::posix_time::milliseconds(500);

void sendSuitStatusMsg(const boost::system::error_code& ec, Engine* e, EncodingOperations* encoder, zmq::socket_t* socket, boost::asio::deadline_timer* t) {
	NullSpace::Communication::SuitStatus status = e->SuitConnected() ?
		NullSpace::Communication::SuitStatus::SuitStatus_Connected :
		NullSpace::Communication::SuitStatus::SuitStatus_Disconnected;

	encoder->Finalize(encoder->Encode(status),
		[&](uint8_t* data, int size) {
		Wire::sendTo(*socket, data, size);
	});
	std::cout << "Sending suit update!" << '\n';
	t->expires_at(t->expires_at() + suit_status_update_interval);
	t->async_wait(boost::bind(sendSuitStatusMsg, boost::asio::placeholders::error, e, encoder, socket, t));
}
int main() {
	#ifndef SHOW_CONSOLE
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif

	using namespace std::chrono;
	EncodingOperations _encoder;
	auto io = std::make_shared<boost::asio::io_service>();

	Engine engine(io);

	zmq::context_t context(1);
	zmq::socket_t server_updates(context, ZMQ_PUB);
	zmq::socket_t haptic_requests(context, ZMQ_PAIR);
	boost::asio::deadline_timer suitStatusTimer(*io, suit_status_update_interval);
	suitStatusTimer.async_wait(boost::bind(sendSuitStatusMsg, boost::asio::placeholders::error, &engine, &_encoder, &server_updates, &suitStatusTimer));
	try {
		haptic_requests.bind("tcp://127.0.0.1:9452");
		server_updates.bind("tcp://127.0.0.1:9453");

		#pragma region Chrono setup
		auto previousTime = std::chrono::high_resolution_clock::now();
		int framecount = 0;
		typedef std::chrono::duration<float, std::ratio<1, 1>> duration;
		float total = 0.0f;
		#pragma endregion

		while (true) {
		
			boost::this_thread::sleep(boost::posix_time::millisec(1));
			if (io->stopped()) {
				std::cout << "resetting io " << '\n';
				io->reset();
			}
			io->poll();

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
				std::cout << "got something" << "\n";
				auto data = msg.data();
				auto size = msg.size();
				flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data), size);
				if (NullSpace::HapticFiles::VerifyHapticPacketBuffer(verifier)) {
					std::cout << "	it's a packet" << "\n";
					auto packet =
						std::unique_ptr<const NullSpace::HapticFiles::HapticPacket>(NullSpace::HapticFiles::GetHapticPacket(data));
					switch (packet->packet_type()) {
					case NullSpace::HapticFiles::FileType::FileType_Sequence: 
						engine.PlaySequence(std::move(packet));
						break;
					case NullSpace::HapticFiles::FileType::FileType_Pattern: 
						engine.PlayPattern(packet);
						break;	
					case NullSpace::HapticFiles::FileType::FileType_HapticEffect:
						engine.PlayEffect(std::move(packet));
						break;
					case NullSpace::HapticFiles::FileType::FileType_Experience:
						engine.PlayExperience(std::move(packet));
						break;
					default:
						break;
					}
					packet.release();
				}
				else {
					std::cout << "Bad packet" << "\n";
				}
			}
			engine.Update(elapsed.count());
			
		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	return 0;

}
