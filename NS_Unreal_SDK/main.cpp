#include "StdAfx.h"
#include <iostream>
#include <stdio.h>

#include "SuitHardwareInterface.h"


#include "SerialAdapter.h"
#include "Enums.h"
int main() {
	

	
	SuitHardwareInterface suit;
	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	adapter->Connect();

	suit.SetAdapter(adapter);
	suit.PlayEffectContinuous("Chest_Left", "Strong_Click_100");
	Sleep(5000);
	suit.HaltEffect(Location::Chest_Left);
	
}