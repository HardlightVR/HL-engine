#include "stdafx.h"
#include "logger.h"


BOOST_LOG_GLOBAL_LOGGER_INIT(sclogger, my_logger)
{
	my_logger lg;
	return lg;
}