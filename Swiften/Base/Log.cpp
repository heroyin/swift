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
		spdlog::get("xmpp")->error(stream.str());
		//LOG(ERROR) << stream.str();
	else if(_severity==Severity::warning)
		spdlog::get("xmpp")->warn(stream.str());
		//LOG(WARNING) << stream.str();
	else if(_severity==Severity::info)
		spdlog::get("xmpp")->info(stream.str());
//		LOG(INFO) << stream.str();
	else if(_severity==Severity::debug)
		spdlog::get("xmpp")->debug(stream.str());
		//		LOG(DEBUG) << stream.str();
	else 
		spdlog::get("xmpp")->debug(stream.str());
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

class my_sink : public spdlog::sinks::sink
{
    void log(const spdlog::details::log_msg& msg) override
    {
        // Your code here.
        // details::log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
        // msg.formatted contains the formatted log.
        // msg.raw contains pre formatted log
#ifdef SWIFTEN_PLATFORM_WINDOWS
        OutputDebugStringA(msg.formatted.data());
#else
        fwrite(msg.formatted.data(), sizeof(char), msg.formatted.size(), stdout);
        flush();
#endif
    }

    void flush()
    {
#ifndef SWIFTEN_PLATFORM_WINDOWS
        fflush(stdout);
#endif
    }

};

void Log::initLogger(std::string logFile) {

		try
	{
		std::vector<spdlog::sink_ptr> sinks;
		//sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
		sinks.push_back(std::make_shared<my_sink>());
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024*1024*6, 5));
		auto combined_logger = std::make_shared<spdlog::logger>("xmpp", begin(sinks), end(sinks));
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
	spdlog::get("xmpp")->flush();
	spdlog::drop_all();
}

Log::Severity Log::getLogLevel() {
	return logLevel;
}

void Log::setLogLevel(Severity level) {
	logLevel = level;
}

}
