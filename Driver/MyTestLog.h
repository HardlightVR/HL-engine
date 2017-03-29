#pragma once
#include <boost\log\core.hpp>
#include <boost\log\sinks\basic_sink_backend.hpp>
#include <boost\log\sinks\frontend_requirements.hpp>
#include "DriverMessenger.h"

class MyTestLog : public boost::log::sinks::basic_sink_backend<
	boost::log::sinks::combine_requirements<
		boost::log::sinks::synchronized_feeding
	>::type>
{
public:
	explicit MyTestLog(DriverMessenger& messenger);
	MyTestLog() {}
	~MyTestLog();

	void consume(const boost::log::record_view& msg);

	void ProvideMessenger(DriverMessenger& messenger);
private:
	DriverMessenger* m_messenger;

};

