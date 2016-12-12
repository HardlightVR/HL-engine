#include "FifoConsumer.h"
#include <iostream>


FifoConsumer::FifoConsumer()
{
}


FifoConsumer::~FifoConsumer()
{
}

void FifoConsumer::ConsumePacket(const packet & packet)
{
	std::cout << "Rec'd a FIFO PACKET!" << '\n';
}
