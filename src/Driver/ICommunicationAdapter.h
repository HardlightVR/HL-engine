#include "StdAfx.h"
#pragma once
#include "Synchronizer.h"
#include <functional>
#include <boost/asio.hpp>
class ICommunicationAdapter {
public:
	virtual ~ICommunicationAdapter() = default;
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual bool IsConnected() const = 0;

	virtual void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length, std::function<void(const boost::system::error_code&, std::size_t)> cb) = 0;
	virtual void Write(std::shared_ptr<uint8_t*> bytes, std::size_t length) = 0;

	virtual std::shared_ptr<Buffer> GetDataStream() = 0;

};