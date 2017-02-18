#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"
#include "InstructionSet.h"
FirmwareInterface::FirmwareInterface(std::unique_ptr<ICommunicationAdapter>& adapter, boost::asio::io_service& io):
m_instructionSet(std::make_shared<InstructionSet>()),
_adapter(adapter),
_builder(m_instructionSet),
_writeTimer(io),
_batchingDeadline(io),
_writeInterval(10),
_batchingTimeout(20),
BATCH_SIZE(64),
_lfQueue(512)

{

	_writeTimer.expires_from_now(_writeInterval);
	_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
}


FirmwareInterface::~FirmwareInterface()
{
}

void FirmwareInterface::writeBuffer() {
	const std::size_t avail = _lfQueue.read_available();
	if (avail == 0) {
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
	}
	else if (avail > 0 && avail < BATCH_SIZE) {
		if (_isBatching) {
			//	std::cout << "psst! I'm waiting for a batch of cookies!" << '\n';
			_writeTimer.expires_from_now(_writeInterval);
			_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
			return;
		}
		//	std::cout << "Okay, we need to cook a new batch\n";
		_isBatching = true;
		_batchingDeadline.expires_from_now(_batchingTimeout);
		_batchingDeadline.async_wait([&](const boost::system::error_code& ec) {
			if (!ec) {
				auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
				const int actualLen = _lfQueue.pop(*a, BATCH_SIZE);
				//	std::cout << "had to send a mini batch of " << actualLen << " cookies\n";

				this->_adapter->Write(a, actualLen, [&](const boost::system::error_code& e, std::size_t bytes_t) {

				}
				);
				_writeTimer.expires_from_now(_writeInterval);
				_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
				_isBatching = false;
			}
		});
		//	std::cout << "Some avail, waiting" << '\n';
	}
	else {
		_isBatching = false;
		_batchingDeadline.cancel();
		auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
		const int actualLen = _lfQueue.pop(*a, BATCH_SIZE);
		_adapter->Write(a, actualLen, [&](const boost::system::error_code& e, std::size_t bytes_t) {


		}
		);
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
		//	std::cout << "Got a FULL BATCH!" << '\n';
	}

}
void FirmwareInterface::PlayEffect(Location location, std::string effectString, float strength) {
	
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		std::cout << "Failed to find atom in instruction set: " << effectString << '\n';
		return;
	}

	auto effect  = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
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

void FirmwareInterface::PlayEffectContinuous(Location location, std::string effectString, float strength)
{
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		std::cout << "Failed to find atom in instruction set: " << effectString << '\n';
		return;
	}

	auto effect = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
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
	_lfQueue.push(packet.Data, packet.Data + packet.Length);

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
