#include "StdAfx.h"
#include "IoService.h"
#include <iostream>
#include "logger.h"
IoService::IoService()
	: m_io()
	, m_work()
	, m_shouldQuit{false}
{
	start();
}

void IoService::start() {
	m_ioLoop = std::thread([&]() {
		//Keep running as long as we haven't signaled to quit, but this is only checked after .run returns.
		//.run will block until it is stopped
		while (!m_shouldQuit.load()) {
			try {
				m_work = std::make_unique<boost::asio::io_service::work>(m_io);

				LOG_TRACE() << "Starting IO Service";

				m_io.run(); //wait here for a while
				LOG_TRACE() << "Resetting IO Service";
				m_io.reset(); //someone stopped us? Reset

			}
			catch (boost::system::system_error& ec) {
				LOG_ERROR() << "IO Service failure: " << ec.what();
			}
		}
	});
}

void IoService::Shutdown()
{

	LOG_TRACE() << "Shutting down IO Service";
	m_shouldQuit.store(true);
	
	m_io.stop();

	if (m_ioLoop.joinable()) {
		m_ioLoop.join();
	}

}

boost::asio::io_service& IoService::GetIOService()
{
	return m_io;
}

