#include "stdafx.h"
#include "Driver.h"
#include "IoService.h"
#include "ClientMessenger.h" 
#include "IntermediateHapticFormats.h"
#include "Encoder.h"
Driver::Driver() :
	_io(new IoService()),
	_running(false),
	m_hapticsPollTimer(_io->GetIOService()),
	m_hapticsPollInterval(10),
	_hardware(_io),
	_messenger(_io->GetIOService()),
	m_dispatcher()

{
}

Driver::~Driver()
{
	if (_workThread.joinable()) {
		_workThread.join();
	}
}

bool Driver::StartThread()
{
	_running = true;
	scheduleHapticsPoll();

	

	return true;
}

bool Driver::Shutdown()
{
	_running.store(false);
	m_hapticsPollTimer.cancel();
//	_messenger.Disconnect();
	
	_messenger.Disconnect();
	
	_io->Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void Driver::handleHaptics(const boost::system::error_code& ec)
{
	if (!ec) {
		if (auto command = _messenger.ReadHaptics()) {
			std::cout << "Got command!" << command->command() << '\n';
			_hardware.ReceiveExecutionCommand(*command);
		}
		scheduleHapticsPoll();
	}
	//Encoder encoder;

	//encoder.AquireEncodingLock();
//	auto offset = encoder.Encode(c);
	//encoder._finalize(offset, [&messenger, &encoder, this](uint8_t* data, int size) {
	//	messenger.Send(data, size);
		//encoder.ReleaseEncodingLock();

	//	_pollTimer.expires_from_now(_pollInterval);
	//	_pollTimer.async_wait(boost::bind(&Driver::_UpdateLoop, this));


	//});

}

void Driver::scheduleHapticsPoll()
{
	m_hapticsPollTimer.expires_from_now(m_hapticsPollInterval);
	m_hapticsPollTimer.async_wait(boost::bind(&Driver::handleHaptics, this, boost::asio::placeholders::error));
}


