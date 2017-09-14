#include "StdAfx.h"
#include "IoService.h"
#include <iostream>
#include "Locator.h"
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
		auto& log = Locator::Logger();
		while (!m_shouldQuit.load()) {
			try {
				m_work = std::make_unique<boost::asio::io_service::work>(m_io);

				BOOST_LOG_TRIVIAL(info) << "[IoS] Starting";

				m_io.run(); //wait here for a while
				BOOST_LOG_TRIVIAL(info) << "[IoS] Going for a reset and notify";
				m_io.reset(); //someone stopped us? Reset

			}
			catch (boost::system::system_error&) {
				BOOST_LOG_TRIVIAL(info) << "[IoS] Failure in io loop";

			}
		}
	});
}

void IoService::Shutdown()
{

	BOOST_LOG_TRIVIAL(info) << "[IoS] Shutting down";		
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

