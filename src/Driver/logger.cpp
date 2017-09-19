#include "stdafx.h"
#include "logger.h"

BOOST_LOG_GLOBAL_LOGGER_INIT(clogger, my_logger)
{
	my_logger lg(boost::log::keywords::channel = "core");
	return lg;
}

