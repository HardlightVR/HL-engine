#include "StdAfx.h"
#include "SuitStatusConsumer.h"
#include "SuitHardwareInterface.h"



SuitStatusConsumer::SuitStatusConsumer()
{
}


SuitStatusConsumer::~SuitStatusConsumer()
{
}

void SuitStatusConsumer::ConsumePacket(const packet & packet)
{
	SuitHardwareInterface::SuitStatusUpdate update(packet.raw[3], packet.raw[4]);
	std::cout << "Suit init update: Stage " << int(update.Stage) << ", Response: " << update.Response;
	
}
