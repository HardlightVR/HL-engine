#include "Engine.h"
#include "BoostSerialAdapter.h"
#include "Wire.h"
#include "EncodingOperations.h"
#include "IoService.h"
Engine::Engine(std::shared_ptr<IoService> io) :
	_instructionSet(std::make_shared<InstructionSet>()),
	_adapter(std::shared_ptr<ICommunicationAdapter>(
		new BoostSerialAdapter(io)
	)),
	_packetDispatcher(std::make_shared<PacketDispatcher>(_adapter->GetDataStream())),
	_streamSynchronizer(_adapter->GetDataStream(), _packetDispatcher),
	_executor(SuitHardwareInterface(_adapter, _instructionSet))

{
	//Pulls all instructions, effects, etc. from disk
	_instructionSet->LoadAll();

	if (!_adapter->Connect()) {
		std::cout << "Unable to connect to suit" << "\n";
	}
	else {
		std::cout << "Connected to suit" << "\n";

	}

	_adapter->BeginRead();

}


void Engine::PlaySequence(const NullSpace::HapticFiles::HapticPacket& packet)
{
	if (_hapticCache.ContainsSequence(packet.name()->str())) {
		_executor.Play(_hapticCache.GetSequence(packet.name()->str()));
	}
	else {
		auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::Sequence*>(packet.packet()));
		_hapticCache.AddSequence(packet.name()->str(), decoded);
		_executor.Play(decoded);
	}
}

void Engine::PlayPattern(const NullSpace::HapticFiles::HapticPacket& packet)
{
	if (_hapticCache.ContainsPattern(packet.name()->str())) {
		_executor.Play(_hapticCache.GetPattern(packet.name()->str()));
	}
	else {
		const NullSpace::HapticFiles::Pattern* packet_ptr = static_cast<const NullSpace::HapticFiles::Pattern*>(packet.packet());
		std::vector<HapticFrame> decoded = EncodingOperations::Decode(packet_ptr);
		_hapticCache.AddPattern(packet.name()->str(), decoded);
		_executor.Play(decoded);
	}
}

void Engine::PlayExperience(const NullSpace::HapticFiles::HapticPacket& packet)
{
	if (_hapticCache.ContainsExperience(packet.name()->str())) {
		_executor.Play(_hapticCache.GetExperience(packet.name()->str()));
	}
	else {
		auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::Experience*>(packet.packet()));
		_hapticCache.AddExperience(packet.name()->str(), decoded);
		_executor.Play(decoded);
	}
}

void Engine::PlayEffect(const NullSpace::HapticFiles::HapticPacket& packet)
{
	auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::HapticEffect*>(packet.packet()));
	_executor.Play(decoded);
}

void Engine::Update(float dt)
{
	_executor.Update(dt);

		if (_adapter->NeedsReset()) {
			_adapter->DoReset();
		}
	
	//_streamSynchronizer.TryReadPacket();

}

bool Engine::SuitConnected() const
{
	return _adapter->IsConnected();
}




Engine::~Engine()
{
}


