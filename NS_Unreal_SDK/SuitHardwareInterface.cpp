#include "SuitHardwareInterface.h"
#include <fstream>

#include <iostream>
#include <boost\thread.hpp>
SuitHardwareInterface::SuitHardwareInterface(std::shared_ptr<ICommunicationAdapter> a, std::shared_ptr<InstructionSet> iset, std::shared_ptr<boost::asio::io_service> io) :
	adapter(a),
	builder(iset),
	_io(io),
	_useDeferredWriting(false),
	_lfQueue(512),
	_writeTimer(*io, _writeInterval),
	_batchingDeadline(*io, _batchingTimeout)
{
	//_writeTimer.expires_from_now(_writeInterval);
	//_writeTimer.async_wait(boost::bind(&SuitHardwareInterface::writeBuffer, this));
	//_preWriteBuffer.reserve(512);
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
		chooseExecutionStrategy(builder.Build());

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
		chooseExecutionStrategy(builder.Build());
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
		chooseExecutionStrategy(builder.Build());
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
		chooseExecutionStrategy(builder.Build());
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

void SuitHardwareInterface::writeBuffer() {
	const std::size_t avail = _lfQueue.read_available();
	if (avail == 0) {
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&SuitHardwareInterface::writeBuffer, this));
		std::cout << "Nothing avail" << '\n';
	}
	else if (avail > 0 && avail < 64) {
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&SuitHardwareInterface::writeBuffer, this));

		_batchingDeadline.expires_from_now(_batchingTimeout);
		_batchingDeadline.async_wait([&](const boost::system::error_code& ec) {
			if (!ec) {
				std::cout << "had to send what I had" << '\n';
			}
		});
		std::cout << "Some avail, waiting" << '\n';
	}
	else {
		_batchingDeadline.cancel();
		//send batch!
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&SuitHardwareInterface::writeBuffer, this));
		std::cout << "Got a FULL BATCH!" << '\n';
	}
	
	
}
void SuitHardwareInterface::UseImmediateMode() {
	_useDeferredWriting = false;
}
void SuitHardwareInterface::UseDeferredMode() {
	_useDeferredWriting = true;
}




void SuitHardwareInterface::executeImmediately(Packet packet)
{
	//grab the bytes out of the packet, and copy them into a new shared pointer
	auto a = std::make_shared<uint8_t*>(new uint8_t[packet.Length]);
	memcpy(*a, packet.Data, packet.Length);
	this->adapter->Write(a, packet.Length);
}

void SuitHardwareInterface::executeLater(Packet packet)
{
	_lfQueue.push(packet.Data, packet.Data + packet.Length);
	//for (int i = 0; i < packet.Length; i++) {
	//	_preWriteBuffer.push(packet.Data[i]);
	//}
//	_preWriteBuffer.insert(_preWriteBuffer.end(), packet.Data, packet.Data + packet.Length);
	
	
}

void SuitHardwareInterface::chooseExecutionStrategy(Packet  packet)
{
	if (_useDeferredWriting) {
		this->executeLater(packet);
	}
	else {
		this->executeImmediately(packet);
	}
}
