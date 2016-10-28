#pragma once

#include "ICommunicationAdapter.h"
#include "InstructionBuilder.h"
#include "Enums.h"
#include "Locator.h"

class SuitHardwareInterface
{
public:
	SuitHardwareInterface(std::shared_ptr<ICommunicationAdapter>, std::shared_ptr<InstructionSet> iset);
	~SuitHardwareInterface();
	void SetAdapter(std::shared_ptr<ICommunicationAdapter> adapter);
	void PlayEffect(Location location, Effect effect);
	void HaltEffect(Location location);
	void PlayEffectContinuous(Location location, Effect effect);
	void HaltAllEffects();
	void PingSuit();
private:
	std::shared_ptr<ICommunicationAdapter> adapter;
	void execute(const Packet& packet) const;
	InstructionBuilder builder;
};

