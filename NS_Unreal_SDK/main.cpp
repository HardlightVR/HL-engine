#include "StdAfx.h"

#include "SuitHardwareInterface.h"

#include "zmq.hpp"
#include "HapticDirectoryTools.h"
#include "DependencyResolver.h"
#include "HapticFileInfo.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
#include <fstream>
#include <cassert>
#include <chrono>


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
//	if (!adapter->Connect()) {
	//	std::cout << "Unable to connect to suit";
	//	exit(0);
	//}

	suit->SetAdapter(adapter);
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PAIR);
	try {
		socket.bind("tcp://127.0.0.1:5555");
		while (true) {
			zmq::message_t request;
			socket.recv(&request);
			std::cout << request.data() << std::endl;
			Sleep(1);
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
	HapticsExecutor exec(suit);
	
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
