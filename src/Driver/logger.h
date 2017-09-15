#pragma once
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include "PluginAPI.h"

typedef boost::log::sources::severity_channel_logger_mt<nsvr_loglevel, std::string> my_logger;
BOOST_LOG_GLOBAL_LOGGER(sclogger, my_logger)
