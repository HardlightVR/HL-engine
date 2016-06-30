#pragma once

#include "ICommunicationAdapter.h"
#include "InstructionBuilder.h"
#include "Enums.h"
#include "EnumTranslator.h"
class SuitHardwareInterface
{
public:
	SuitHardwareInterface();
	~SuitHardwareInterface();
	void SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter);
	void PlayEffect(std::string location, std::string effect);
	void HaltEffect(Location location);
	void PlayEffectContinuous(std::string location, std::string effect);
	void HaltAllEffects();
	void PingSuit();
private:
	std::shared_ptr<ICommunicationAdapter> adapter;
	void execute(Packet packet);
	EnumTranslator translator;
	InstructionBuilder builder;
};

