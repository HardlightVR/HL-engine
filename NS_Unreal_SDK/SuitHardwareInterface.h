#pragma once

#include "StdAfx.h"
#include "ICommunicationAdapter.h"
#include "InstructionBuilder.h"
class SuitHardwareInterface
{
public:
	SuitHardwareInterface();
	~SuitHardwareInterface();
	void SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter);
	void PlayEffect(std::string location, std::string effect);
	void HaltEffect(std::string location);
	void PlayEffectContinuous(std::string location, std::string effect);
	void HaltAllEffects();
	void PingSuit();
private:
	std::shared_ptr<ICommunicationAdapter> adapter;
	void execute(Packet packet);
	InstructionBuilder builder;
};

