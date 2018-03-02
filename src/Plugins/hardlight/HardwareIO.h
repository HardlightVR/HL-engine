#pragma once
#include <cstdint>
#include <memory>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio/serial_port.hpp>
#include "ReaderAdapter.h"
#include "WriterAdapter.h"
#include "SerialPortReader.h"
#include "SerialPortIO.h"
#include "Instructions.h"
#include <boost/signals2.hpp>
#include "suit_packet.h"
#include <boost/asio/ip/tcp.hpp>
#include "synchronizer2.h"
#include "SocketReader.h"
#include "PacketDispatcher.h"

#include "hardlight_device_version.h"
class HardwareIO {
public:
	virtual ~HardwareIO () {}

	virtual bool ready() = 0;

	//possibly do locking + overflow check in here?
	virtual void QueuePacket(uint8_t* data, size_t length) = 0;
	virtual void OnPacket(inst::Id, boost::signals2::signal<void(Packet)>::slot_type handler) = 0;
	virtual float GetUtilizationRatio() const = 0;
	virtual hardlight_device_version GetVersion() const = 0;

	//todo make pure virtual
	synchronizer2::State get_synchronization_state() const { 
		return synchronizer2::State::ConfirmingSync; 
	}
};


template<typename IoObject, typename ConnectionArgs>
class DefaultConnector {
public:
	DefaultConnector(IoObject&, ConnectionArgs) {}
	void try_connect(std::function<void(bool)>) {}

};

struct DefaultConnectionArgs {};


template<
	typename IoObject,
	typename Reader,
	typename Writer,
	typename ConnectionArgs = DefaultConnectionArgs,
	typename Connector = DefaultConnector<IoObject, ConnectionArgs>
> 

class IoBase : public HardwareIO {
public:
	IoBase(std::unique_ptr<IoObject> io, ConnectionArgs args)
	: incoming(4096)
	, outgoing(4096*3)
	, ioObject(std::move(io))
	, reader(incoming, *ioObject)
	, writer(outgoing, *ioObject)
	, args(args)
	, connector(*ioObject, args)
	, connection_ready(false)
	, dispatcher()
	, synchronizer(ioObject->get_io_service(), incoming)
	
	{
		synchronizer.on_packet([this](Packet p) {
			dispatcher.Dispatch(p);
		});

		connector.try_connect([this](bool success) {
		
			
			if (success) {
				reader.start();
				writer.start();
				synchronizer.start();
				std::vector<uint8_t> ping = { 0x24, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x0d, 0x0a };

				std::vector<uint8_t> click = { 0x24, 0x02, 0x13, 0x01, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x0d, 0x0a };
			
				this->OnPacket(inst::Id::GET_VERSION, [this](const Packet& version_packet) {
					version = parse_version(version_packet);
					connection_ready = true;
				});

				//auto version = inst::Build(inst::get_version());

			//	this->QueuePacket(version.data(), version.size());

			}
			
		});
	}

	inline hardlight_device_version GetVersion() const override {
		return version;
	}

	inline float GetUtilizationRatio() const override {
		return outgoing.read_available() / (float)(4096 * 3);

	}
	inline bool ready()  {
		auto version = inst::Build(inst::get_version());

		this->QueuePacket(version.data(), version.size());
		return connection_ready;
	}

private:
	using PacketQueue = boost::lockfree::spsc_queue<uint8_t>;
	PacketQueue incoming;
	PacketQueue outgoing;

	std::unique_ptr<IoObject> ioObject;

	Reader reader;
	Writer writer;

	ConnectionArgs args;

	Connector connector;

	bool connection_ready;
	

	PacketDispatcher dispatcher;
	synchronizer2 synchronizer;

	hardlight_device_version version;
	

	// Inherited via HardwareIO
	virtual void QueuePacket(uint8_t * data, size_t length) override
	{
		assert(length == 16);
		if (outgoing.write_available() >= length) {
			outgoing.push(data, length);
		}
	}

	virtual void OnPacket(inst::Id id, boost::signals2::signal<void(Packet)>::slot_type handler) override
	{
		dispatcher.AddConsumer(id, handler);
	}

};

