#include "stdafx.h"
#include "SuitDiagnostics.h"
#include "Consumers\SuitStatusConsumer.h"
#include "Consumers\FifoConsumer.h"
#include "Consumers\SuitInfoConsumer.h"

SuitDiagnostics::SuitDiagnostics() :
	_statusConsumer(std::make_unique<SuitStatusConsumer>(boost::bind(&SuitDiagnostics::ReceiveDiagnostics, this, _1))),
	_infoConsumer(std::make_unique<SuitInfoConsumer>(boost::bind(&SuitDiagnostics::ReceiveVersion, this, _1))),
	_fifoConsumer(std::make_unique<FifoConsumer>())
{
	
	
}


SuitDiagnostics::~SuitDiagnostics()
{
}

void SuitDiagnostics::ReceiveDiagnostics(const SuitDiagnosticInfo & info)
{
	//std::cout << "Got diagnostics!" << '\n';
	//std::cout << "Count = " << info.Count << '\n';
}

void SuitDiagnostics::ReceiveVersion(const VersionInfo & info)
{
	if (_callback) {
		_callback(info);
	}
}

void SuitDiagnostics::OnReceiveVersion(SuitVersionCallback cb)
{
	_callback = cb;
}


const std::unique_ptr<IPacketConsumer>& SuitDiagnostics::StatusConsumer()
{
	return _statusConsumer;
}


const std::unique_ptr<IPacketConsumer>& SuitDiagnostics::InfoConsumer()
{
	return _infoConsumer;
}

const std::unique_ptr<IPacketConsumer>& SuitDiagnostics::OverflowConsumer()
{
	return _fifoConsumer;
}

