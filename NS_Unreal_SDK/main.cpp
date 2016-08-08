#include "StdAfx.h"
#include <iostream>
#include <stdio.h>

#include "SuitHardwareInterface.h"


#include "SerialAdapter.h"
#include "Enums.h"


#include "HapticDirectoryTools.h"
int main() {
	

	
	SuitHardwareInterface suit;
	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	adapter->Connect();

	suit.SetAdapter(adapter);


	HapticDirectoryTools::HapticEnumerator enumer("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	auto files = enumer.EnumerateFiles();
	auto whats = enumer.GetEnums(files);
	auto whoozits = enumer.GenerateNodes(whats);

	std::cin.get();

	
	
}