#include "StdAfx.h"
#pragma once
#include "ByteQueue.h"

class ICommunicationAdapter {
public:
	virtual ~ICommunicationAdapter() = default;
	virtual bool Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Write(char bytes[]) = 0;
	virtual void Read() = 0;
	ByteQueue suitDataStream;
};