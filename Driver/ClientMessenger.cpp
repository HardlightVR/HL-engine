#include "stdafx.h"
#include "ClientMessenger.h"
#include "Locator.h"

ClientMessenger::ClientMessenger(boost::asio::io_service& io):

	m_sentinalTimer(io),
	m_sentinalInterval(1000),
	m_sentinalTimeout(1000)
{
	//default state is that we are not connected to the driver
	startAttemptEstablishConnection();
}


ClientMessenger::~ClientMessenger()
{
}


void ClientMessenger::startAttemptEstablishConnection()
{
	m_sentinalTimer.expires_from_now(m_sentinalInterval);
	m_sentinalTimer.async_wait(boost::bind(&ClientMessenger::attemptEstablishConnection, this, boost::asio::placeholders::error));
}

void ClientMessenger::attemptEstablishConnection(const boost::system::error_code & ec)
{
	try {
		Locator::Logger().Log("ClientMessenger", "Attempting to create Sentinal Shared Object", LogLevel::Info);
		m_sentinal = std::make_unique<ReadableSharedObject<std::time_t>>("ns-sentinal");

	}
	catch (const boost::interprocess::interprocess_exception& ec) {
		Locator::Logger().Log("ClientMessenger", "Failed to create Sentinal Shared Object", LogLevel::Error);

		//the shared memory object doesn't exist yet? Try again
		startAttemptEstablishConnection();
		return;
	}


	//Once the sentinal has connected, we want to setup the other shared objects
	try {
		Locator::Logger().Log("ClientMessenger", "Attempting to create all the other shared objects");

		m_hapticsStream = std::make_unique<WritableSharedQueue>("ns-haptics-data");
		m_trackingData = std::make_unique<ReadableSharedObject<int>>("ns-tracking-data");
		m_suitConnectionInfo = std::make_unique<ReadableSharedObject<int>>("ns-suit-data");
	}
	catch (const boost::interprocess::interprocess_exception& ec) {
		Locator::Logger().Log("ClientMessenger", "Failed to create all the other shared objects", LogLevel::Error);

		//somehow failed to make these shared objects.
		//for now, until we know what types of errors these are, try again
		startAttemptEstablishConnection();
		return;
	}


	//Everything setup successfully? Monitor the connection!
	startMonitorConnection();
}

void ClientMessenger::startMonitorConnection()
{
	m_sentinalTimer.expires_from_now(m_sentinalInterval);
	m_sentinalTimer.async_wait(boost::bind(&ClientMessenger::monitorConnection, this, boost::asio::placeholders::error));
}

void ClientMessenger::monitorConnection(const boost::system::error_code & ec)
{
	if (!ec) {
		Locator::Logger().Log("ClientMessenger", "Reading the sentinal..");

		std::time_t lastDriverTimestamp = m_sentinal->Read();
		//assumes that the current time is >= the read time
		auto time = boost::chrono::duration_cast<boost::chrono::milliseconds>(
			boost::chrono::seconds(std::time(nullptr) - lastDriverTimestamp)
		);

		if (time <= m_sentinalTimeout) {
			//we are connected, so keep monitoring
			Locator::Logger().Log("ClientMessenger", "All good!");

			startMonitorConnection();
		}
		else {
			startAttemptEstablishConnection();
		}
	}
	else {
		Locator::Logger().Log("ClientMessenger", "Monitor connection was cancelled");
	}

}


