#include "SuitHardwareInterface.h"
#include <fstream>

#include <iostream>

SuitHardwareInterface::SuitHardwareInterface()
{
	Json::Value inst_root;
	std::ifstream instruction_json("Instructions.json", std::ifstream::binary);
	instruction_json >> inst_root;
	this->builder.LoadInstructions(inst_root);
	
	Json::Value zone_root;
	std::ifstream zone_json("Zones.json", std::ifstream::binary);
	zone_json >> zone_root;
	this->builder.LoadZones(zone_root);
	
	Json::Value effect_root;
	std::ifstream effect_json("Effects.json", std::ifstream::binary);
	effect_json >> effect_root;
	this->builder.LoadEffects(effect_root);
}


SuitHardwareInterface::~SuitHardwareInterface()
{
}

void SuitHardwareInterface::SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter) {
	this->adapter = adapter;
}

void SuitHardwareInterface::PlayEffect(std::string location, std::string effect) {

	if (builder.UseInstruction("PLAY_EFFECT")
		.WithParam("zone", location)
		.WithParam("effect", effect)
		.Verify())
	{
		this->execute(builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << builder.GetDebugString();
	}
}

void SuitHardwareInterface::PlayEffectContinuous(std::string location, std::string effect)
{
	if (builder.UseInstruction("PLAY_CONTINUOUS")
		.WithParam("effect", effect)
		.WithParam("zone", location)
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


void SuitHardwareInterface::HaltEffect(std::string location)
{
	if (builder.UseInstruction("HALT_SINGLE")
		.WithParam("zone", location)
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



void SuitHardwareInterface::execute(Packet packet) {
	this->adapter->Write(packet.Data, packet.Length);
}