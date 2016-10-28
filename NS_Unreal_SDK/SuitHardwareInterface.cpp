#include "SuitHardwareInterface.h"
#include <fstream>

#include <iostream>

SuitHardwareInterface::SuitHardwareInterface(std::shared_ptr<ICommunicationAdapter> a, std::shared_ptr<InstructionSet> iset):adapter(a),builder(iset)
{
	
	
}


SuitHardwareInterface::~SuitHardwareInterface()
{
}

void SuitHardwareInterface::SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter) {
	this->adapter = adapter;
}

void SuitHardwareInterface::PlayEffect(Location location, Effect effect) {

	if (builder.UseInstruction("PLAY_EFFECT")
		.WithParam("zone", Locator::getTranslator().ToString(location))
		.WithParam("effect", Locator::getTranslator().ToString(effect))
		.Verify())
	{
		this->execute(builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << builder.GetDebugString();
	}
}

void SuitHardwareInterface::PlayEffectContinuous(Location location, Effect effect)
{
	if (builder.UseInstruction("PLAY_CONTINUOUS")
		.WithParam("effect", Locator::getTranslator().ToString(effect))
		.WithParam("zone", Locator::getTranslator().ToString(location))
		.Verify())
	{
		this->execute(builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << builder.GetDebugString();
	}
}

 void SuitHardwareInterface::PingSuit()
{
	if (builder.UseInstruction("STATUS_PING").Verify())
	{
		this->execute(builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << builder.GetDebugString();
	}

}


void SuitHardwareInterface::HaltEffect(Location location)
{
	if (builder.UseInstruction("HALT_SINGLE")
		.WithParam("zone", Locator::getTranslator().ToString(location))
		.Verify())
	{
		this->execute(builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << builder.GetDebugString();
	}

}

void SuitHardwareInterface::HaltAllEffects()
{
	//must reimplement
}



void SuitHardwareInterface::execute(const Packet& packet) const
{
	this->adapter->Write(packet.Data, packet.Length);
}
