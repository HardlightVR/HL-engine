#pragma once

class IoService
{
public:
	//Constructs and Start's a new IoService
	IoService();
	~IoService();

	//Return the underlying io_service object
	boost::asio::io_service& GetIOService();

	//Returns true if the service was started
	//Calling while it is running is undefined
	bool Start();

	//Tell it to stop, but it is not necessarily stopped by the end of the call
	void Stop();

	//Restart the IO service. Necessary for serial port resetting
	void RestartIOService(std::function<void()> ioResetCallback);
private:
	bool _running;
	bool _dataReady;

	std::thread _ioLoop;
	boost::asio::io_service _io;
	std::unique_ptr<boost::asio::io_service::work> _work;

	std::function<void()> _resetIOCallback;

	boost::barrier _readyToResumeIO;
	std::thread _adapterResetLoop;

	boost::condition_variable _needToCheckAdapter;
	boost::mutex _needToCheckMut;
};

