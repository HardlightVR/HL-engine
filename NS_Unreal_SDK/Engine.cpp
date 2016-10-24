#include "Engine.h"
#include "BoostSerialAdapter.h"


Engine::Engine(std::shared_ptr<boost::asio::io_service> io):
	_suitHardware(std::make_shared<SuitHardwareInterface>()),
	_adapter(std::make_shared<ICommunicationAdapter>(new BoostSerialAdapter(io))),
	_packetDispatcher(_adapter->GetDataStream()),
	_streamSynchronizer(_adapter->GetDataStream(), std::shared_ptr<PacketDispatcher>(&_packetDispatcher)),
	_executor(_suitHardware)

{
}


Engine::~Engine()
{
}
