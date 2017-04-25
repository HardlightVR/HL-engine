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

boost::optional<TrackingUpdate> ClientMessenger::ReadTracking()
{
	if (m_trackingData) {
		return m_trackingData->Read();
	}
	return boost::optional<TrackingUpdate>();
}

boost::optional<SuitsConnectionInfo> ClientMessenger::ReadSuits()
{
	if (m_suitConnectionInfo) {
		return m_suitConnectionInfo->Read();
	}

	return boost::optional<SuitsConnectionInfo>();
}

void ClientMessenger::WriteHaptics(ExecutionCommand e)
{
	m_encoder.AquireEncodingLock();
	auto encoded = m_encoder.Encode(e);
	m_encoder.ReleaseEncodingLock();

	m_encoder._finalize(encoded, [this](void* data, int size) {
		if (m_hapticsStream) {
			try {
				m_hapticsStream->Push(data, size);
			}
			catch (const boost::interprocess::interprocess_exception& ec) {
				//probably full queue, which means the server isn't reading fast enough!
				//should log
			}
		}
	});
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
		m_trackingData = std::make_unique<ReadableSharedObject<TrackingUpdate>>("ns-tracking-data");
		m_suitConnectionInfo = std::make_unique<ReadableSharedObject<SuitsConnectionInfo>>("ns-suit-data");
		try {
			m_logStream = std::make_unique<ReadableSharedQueue>("ns-logging-data");
		}
		catch (const boost::interprocess::interprocess_exception& ec) {
			//we don't care if we can't instantiate the logger.
			//The engine may not create it if it is not in debug mode
		}

		//Everything setup successfully? Monitor the connection!
		startMonitorConnection();

	}
	catch (const boost::interprocess::interprocess_exception& ec) {
		Locator::Logger().Log("ClientMessenger", "Failed to create all the other shared objects", LogLevel::Error);

		//somehow failed to make these shared objects.
		//for now, until we know what types of errors these are, try again
		startAttemptEstablishConnection();
		return;
	}


	
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


