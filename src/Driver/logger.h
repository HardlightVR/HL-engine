#pragma once
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "PluginAPI.h"

typedef boost::log::sources::severity_channel_logger_mt<nsvr_severity, std::string> my_logger;

BOOST_LOG_GLOBAL_LOGGER(clogger, my_logger)


#define LOG_TRACE() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_severity_trace)
#define LOG_INFO() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_severity_info)
#define LOG_WARN() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_severity_warning)
#define LOG_ERROR() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_severity_error)
#define LOG_FATAL() BOOST_LOG_STREAM_SEV(clogger::get(), nsvr_severity_fatal)
