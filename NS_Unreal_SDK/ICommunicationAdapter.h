#include "StdAfx.h"
#pragma once
#include "ByteQueue.h"
#include "Synchronizer.h"
class ICommunicationAdapter {
public:
	virtual ~ICommunicationAdapter() = default;
	virtual bool Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Write(uint8_t bytes[], std::size_t length) = 0;
	virtual void Read() = 0;
	virtual bool IsConnected() const;
	virtual std::shared_ptr<CircularBuffer> GetDataStream() = 0;
};