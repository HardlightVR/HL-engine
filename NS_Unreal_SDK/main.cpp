#include "StdAfx.h"

#include "SuitHardwareInterface.h"

#include "zmq.hpp"
#include "HapticDirectoryTools.h"
#include "DependencyResolver.h"
#include "HapticFileInfo.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
#include <fstream>
#include <boost\thread.hpp>
#include <cassert>
#include <chrono>
#include "flatbuffers\flatbuffers.h"
#include "flatbuff_defs\Sequence_generated.h"
#include "flatbuff_defs\HapticEffect_generated.h"
#include "SerialAdapter.h"
#include <memory>
#define SHOW_CONSOLE


int main() {
	#ifndef SHOW_CONSOLE
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif
	using namespace std::chrono;
	auto suit = std::make_shared<SuitHardwareInterface>();

	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	if (!adapter->Connect()) {
		std::cout << "Unable to connect to suit" << "\n";
	}
	else {
		std::cout << "Connected to suit" << "\n";
	}
	


	suit->SetAdapter(adapter);
	HapticsExecutor exec(suit);

	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PAIR);
	try {
		socket.bind("tcp://127.0.0.1:5555");
		auto previousTime = std::chrono::high_resolution_clock::now();
		while (true) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			typedef std::chrono::duration<float, std::ratio<1,1>> duration;
			duration elapsed = currentTime - previousTime;
			previousTime = currentTime;
			exec.Update(elapsed.count());
			
			zmq::message_t msg;
			if (socket.recv(&msg, ZMQ_DONTWAIT) != -1);
			{
				auto data = msg.data();
				flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data), msg.size());
				bool isgood = NullSpace::HapticFiles::VerifySequenceBuffer(verifier);
				if (isgood) {

					auto sequence = NullSpace::HapticFiles::GetSequence(data);
					std::vector<HapticEffect> effects;
					effects.reserve(sequence->items()->size());
					for (const auto &effect : *sequence->items()) {
						effects.push_back(
							HapticEffect((Effect)effect->effect(), (Location)effect->location(), effect->duration(), effect->time(), (int)effect->priority())
						);
					}
					exec.Play(effects);
					
				}
			}
			
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
