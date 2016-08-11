#include "StdAfx.h"
#include <iostream>

#include "SuitHardwareInterface.h"


#include "SerialAdapter.h"

#include "HapticDirectoryTools.h"
#include "DependencyResolver.h"
#include "HapticFileInfo.h"
int main() {
	

	
	SuitHardwareInterface suit;
	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	adapter->Connect();

	suit.SetAdapter(adapter);

	//Parser p;
	//p.SetBasePath("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	//p.EnumerateHapticFiles();

	DependencyResolver resolver("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	resolver.Load(SequenceFileInfo("ns.rumble - Copy"));
	resolver.ResolveSequence("ns.rumble - Copy", Location::Chest_Left);
	std::cin.get();

	
	
}
