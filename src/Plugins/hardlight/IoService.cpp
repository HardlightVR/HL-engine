#include "StdAfx.h"
#include "IoService.h"
#include <iostream>
#include "Locator.h"
#include "logger.h"

//This may want to be refactored to match Plugin's version, which is simpler and does not have all the
//adapter resets. Not entirely sure if they are still necessary. Should test.


IoService::IoService():
	m_io(), 
	m_work(),
	_running(false), 
	_readyToResumeIO(2),
	_shouldQuit{false}, 
	_wantsReset{false},
	_isReset{false}
{
	start();
}

void IoService::start() {
	if (_running) {
		return;
	}
	_running = true;
	//_adapterResetLoop = std::thread([&]() {

	//	for (;;) {
	//		try {
	//			boost::unique_lock<boost::mutex> lock(_needToCheckMut); //unlocked at this point
	//			//log.Log("IoService-r", "Waiting on the conditions");
	//			//this thread will wakeup when either _shouldQuit or _wantsReset returns true, and acquire the lock
	//			_needWakeup.wait(lock, [&] { return _shouldQuit.load() || _wantsReset.load();  });
	//			if (!_shouldQuit.load()) {
	//				//	log.Log("IoService-r", "Was awoken to do work");
	//					//Okay, we have not quit, so it must be that we want a reset
	//					//First, reset  _wantsReset to it's default state: false
	//				_wantsReset.store(false);
	//				//Then, we unlock the mutex so other threads can modify any of the variables
	//				m_io.stop();
	//				//	log.Log("IoService-r", "Now waiting on the reset condition..");
	//					//we will wakeup again once isReset becomes true
	//				_doneResettingIO.wait(lock, [&] {return _isReset.load(); });
	//				//	log.Log("IoService-r", "Okay, going to call the callback");
	//				_isReset.store(false); //set back to default state
	//				lock.unlock();
	//				m_io.post(_resetIOCallback);
	//			}
	//			else {
	//				//we want to quit, so break the loop
	//				break;
	//			}
	//		}
	//		catch (boost::system::system_error& ec) {
	//			core_log(nsvr_loglevel_error, "IoService", ec.what());

	//		}
	//	}

	//});
	
	_ioLoop = std::thread([&]() {
		//Keep running as long as we haven't signaled to quit, but this is only checked after .run returns.
		//.run will block until it is stopped
		auto& log = Locator::Logger();
		while (!_shouldQuit.load()) {
			try {
				m_work = std::make_unique<boost::asio::io_service::work>(m_io);
				core_log("IoService", "Starting IO Service");

				m_io.run(); //wait here for a while
				core_log("IoService", "IO was reset");
				m_io.reset(); //someone stopped us? Reset

				_isReset.store(true);
				_doneResettingIO.notify_one();
			}
			catch (boost::system::system_error& ec) {
				core_log(nsvr_severity_error, "IoService", ec.what());

			}

		}
	});
	
	
}
void IoService::Shutdown()
{

	core_log("IoService", "Shutting down");

	{
		boost::lock_guard<boost::mutex> lock(_needToCheckMut);
		_shouldQuit.store(true);
	}
	_needWakeup.notify_one();
	m_io.stop();

	if (_ioLoop.joinable()) {
		_ioLoop.join();
	}

	//if (_adapterResetLoop.joinable()) {
	//	_adapterResetLoop.join();
	//}
}

IoService::~IoService()
{
	
}

boost::asio::io_service& IoService::GetIOService()
{
	return m_io;
}

