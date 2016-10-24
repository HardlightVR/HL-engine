#include "StdAfx.h"

#include "SuitHardwareInterface.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"
#include "HapticDirectoryTools.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
#include "PacketDispatcher.h"
#include <fstream>
#include <boost\thread.hpp>
#include <cassert>
#include <chrono>
#include "flatbuffers\flatbuffers.h"
#include "Sequence_generated.h"
#include "HapticEffect_generated.h"
#include "HapticPacket_generated.h"
#include "EnginePacket_generated.h"
#include "SerialAdapter.h"
#include <memory>
#include "HapticCache2.h"
#include "Wire.h"
#include "BoostSerialAdapter.h"
#include <thread>
#define SHOW_CONSOLE


void tick(const boost::system::error_code& e) {
	
}
int main() {
#ifndef SHOW_CONSOLE
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	using namespace std::chrono;
	auto suit = std::make_shared<SuitHardwareInterface>();
	auto io = std::make_shared<boost::asio::io_service>();
	boost::asio::deadline_timer timer(*io, boost::posix_time::seconds(1));

	std::shared_ptr<ICommunicationAdapter> adapter(new BoostSerialAdapter(io));
	if (!adapter->Connect()) {
		std::cout << "Unable to connect to suit" << "\n";
	}
	
	else {
		std::cout << "Connected to suit" << "\n";
		suit->SetAdapter(adapter);
	}

	HapticCache2 _cache;
	PacketDispatcher dispatcher(adapter->GetDataStream());

	Synchronizer sync(adapter->GetDataStream(), dispatcher);

	std::function<void(const boost::system::error_code&)> f = [&](const boost::system::error_code&) {
		suit->PingSuit();
		timer.expires_at(timer.expires_at() + boost::posix_time::seconds(1));
		timer.async_wait(f);
	};
	//timer.async_wait();
	HapticsExecutor exec(suit);
	timer.async_wait(f);
	zmq::context_t context(1);
	zmq::socket_t server_updates(context, ZMQ_PUB);
	zmq::socket_t socket(context, ZMQ_PAIR);
	try {
		socket.bind("tcp://127.0.0.1:9452");
		server_updates.bind("tcp://127.0.0.1:9453");
		auto previousTime = std::chrono::high_resolution_clock::now();
		int framecount = 0;
		typedef std::chrono::duration<float, std::ratio<1, 1>> duration;
		float total = 0.0f;
		//duration total = 
		while (true) {
		
			boost::this_thread::sleep(boost::posix_time::millisec(1));
			framecount++;
			io->poll();
			adapter->Read();
			sync.TryReadPacket();
			auto currentTime = std::chrono::high_resolution_clock::now();
			duration elapsed = currentTime - previousTime;
			previousTime = currentTime;
			total += elapsed.count();
		//	std::cout << "Tick" << elapsed.count()<< "\n";
			if (total >= 1.0f) {
				//x frames : 1 second
				std::cout << "FPS: " << (framecount) << "\n";
				total = 0.0f;
				framecount = 0;
			}

			zmq::message_t msg;
			if (socket.recv(&msg, ZMQ_DONTWAIT))
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
					case NullSpace::HapticFiles::FileType::FileType_Sequence: {
						if (_cache.ContainsSequence(packet->name()->str())) {
							exec.Play(_cache.GetSequence(packet->name()->str()));
						}
						else {
						auto decoded = Wire::Decode(static_cast<const NullSpace::HapticFiles::Sequence*>(packet->packet()));
							_cache.AddSequence(packet->name()->str(), decoded);
							exec.Play(decoded);
							
						}
						break;
					}
					case NullSpace::HapticFiles::FileType::FileType_Pattern: {
						std::cout << "		got pattern" << "\n";
						if (_cache.ContainsPattern(packet->name()->str())) {
							exec.Play(_cache.GetPattern(packet->name()->str()));
						}
						else {
							auto decoded = Wire::Decode(static_cast<const NullSpace::HapticFiles::Pattern*>(packet->packet()));
							_cache.AddPattern(packet->name()->str(), decoded);
							exec.Play(decoded);
						}
						break;
					}
																		
					case NullSpace::HapticFiles::FileType::FileType_Experience:
					{
						if (_cache.ContainsExperience(packet->name()->str())) {
							exec.Play(_cache.GetExperience(packet->name()->str()));
						}
						else {
							auto decoded = Wire::Decode(static_cast<const NullSpace::HapticFiles::Experience*>(packet->packet()));
							_cache.AddExperience(packet->name()->str(), decoded);
							exec.Play(decoded);
						}
						break;
					}
					case NullSpace::HapticFiles::FileType::FileType_HapticEffect:
					{
						auto decoded = Wire::Decode(static_cast<const NullSpace::HapticFiles::HapticEffect*>(packet->packet()));
						exec.Play(decoded);
						break;
					}
					default:
						break;
					}
		
					//std::cout << "Received packet of type " << int(packetType) <<" and size " << msg.size() << "\n";
					packet.release();
				}
				else {
					std::cout << "Bad packet" << "\n";
				}
			}
			exec.Update(elapsed.count());




		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	return 0;
	/*


	//Parser p;
	//p.SetBasePath("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	//p.EnumerateHapticFiles();

	resolver.Load(SequenceFileInfo("ns.strong_click2"));
	resolver.Load(PatternFileInfo("ns.full_body_jolt2"));

	auto effect = resolver.ResolvePattern("ns.full_body_jolt2", Side::Mirror);


	//Synchronizer sync(adapter->GetDataStream(), std::make_shared<PacketDispatcher>());

//	std::vector<HapticEffect> effect;
//	effect.push_back(HapticEffect(Effect::Smooth_Hum_50, Location::Chest_Left, 1.0, 0.0, 1));
	exec.Play(effect);
	for (auto i = 0; i < 1000; i++)
	{

		exec.Update(.1f);
		boost::this_thread::sleep(boost::posix_time::millisec(100));

	}

	*/


}
