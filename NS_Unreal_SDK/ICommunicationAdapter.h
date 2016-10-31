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
	virtual void BeginRead() = 0;
	virtual bool IsConnected() const = 0;
	virtual bool NeedsReset() const { return false; };
	virtual void DoReset() {};
	virtual std::shared_ptr<Buffer> GetDataStream() = 0;
};