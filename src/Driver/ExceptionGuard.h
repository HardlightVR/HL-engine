#pragma once
#include "logger.h"

#define CATCH_EXCEPTIONS

template<typename T> nsvr_result ExceptionGuard(T&& t) {
#ifdef CATCH_EXCEPTIONS
	try {
#endif
		return t();
#ifdef CATCH_EXCEPTIONS
	}
	catch (const std::exception& e) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_error) << 
		std::this_thread::get_id() <<
			"[Service] Exception: " << e.what();
		return nsvr_error_unknown;
	}
#endif
}