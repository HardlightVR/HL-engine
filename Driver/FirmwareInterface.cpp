#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"

FirmwareInterface::FirmwareInterface(std::unique_ptr<ICommunicationAdapter>& adapter, boost::asio::io_service& io):
_adapter(adapter),
_builder(std::make_shared<InstructionSet>()),
_writeTimer(io),
_batchingDeadline(io),
_writeInterval(10),
_batchingTimeout(20),
BATCH_SIZE(64),
_lfQueue(512)

{
}


FirmwareInterface::~FirmwareInterface()
{
}

void FirmwareInterface::PlayEffect(Location location, Effect effect) {

	if (_builder.UseInstruction("PLAY_EFFECT")
		.WithParam("zone", Locator::Translator().ToString(location))
		.WithParam("effect", Locator::Translator().ToString(effect))
		.Verify())
	{
		chooseExecutionStrategy(_builder.Build());

	}
	else
	{
		std::cout << "Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::PlayEffectContinuous(Location location, Effect effect)
{
	if (_builder.UseInstruction("PLAY_CONTINUOUS")
		.WithParam("effect", Locator::Translator().ToString(effect))
		.WithParam("zone", Locator::Translator().ToString(location))
		.Verify())
	{
		chooseExecutionStrategy(_builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::chooseExecutionStrategy(const Packet & packet)
{
}


void FirmwareInterface::HaltEffect(Location location)
{
	if (_builder.UseInstruction("HALT_SINGLE")
		.WithParam("zone", Locator::Translator().ToString(location))
		.Verify())
	{
		chooseExecutionStrategy(_builder.Build());
	}
	else
	{
		std::cout << "Failed to build instruction " << _builder.GetDebugString();
	}

}
