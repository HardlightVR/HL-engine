#pragma once
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "PluginAPI.h"

typedef boost::log::sources::severity_channel_logger_mt<nsvr_loglevel, std::string> my_logger;

BOOST_LOG_GLOBAL_LOGGER(clogger, my_logger)


#define LOG_TRACE() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_loglevel_trace)
#define LOG_INFO() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_loglevel_info)
#define LOG_WARN() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_loglevel_warning)
#define LOG_ERROR() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_loglevel_error)
#define LOG_FATAL() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_loglevel_fatal)
