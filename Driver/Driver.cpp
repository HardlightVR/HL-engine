#include "stdafx.h"
#include "Driver.h"
#include "IoService.h"
Driver::Driver() :
	_io(new IoService()),
	_running(false),
	_pollTimer(_io->GetIOService()),
	_pollInterval(1),
	_hardware(_io)

{
}

Driver::~Driver()
{
}

bool Driver::StartThread()
{
	
	_running = true;
//	_workThread = std::thread(boost::bind(&Driver::_UpdateLoop, this));
	_pollTimer.expires_from_now(_pollInterval);
	_pollTimer.async_wait(boost::bind(&Driver::_PollHandler, this, boost::asio::placeholders::error));


	return true;
}

bool Driver::Shutdown()
{
	//_running = false;

	//if (_workThread.joinable()) {
	//	_workThread.join();
	//}
	//After this point, we have stopped doing updates, 
	//so it's safe to cancel the status timer and stop the Io service
	//suitStatusTimer.cancel();

	_io->Stop();

	//Send one last update 
	//_encoder.AquireEncodingLock();
	//_encoder.Finalize(_encoder.Encode(NullSpace::Communication::SuitStatus::SuitStatus_Disconnected),
	//	[&](uint8_t* data, int size) {
	//	Wire::sendTo(server_updates, data, size);
	//});
	//_encoder.ReleaseEncodingLock();
	return true;
}

void Driver::_UpdateLoop()
{
	while (_running) {
		_hardware.Update();
	}

}

void Driver::_Update(void* data, std::size_t size)
{
	
	std::cout << "Hello from poll handler!" << '\n';
		//We are paying the price of verifying the buffer, but we don't need to if we trust the client's ability
		//to encode messages correctly. 
		/*
		flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data), size);
		if (NullSpace::HapticFiles::VerifyHapticPacketBuffer(verifier)) {
			auto packet = NullSpace::HapticFiles::GetHapticPacket(data);
			switch (packet->packet_type()) {
			case NullSpace::HapticFiles::FileType::FileType_Sequence:
				engine.PlaySequence(*packet);
				break;
			
		}
		else {
			std::cout << "Bad packet" << "\n";
		}
		*/
	

}

void Driver::_PollHandler(const boost::system::error_code & ec)
{
	_messenger.Poll(boost::bind(&Driver::_Update, this, _1, _2));
	_pollTimer.expires_from_now(_pollInterval);
	_pollTimer.async_wait(boost::bind(&Driver::_PollHandler, this, boost::asio::placeholders::error));
}
