#include "StdAfx.h"
#include <iostream>
#include <stdio.h>

#include "SuitHardwareInterface.h"


#include "SerialAdapter.h"
int main() {
	

	
	SuitHardwareInterface suit;
	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	adapter->Connect();

	suit.SetAdapter(adapter);
	std::cin.get();
	suit.PlayEffect();
}