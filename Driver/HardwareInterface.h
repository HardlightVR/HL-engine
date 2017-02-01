#pragma once
#include "ICommunicationAdapter.h"
#include <boost\thread\mutex.hpp>
class Synchronizer;
class IoService;
class HardwareInterface
{
public:
	HardwareInterface(std::shared_ptr<IoService> io);
	~HardwareInterface();
	void Update();

	boost::condition_variable& Cond() {
		return _needToCheckAdapter;
	}
	boost::mutex& Mut() {
		return _needToCheckMut;
	}

	bool& DataReady() {
		return _dataReady;
	}
private:
	std::unique_ptr<ICommunicationAdapter> _adapter;
	std::unique_ptr<Synchronizer> _synchronizer;

	boost::asio::deadline_timer _adapterResetCheckTimer;
	boost::posix_time::milliseconds _adapterResetCheckInterval;

	std::thread _adapterResetChecker;

	bool _running;

	boost::condition_variable _needToCheckAdapter;
	boost::mutex _needToCheckMut;
	bool _dataReady;
};

