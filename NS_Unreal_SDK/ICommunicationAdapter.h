#include "StdAfx.h"
#pragma once
#include "ByteQueue.h"

class ICommunicationAdapter {
public:
	virtual ~ICommunicationAdapter() = default;
	virtual bool Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Write(uint8_t bytes[], std::size_t length) = 0;
	virtual void Read() = 0;
	ByteQueue suitDataStream;
};