#include "SuitHardwareInterface.h"
#include <fstream>

#include <iostream>
#include <boost\thread.hpp>
SuitHardwareInterface::SuitHardwareInterface(std::shared_ptr<ICommunicationAdapter> a, std::shared_ptr<InstructionSet> iset, std::shared_ptr<boost::asio::io_service> io):
	adapter(a),builder(iset), _io(io), _useDeferredWriting(false),  _needsFlush(false)
{
	
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

void SuitHardwareInterface::UseImmediateMode() {
	_useDeferredWriting = false;
}
void SuitHardwareInterface::UseDeferredMode() {
	_useDeferredWriting = true;
}
void SuitHardwareInterface::Flush()
{

	if (_preWriteBuffer.size() > 0) {
		const int size = _preWriteBuffer.size();
		std::cout << "DEFERRED writing " << size << " bytes" << '\n';

		//write the entire buffer in one call
		auto a = std::make_shared<uint8_t*>(new uint8_t[size]);
		for (int i = 0; i < size; ++i) {
			(*a)[i] = _preWriteBuffer.front();
			_preWriteBuffer.pop();
		}
		this->adapter->Write(a, size);

	}
	
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
	for (int i = 0; i < packet.Length; i++) {
		_preWriteBuffer.push(packet.Data[i]);
	}
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
