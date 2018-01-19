#include "stdafx.h"
#include "AsyncPacketRequest.h"
#include "synchronizer2.h"
#include "PacketDispatcher.h"
#include <cassert>
std::shared_ptr<AsyncPacketRequest> AsyncPacketRequest::make(boost::asio::io_service& io, PacketDispatcher* dispatcher, WritePacketCallback write, SuccessCallback s, TimeoutCallback t, boost::posix_time::time_duration d)
{

	auto ptr =  std::make_shared<AsyncPacketRequest>(io, s, t, d);
	
	ptr->wait(write(), dispatcher);
	return ptr;
}
AsyncPacketRequest::AsyncPacketRequest(boost::asio::io_service& io, SuccessCallback s, TimeoutCallback t, boost::posix_time::time_duration timeout)
	: success(s)
	, timeout(t)
	, timer(io, timeout)
{
}


void AsyncPacketRequest::wait(inst::Id::_enumerated id, PacketDispatcher* dispatcher) {


	auto self(shared_from_this());

	dispatcher->AddConsumer(id, [this, thing = std::weak_ptr<AsyncPacketRequest>(self)](Packet packet) {
		if (auto actual = thing.lock()) {
			this->packet = packet;
			this->timer.cancel();
		}
	});
	
	timer.async_wait([self, this] (boost::system::error_code ec) {
		if (ec == boost::asio::error::operation_aborted) { //cancelled, because success
			assert(packet);
			success(*packet);
		}
		else  { //timed out!
			timeout();
		}
	});
}