#include "StdAfx.h"
#include "IoService.h"
#include <iostream>
#include "Locator.h"
IoService::IoService():_io(), _work(), _running(false), _readyToResumeIO(2),
_shouldQuit{false}, _wantsReset{false}, _isReset{false}
{
	start();
}

void IoService::start() {
	if (_running) {
		return;
	}
	_running = true;
	_adapterResetLoop = std::thread([&]() {

		for (;;) {
			try {
				auto& log = Locator::Logger();
				boost::unique_lock<boost::mutex> lock(_needToCheckMut); //unlocked at this point
				//log.Log("IoService-r", "Waiting on the conditions");
				//this thread will wakeup when either _shouldQuit or _wantsReset returns true, and aquire the lock
				_needWakeup.wait(lock, [&] { return _shouldQuit.load() || _wantsReset.load();  });
				if (!_shouldQuit.load()) {
					//	log.Log("IoService-r", "Was awoken to do work");
						//Okay, we have not quit, so it must be that we want a reset
						//First, reset  _wantsReset to it's default state: false
					_wantsReset.store(false);
					//Then, we unlock the mutex so other threads can modify any of the variables
					_io.stop();
					//	log.Log("IoService-r", "Now waiting on the reset condition..");
						//we will wakeup again once isReset becomes true
					_doneResettingIO.wait(lock, [&] {return _isReset.load(); });
					//	log.Log("IoService-r", "Okay, going to call the callback");
					_isReset.store(false); //set back to default state
					lock.unlock();
					_io.post(_resetIOCallback);
				}
				else {
					//we want to quit, so break the loop
					break;
				}
			}
			catch (boost::system::system_error& ec) {
				std::cout << "BIG ERROR\n";
			}
		}

	});
	
	_ioLoop = std::thread([&]() {
		//Keep running as long as we haven't signaled to quit, but this is only checked after .run returns.
		//.run will block until it is stopped
		auto& log = Locator::Logger();
		while (!_shouldQuit.load()) {
			try {
				_work = std::make_unique<boost::asio::io_service::work>(_io);

				log.Log("IoService-i", "Going to run");
				_io.run(); //wait here for a while
				log.Log("IoService-i", "Going to reset and notify");
				_io.reset(); //someone stopped us? Reset

				_isReset.store(true);
				_doneResettingIO.notify_one();
			}
			catch (boost::system::system_error& e) {
				std::cout <<"WTF\n";
			}

		}
	});
	
	
}
void IoService::Shutdown()
{
	{
		boost::lock_guard<boost::mutex> lock(_needToCheckMut);
		_shouldQuit.store(true);
	}
	_needWakeup.notify_one();
	_io.stop();

	if (_ioLoop.joinable()) {
		_ioLoop.join();
	}

	if (_adapterResetLoop.joinable()) {
		_adapterResetLoop.join();
	}
}

IoService::~IoService()
{
	/*
	Shutdown();

	if (_ioLoop.joinable()) {
		_ioLoop.join();
	}

	if (_adapterResetLoop.joinable()) {
		_adapterResetLoop.join();
	}
*/	
}

boost::asio::io_service& IoService::GetIOService()
{
	return _io;
}

void IoService::RestartIOService(std::function<void()> ioResetCallback)
{
//	Locator::Logger().Log ("IoService", "Someone requested a restart of IO service");
	_resetIOCallback = ioResetCallback;

	{
		boost::lock_guard<boost::mutex> lock(_needToCheckMut);
		_wantsReset.store(true);
	}
	_needWakeup.notify_one();
	//Locator::Logger().Log("IoService", "I notified it");

}

