#include "StdAfx.h"
#include <iostream>

#include "SuitHardwareInterface.h"

#include <boost/thread/thread.hpp> 
#include "SerialAdapter.h"

#include "HapticDirectoryTools.h"
#include "DependencyResolver.h"
#include "HapticFileInfo.h"
#include "Synchronizer.h"
int main() {
	

	
	SuitHardwareInterface suit;
	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	adapter->Connect();

	suit.SetAdapter(adapter);

	//Parser p;
	//p.SetBasePath("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	//p.EnumerateHapticFiles();

	//DependencyResolver resolver("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	//resolver.Load(SequenceFileInfo("ns.rumble - Copy"));
	//resolver.ResolveSequence("ns.rumble - Copy", Location::Chest_Left);
	

	Synchronizer sync(adapter->GetDataStream(), std::make_shared<PacketDispatcher>());

	for (int i = 0; i < 100; i++)
	{
		adapter->Read();
		sync.TryReadPacket();
		boost::this_thread::sleep(boost::posix_time::millisec(100));

	}


	
	
}
