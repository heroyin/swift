/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <Swiften/Base/DateTime.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Platform.h>

#include <cstdio>
#if defined(SWIFTEN_PLATFORM_WINDOWS)
#include <windows.h>
#endif

#include "spdlog/spdlog.h"


namespace Swift {

static Log::Severity logLevel = Log::warning;

Log::Log() {
}

Log::~Log() {
	// Using stdio for thread safety (POSIX file i/o calls are guaranteed to be atomic)
//	fprintf(stderr, "%s", stream.str().c_str());
	//fflush(stderr);
//	std::cout << stream.str();

#if defined(SWIFTEN_PLATFORM_WINDOWS)	
//	OutputDebugString(stream.str().c_str());
#endif	

	if(_severity==Severity::error)
		spdlog::get("rylogger")->error(stream.str());
		//LOG(ERROR) << stream.str();
	else if(_severity==Severity::warning)
		spdlog::get("rylogger")->warn(stream.str());
		//LOG(WARNING) << stream.str();
	else if(_severity==Severity::info)
		spdlog::get("rylogger")->info(stream.str());
//		LOG(INFO) << stream.str();
	else if(_severity==Severity::debug)
		spdlog::get("rylogger")->debug(stream.str());
		//		LOG(DEBUG) << stream.str();
	else 
		spdlog::get("rylogger")->debug(stream.str());
		//		LOG(DEBUG) << stream.str();

}

std::ostringstream& Log::getStream(
		Severity severity, 
		const std::string& severityString, 
		const std::string& file, 
		int line,
		const std::string& function) {
	_severity = severity;
	stream << file << ":" << line << " " << function << ": ";
	return stream;

//	el::base::Writer& writer = LOG(INFO);



//	writer << file << ":" << line << " " << function << ": ";

//	return writer;
}

void Log::initLogger(std::string logFile) {

		try
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024*1024*6, 5));
		auto combined_logger = std::make_shared<spdlog::logger>("rylogger", begin(sinks), end(sinks));
		spdlog::register_logger(combined_logger);

		spdlog::set_level(spdlog::level::debug);
		spdlog::set_async_mode(4096);
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
	}

}

void Log::uninitLogger() {
	spdlog::get("rylogger")->flush();
	spdlog::drop_all();
}

Log::Severity Log::getLogLevel() {
	return logLevel;
}

void Log::setLogLevel(Severity level) {
	logLevel = level;
}

}
