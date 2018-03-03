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
#include "Reader.h"
#include "Writer.h"
#include "PacketDispatcher.h"

#include "hardlight_device_version.h"


//This is the main interface that we expose to other components. 
//We have the ability to subscribe to certain packet types from the suit,
//and to enqueue a packet for delivery to the suit.

class HardwareIO {
public:
	virtual ~HardwareIO () {}

	virtual bool Ready() = 0;

	virtual void QueuePacket(uint8_t* data, size_t length) = 0;
	virtual void OnPacket(inst::Id, boost::signals2::signal<void(Packet)>::slot_type handler) = 0;
	virtual float GetUtilizationRatio() const = 0;
	virtual hardlight_device_version GetVersion() const = 0;

	//todo make pure virtual
	virtual synchronizer2::State GetSynchronizationState()  const = 0; 
};




template<
	typename IoTraits
> 

class IoBase : public HardwareIO {
public:

	using io_t			= typename IoTraits::io_t;
	using connector_t	= typename IoTraits::connector_t;
	using connection_t	= typename IoTraits::connection_t;

	using Writer = Writer<io_t, IoTraits::writer_packet_size>;
	using Reader = Reader<io_t, IoTraits::reader_packet_size>;

	constexpr static size_t incoming_capacity = 4096;
	constexpr static size_t outgoing_capacity = 4096 * 3;

	IoBase(std::unique_ptr<io_t> io, connection_t args)
	: incoming(incoming_capacity)
	, outgoing(outgoing_capacity)
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

	hardlight_device_version GetVersion() const override {
		return version;
	}

	float GetUtilizationRatio() const override {
		return outgoing.read_available() / (float)(outgoing_capacity);
	}

	bool Ready() override {
		//todo: eventually we could make this const, but right now I'm hijacking the behavior of the DeviceManager,
		//which polls the potential devices to see if they are Ready. This polling triggers us to spam the device with a "get_version"
		//packet. This behavior works because we simply want to get the version back as fast as possible, so that we can become ready.
		//A better technique might be to have an independent timer that sends the "get_version" packets, so we don't depend on DeviceManager's 
		//polling behavior. 

		auto version = inst::Build(inst::get_version());

		this->QueuePacket(version.data(), version.size());
		return connection_ready;
	}


	synchronizer2::State GetSynchronizationState() const override {
		return synchronizer.state();
	}
private:
	using PacketQueue = boost::lockfree::spsc_queue<uint8_t>;
	PacketQueue incoming;
	PacketQueue outgoing;

	std::unique_ptr<io_t> ioObject;

	Reader reader;
	Writer writer;

	connection_t args;

	connector_t connector;

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

