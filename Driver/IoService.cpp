#include "StdAfx.h"
#include "IoService.h"
#include <iostream>

IoService::IoService():_io(), _work(), _running(false), _readyToResumeIO(2)
{
	Start();
}

bool IoService::Start() {
	if (_running) {
		return false;
	}
	_running = true;
	
	_adapterResetLoop = std::thread([&]() {
		while (_running) {
			boost::unique_lock<boost::mutex> lock(_needToCheckMut);
			while (!_dataReady) {
				_needToCheckAdapter.wait(lock);
			}
			//We were signaled to restart the io_service
			_dataReady = false;
			_io.stop();

			//Before we let the callback do its thing, we need to wait for the 
			//io thread to finish resetting
			_readyToResumeIO.wait();
			_resetIOCallback();
		}
	});

	_work = std::make_unique<boost::asio::io_service::work>(_io);
	_ioLoop = std::thread([&]() {
		while (_running) {
			_io.run();
			_io.reset();
			//wait for the reset checker thread here 
			_readyToResumeIO.wait();
		} 
	});
	return true;
}
void IoService::Stop()
{
	_work.reset();
	_running = false;
}

IoService::~IoService()
{
	Stop();

	if (_ioLoop.joinable()) {
		_ioLoop.join();
	}

	if (_adapterResetLoop.joinable()) {
		_adapterResetLoop.join();
	}
	
}

boost::asio::io_service& IoService::GetIOService()
{
	return _io;
}

void IoService::RestartIOService(std::function<void()> ioResetCallback)
{
	{
		boost::lock_guard<boost::mutex> lock(_needToCheckMut);
		_dataReady = true;
	}
	_resetIOCallback = ioResetCallback;
	_needToCheckAdapter.notify_one();
}

