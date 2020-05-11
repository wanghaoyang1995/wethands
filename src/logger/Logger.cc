// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-11 19:09:17
// Description:

#include "src/logger/Logger.h"
#include <cstdio>
#include "src/thread/CurrentThread.h"
#include "src/utils/Timestamp.h"

namespace wethands {

void DefaultOutput(const char* msg, int len) {
  fwrite(msg, sizeof(char), len, stdout);
}

void DefaultFlush() {
  fflush(stdout);
}

Logger::LogLevel gLogLevel = Logger::LogLevel::INFO;
Logger::OutputFunc gOutput = DefaultOutput;
Logger::FlushFunc gFlush = DefaultFlush;

const char* logLevelName[Logger::LogLevel::NUM_OF_LOGLEVEL] = {
  "TRACE",
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};

}  // namespace wethands

using wethands::Logger;

Logger::Logger(const char* file, int line, LogLevel level, const char* func) {
  stream_ << Timestamp::Now().ToString(true);
  stream_ << CurrentThread::Tid();
  stream_ << logLevelName[0];

}

Logger::~Logger() {

}

Logger::LogLevel getLogLevel() { return wethands::gLogLevel; }
void Logger::setLogLevel(LogLevel level) { wethands::gLogLevel = level; }

void Logger::setOutputFunc(OutputFunc output) { gOutput = output; }
void Logger::setFlushFunc(FlushFunc flush) { gFlush = flush; }
