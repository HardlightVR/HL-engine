#include "stdafx.h"
#include "MyTestLog.h"
#include <boost\log\expressions.hpp>



MyTestLog::MyTestLog(DriverMessenger & messenger):
	m_messenger(&messenger)
{
}

MyTestLog::~MyTestLog()
{
}

void MyTestLog::consume(const boost::log::record_view& msg)
{
	if (m_messenger != nullptr) {
		m_messenger->WriteLog(*msg[boost::log::expressions::smessage]);
	}
}

void MyTestLog::ProvideMessenger(DriverMessenger & messenger)
{
	m_messenger = &messenger;
}
