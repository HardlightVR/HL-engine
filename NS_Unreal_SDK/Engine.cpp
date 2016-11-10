#include "StdAfx.h"
#include "Engine.h"
#include "BoostSerialAdapter.h"

#include "TrackingUpdate_generated.h"
#include "FifoConsumer.h"
#include "PlayableSequence.h"
Engine::Engine(std::shared_ptr<IoService> io, EncodingOperations& encoder, zmq::socket_t& socket) :
	_instructionSet(std::make_shared<InstructionSet>()),
	_adapter(std::shared_ptr<ICommunicationAdapter>(
		new BoostSerialAdapter(io)
		)),
	_packetDispatcher(std::make_shared<PacketDispatcher>()),
	_streamSynchronizer(_adapter->GetDataStream(), _packetDispatcher),
	_executor(std::make_unique<SuitHardwareInterface>(_adapter, _instructionSet, io->GetIOService())),
	_imuConsumer(std::make_shared<ImuConsumer>()),
	_trackingUpdateTimer(*io->GetIOService(), _trackingUpdateInterval),
	_encoder(encoder),
	_socket(socket),
	_userRequestsTracking(false)

{
	//Pulls all instructions, effects, etc. from disk
	if (!_instructionSet->LoadAll()) {
		std::cout << "Couldn't load configuration data, will exit after you hit [any key]" << '\n';
		std::cin.get();
		exit(0);
	}

	if (!_adapter->Connect()) {
		std::cout << "Unable to connect to suit" << "\n";
	}
	else {
		std::cout << "Connected to suit" << "\n";

	}
	_adapter->BeginRead();
	_packetDispatcher->AddConsumer(SuitPacket::PacketType::ImuData, _imuConsumer);
	_packetDispatcher->AddConsumer(SuitPacket::PacketType::FifoOverflow, std::make_shared<FifoConsumer>());
	_trackingUpdateTimer.async_wait(boost::bind(&Engine::sendTrackingUpdate, this));

}

void Engine::sendTrackingUpdate() {
	if (boost::optional<Quaternion> q = _imuConsumer->GetOrientation(Imu::Chest)) {
		_encoder.Finalize(_encoder.Encode(*q), [&](uint8_t* data, int size) {Wire::sendTo(_socket, data, size); });

	}
	_trackingUpdateTimer.expires_from_now(_trackingUpdateInterval);
	_trackingUpdateTimer.async_wait(boost::bind(&Engine::sendTrackingUpdate, this));
}

void Engine::PlaySequence(const NullSpace::HapticFiles::HapticPacket& packet)
{
	auto name = packet.name()->str();
	auto rawSequenceData = static_cast<const NullSpace::HapticFiles::Sequence*>(packet.packet());
	auto location = AreaFlag(rawSequenceData->location());
	auto handle = packet.handle(); //converts from uint64 to uint32, maybe should check this

	if (!_hapticCache.ContainsSequence(name)) {
		auto decoded = EncodingOperations::Decode(rawSequenceData);
		_hapticCache.AddSequence(name, decoded);
	}

	_executor.Create(handle, std::unique_ptr<IPlayable>(new PlayableSequence(_hapticCache.GetSequence(name), location)));
}

void Engine::PlayPattern(const NullSpace::HapticFiles::HapticPacket& packet)
{
	if (_hapticCache.ContainsPattern(packet.name()->str())) {
		//_executor.Play(_hapticCache.GetPattern(packet.name()->str()));
	}
	else {
		const NullSpace::HapticFiles::Pattern* packet_ptr = static_cast<const NullSpace::HapticFiles::Pattern*>(packet.packet());
		//std::vector<HapticFrame> decoded = EncodingOperations::Decode(packet_ptr);
		//_hapticCache.AddPattern(packet.name()->str(), decoded);
		//_executor.Play(decoded);
	}
}

void Engine::PlayExperience(const NullSpace::HapticFiles::HapticPacket& packet)
{
	if (_hapticCache.ContainsExperience(packet.name()->str())) {
		//	_executor.Play(_hapticCache.GetExperience(packet.name()->str()));
	}
	else {
		//auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::Experience*>(packet.packet()));
		//_hapticCache.AddExperience(packet.name()->str(), decoded);
		//_executor.Play(decoded);
	}
}

void Engine::PlayEffect(const NullSpace::HapticFiles::HapticPacket& packet)
{
	///auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::HapticEffect*>(packet.packet()));
	//_executor.Play(decoded);
}

void Engine::HandleCommand(const NullSpace::HapticFiles::HapticPacket & packet)
{
	auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::HandleCommand*>(packet.packet()));
	switch (decoded.Command) {
	case NullSpace::HapticFiles::Command_PAUSE:
		_executor.Pause(decoded.Handle);
		break;
	case NullSpace::HapticFiles::Command_PLAY:
		_executor.Play(decoded.Handle);
		break;
	case NullSpace::HapticFiles::Command_RESET:
		_executor.Reset(decoded.Handle);
		break;
	case NullSpace::HapticFiles::Command_RELEASE:
		_executor.Release(decoded.Handle);
	default:
		break;
	}
}

void Engine::EnableOrDisableTracking(const NullSpace::HapticFiles::HapticPacket & packet)
{
	_userRequestsTracking = EncodingOperations::Decode(static_cast<const NullSpace::HapticFiles::Tracking*>(packet.packet()));

	if (_userRequestsTracking) {
		_executor.Hardware()->EnableIMUs();
	}
	else {
		_executor.Hardware()->DisableIMUs();
	}
}

void Engine::Update(float dt)
{
	_executor.Update(dt);
	//todo: Raise event on disconnect and reconnect, so that engine can set the tracking to what the user requested
	if (_adapter->NeedsReset()) {
		_adapter->DoReset();
	}

	//read however many packets there are, with 500 as a hard maximum to keep the loop from
	//taking too much time
	int totalPackets = std::min(500, (int)_streamSynchronizer.PossiblePacketsAvailable());
	for (int i = 0; i < totalPackets; i++) {
		_streamSynchronizer.TryReadPacket();
	}


}

bool Engine::SuitConnected() const
{
	return _adapter->IsConnected();
}




Engine::~Engine()
{
}


