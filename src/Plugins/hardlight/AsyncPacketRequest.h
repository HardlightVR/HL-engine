#pragma once

#include "suit_packet.h"
#include <memory>
#include <functional>
#include "Instructions.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class synchronizer2;
class PacketDispatcher;
class AsyncPacketRequest : public std::enable_shared_from_this<AsyncPacketRequest>{

public: 
	using SuccessCallback = std::function<void(Packet)>;
	using TimeoutCallback = std::function<void()>;
	using WritePacketCallback = std::function<inst::Id::_enumerated()>;
	//if you use this constructor, you must manually call wait
	AsyncPacketRequest(boost::asio::io_service& io, SuccessCallback, TimeoutCallback, boost::posix_time::time_duration);
	void wait(inst::Id::_enumerated, PacketDispatcher* dispatcher);

	static std::shared_ptr<AsyncPacketRequest> make(boost::asio::io_service& io, PacketDispatcher* dispatcher, WritePacketCallback,  SuccessCallback, TimeoutCallback, boost::posix_time::time_duration);
private:
	SuccessCallback success;
	TimeoutCallback timeout;
	boost::asio::deadline_timer timer;
	boost::optional<Packet> packet;
};