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
	void PlayEffect();
private:
	std::shared_ptr<ICommunicationAdapter> adapter;
	void execute(uint8_t* packet, std::size_t length);
	InstructionBuilder builder;
};

