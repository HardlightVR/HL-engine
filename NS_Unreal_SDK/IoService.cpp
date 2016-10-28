#include "IoService.h"
#include <iostream>
#include "boost\thread.hpp"

IoService::IoService():_io(std::make_shared<boost::asio::io_service>()), _work(std::make_unique<boost::asio::io_service::work>(*_io)), _thread()
{
	
}

void IoService::Start() {
	if (_thread) { return; }
	_work = std::make_unique<boost::asio::io_service::work>(*_io);
	_thread.reset(new std::thread(boost::bind(&boost::asio::io_service::run, _io)));
}
void IoService::Stop()
{
	if (!_thread) {
		return;
	}
	_work.reset();
	_io->stop();
	_thread->join();
	_io->reset();
	_thread.reset();
}

IoService::~IoService()
{
	if (_thread && _thread->joinable()) {
		_io->stop();
		_thread->join();
		_thread.reset();
	}
}

std::shared_ptr<boost::asio::io_service> IoService::GetIOService()
{
	return _io;
}

void IoService::RestartIOService()
{
	this->Stop();
	this->Start();
}
