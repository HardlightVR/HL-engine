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
//	_pollTimer.expires_from_now(_pollInterval);
	//_pollTimer.async_wait(boost::bind(&Driver::_PollHandler, this, boost::asio::placeholders::error));


	return true;
}

bool Driver::Shutdown()
{

	_pollTimer.cancel();
	_io->Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}

void Driver::_UpdateLoop()
{


}

void Driver::_Update(void* data, std::size_t size)
{
	
	

}

void Driver::_PollHandler(const boost::system::error_code & ec)
{
	//_messenger.Poll(boost::bind(&Driver::_Update, this, _1, _2));
	_pollTimer.expires_from_now(_pollInterval);
	_pollTimer.async_wait(boost::bind(&Driver::_PollHandler, this, boost::asio::placeholders::error));
}
