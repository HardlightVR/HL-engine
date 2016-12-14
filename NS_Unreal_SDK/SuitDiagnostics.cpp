#include "stdafx.h"
#include "SuitDiagnostics.h"
#include "Consumers\SuitStatusConsumer.h"
#include "Consumers\FifoConsumer.h"
#include "Consumers\SuitInfoConsumer.h"

SuitDiagnostics::SuitDiagnostics() :
	_statusConsumer(std::make_shared<SuitStatusConsumer>(boost::bind(&SuitDiagnostics::ReceiveDiagnostics, this, _1))),
	_versionConsumer(std::make_shared<SuitInfoConsumer>(boost::bind(&SuitDiagnostics::ReceiveVersion, this, _1))),
	_fifoConsumer(std::make_shared<FifoConsumer>())
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
//	std::cout << "Got version!" << '\n';
//	std::cout << info.Major <<", " << info.Minor;
}

std::shared_ptr<SuitStatusConsumer> SuitDiagnostics::StatusConsumer()
{
	return _statusConsumer;
}


std::shared_ptr<SuitInfoConsumer> SuitDiagnostics::InfoConsumer()
{
	return _versionConsumer;
}

std::shared_ptr<FifoConsumer> SuitDiagnostics::OverflowConsumer()
{
	return _fifoConsumer;
}

void SuitDiagnostics::OnReceiveVersion(SuitVersionCallback cb)
{
	_callback = cb;
}

