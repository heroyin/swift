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

#include <easylogging++.h>


INITIALIZE_EASYLOGGINGPP

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
		LOG(ERROR) << stream.str();
	else if(_severity==Severity::warning)
		LOG(WARNING) << stream.str();
	else if(_severity==Severity::info)
		LOG(INFO) << stream.str();
	else if(_severity==Severity::debug)
		LOG(DEBUG) << stream.str();
	else 
		LOG(DEBUG) << stream.str();

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

	el::Configurations* configurations = NULL;

	std::string confFile = "xmpplib.conf";
	if(boost::filesystem::exists(confFile)) {
		configurations = new el::Configurations(confFile);
	}
	else {
		configurations = new el::Configurations();
		configurations->setToDefault();
		configurations->set(el::Level::Global, 
			el::ConfigurationType::Format,  "%datetime [%level] %msg");
	}

	configurations->set(el::Level::Global, 
		el::ConfigurationType::Filename,  logFile);
	el::Loggers::reconfigureAllLoggers(*configurations);

	delete configurations;
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
}

Log::Severity Log::getLogLevel() {
	return logLevel;
}

void Log::setLogLevel(Severity level) {
	logLevel = level;
}

}
