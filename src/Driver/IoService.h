#pragma once

class IoService
{
public:
	//Constructs and Start's a new IoService
	IoService();
	~IoService();

	//Return the underlying io_service object
	boost::asio::io_service& GetIOService();


	//Tell it to shutdown
	void Shutdown();

	//Restart the IO service. Necessary for serial port resetting
	void RestartIOService(std::function<void()> ioResetCallback);
private:

	void start();
	bool _running;

	std::thread _ioLoop;
	boost::asio::io_service _io;
	std::unique_ptr<boost::asio::io_service::work> _work;

	std::function<void()> _resetIOCallback;

	boost::barrier _readyToResumeIO;
	std::thread _adapterResetLoop;

	boost::condition_variable _needToCheckAdapter;
	boost::mutex _needToCheckMut;

	boost::condition_variable _needWakeup, _doneResettingIO;
	std::atomic_bool _shouldQuit, _wantsReset, _isReset;

};

