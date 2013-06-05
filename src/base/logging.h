/*******************************************************************************
 * Copyright (C) 2013 Liu Xun (my@liuxun.org)                                  *
 *                                                                             *
 * FastDCS may be copied only under the terms of the GNU General               *
 * Public License V3, which may be found in the FastDCS source kit.            *
 * Please visit the FastDCS Home Page http://www.FastDCS.com/ for more detail. *
 *******************************************************************************/
//
// Provide logging facilities that treat log messages by their
// severities.  If function |InitializeLogger| was invoked and was
// able to open files specified by the parameters, log messages of
// various severity will be written into corresponding files.
// Otherwise, all log messages will be written to stderr.
//
// Example:
/*
    int main() {
      InitializeLogger("/tmp/info.log", "/tmp/warn.log", "/tmp/erro.log");
      LOG(INFO)    << "An info message going into /tmp/info.log";
      LOG(WARNING) << "An warn message going into /tmp/warn.log";
      LOG(ERROR)   << "An erro message going into /tmp/erro.log";
      LOG(FATAL)   << "An fatal message going into /tmp/erro.log, "
                   << "and kills current process by a segmentation fault.";
      return 0;
    }
 */

#ifndef FASTDCS_BASE_LOGGING_H_
#define FASTDCS_BASE_LOGGING_H_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <errno.h>  // for errno

void InitializeLogger(const std::string& info_log_filename,
                      const std::string& warn_log_filename,
                      const std::string& erro_log_filename,
                      int64_t log_max_size = 10 * 1024 * 1024);

enum LogSeverity { INFO, WARNING, ERROR, FATAL };

class Logger {
  friend void InitializeLogger(const std::string& info_log_filename,
                               const std::string& warn_log_filename,
                               const std::string& erro_log_filename,
                               int64_t log_max_size);
public:
  Logger(LogSeverity s) : severity_(s) {}
  ~Logger();

  static std::ostream& GetStream(LogSeverity severity);
  static std::ostream& Start(LogSeverity severity,
                             const std::string& file,
                             int line,
                             const std::string& function);

private:
  static void TruncateLogFile(LogSeverity severity);

private:
  static std::string info_log_filename_;
  static std::string warn_log_filename_;
  static std::string erro_log_filename_;
  static int64_t log_max_size_;

  static std::ofstream info_log_file_;
  static std::ofstream warn_log_file_;
  static std::ofstream erro_log_file_;
  LogSeverity severity_;
};


// The basic mechanism of logging.{h,cc} is as follows:
//  - LOG(severity) defines a Logger instance, which records the severity.
//  - LOG(severity) then invokes Logger::Start(), which invokes Logger::Stream
//    to choose an output stream, outputs a message head into the stream and
//    flush.
//  - The std::ostream reference returned by LoggerStart() is then passed to
//    user-specific output operators (<<), which writes the log message body.
//  - When the Logger instance is destructed, the destructor appends flush.
//    If severity is FATAL, the destructor causes SEGFAULT and core dump.
//
// It is important to flush in Logger::Start() after outputing message
// head.  This is because that the time when the destructor is invoked
// depends on how/where the caller code defines the Logger instance.
// If the caller code crashes before the Logger instance is properly
// destructed, the destructor might not have the chance to append its
// flush flags.  Without flush in Logger::Start(), this may cause the
// lose of the last few messages.  However, given flush in Start(),
// program crashing between invocations to Logger::Start() and
// destructor only causes the lose of the last message body; while the
// message head will be there.
//
#define LOG(severity)                                                   \
  Logger(severity).Start(severity, __FILE__, __LINE__, __FUNCTION__)


#endif // FASTDCS_BASE_LOGGING_H_
