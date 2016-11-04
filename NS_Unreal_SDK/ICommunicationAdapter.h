#include "StdAfx.h"
#pragma once
#include "ByteQueue.h"
#include "Synchronizer.h"
#include <functional>
#include <boost/asio.hpp>
class ICommunicationAdapter {
public:
	virtual ~ICommunicationAdapter() = default;
	virtual bool Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, std::function<void(boost::system::error_code, std::size_t)> cb) = 0;
	virtual void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length) = 0;
	virtual void BeginRead() = 0;
	virtual bool IsConnected() const = 0;
	virtual bool NeedsReset() const { return false; };
	virtual void DoReset() {};
	virtual std::shared_ptr<Buffer> GetDataStream() = 0;
};