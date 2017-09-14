#pragma once

class IoService
{
public:
	//Constructs and Start's a new IoService
	IoService();

	//Return the underlying io_service object
	boost::asio::io_service& GetIOService();


	//Tell it to shutdown
	void Shutdown();


private:

	void start();

	std::thread m_ioLoop;
	boost::asio::io_service m_io;
	std::unique_ptr<boost::asio::io_service::work> m_work;


	std::atomic_bool m_shouldQuit;

};

