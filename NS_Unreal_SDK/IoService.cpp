#include "StdAfx.h"
#include "IoService.h"
#include <iostream>

IoService::IoService():_io(std::make_shared<boost::asio::io_service>()), _work(std::make_unique<boost::asio::io_service::work>(*_io)), _thread()
{
	
}

bool IoService::Start() {
	if (_thread) { return false; }
	_work = std::make_unique<boost::asio::io_service::work>(*_io);
	_thread.reset(new std::thread(boost::bind(&boost::asio::io_service::run, _io.get())));
	return true;
}
bool IoService::Stop()
{
	if (!_thread) {
		return false;
	}
	_work.reset();
	_io->stop();
	_thread->join();
	_io->reset();
	_thread.reset();
	return true;
}

IoService::~IoService()
{
	
}

std::shared_ptr<boost::asio::io_service> IoService::GetIOService()
{
	return _io;
}

void IoService::RestartIOService()
{
	if (this->Stop()) {
		if (!this->Start()) {
			throw std::exception("wtf");
		}
	}
}
