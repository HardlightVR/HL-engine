#include "stdafx.h"
#include "logging_initialization.h"

#include "PluginAPI.h"


#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>

#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_value_impl.hpp>
#include <boost/log/attributes/attribute_cast.hpp>

#include "logger.h"
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", nsvr_severity)
BOOST_LOG_ATTRIBUTE_KEYWORD(plugin_name, "Plugin", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(component, "Component", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

#define VERBOSE_CONSOLE_LOGGING

std::ostream& operator<< (std::ostream& strm, nsvr_severity level)
{
	static const char* strings[] =
	{
		"trace",
		"info",
		"warning",
		"error",
		"fatal"
	};

	if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
		strm << strings[level];
	else
		strm << static_cast< int >(level);

	return strm;
}


void initialize_logging() {

	boost::filesystem::path::imbue(std::locale("C"));

	namespace logging = boost::log;
	namespace sinks = boost::log::sinks;
	namespace src = boost::log::sources;
	namespace expr = boost::log::expressions;
	namespace attrs = boost::log::attributes;
	namespace keywords = boost::log::keywords;


	//We need two sinks. One sink writes everything to a file,
	//one sink writes only important things to the console. We also want a commmon formatter.

	//Here's the common formatter
	logging::formatter fmt = expr::stream
		<< std::setw(6) 
		<< expr::format_date_time<boost::posix_time::ptime >("TimeStamp", "[%Y-%m-%d, %H:%M:%S.%f]")
		<< expr::if_(expr::has_attr(severity))[
			expr::stream
			<< "<" 
			<< severity 
			<< "> " 
		]
		<< expr::if_(expr::has_attr(plugin_name))[
			expr::stream << "[Plugin = " << plugin_name << "]"
		]
		<< expr::if_(expr::has_attr(component))[
			expr::stream << "[Component = " << component << "]"
		]
		<< " " << expr::smessage;
		
	logging::core::get()->reset_filter();
	//Here's the console sink
	using text_sink = sinks::synchronous_sink<sinks::text_ostream_backend>;



	boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
	sink->locked_backend()->add_stream(stream);
	sink->set_formatter(fmt);

#ifdef VERBOSE_CONSOLE_LOGGING
	sink->set_filter(
		(severity >= nsvr_severity_trace && channel == "plugin") || 
		(severity >= nsvr_severity_info && channel == "core")
	);
#else 
	sink->set_filter(
		(severity >= nsvr_severity_warning && channel == "plugin") ||
		(severity >= nsvr_severity_info && channel == "core")
	);
#endif

	logging::core::get()->add_sink(sink);

	sink = boost::make_shared<text_sink>();

	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("hardlight_service.log"));
	sink->set_formatter(fmt);
	sink->locked_backend()->auto_flush(true);
	
	logging::core::get()->add_sink(sink);

	logging::add_common_attributes();

	logging::core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());

}