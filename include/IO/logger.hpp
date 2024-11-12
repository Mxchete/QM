#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

namespace IO
{

class Logger
{
 public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERR
  };

  Logger(LogLevel lvl = LogLevel::INFO) : out_(&std::cout), log_level_(lvl)
  {
  }

  Logger(const std::string& filename, LogLevel lvl = LogLevel::INFO) : log_level_(lvl)
  {
    file_ = std::make_unique<std::ofstream>(filename);
    if (!file_->is_open())
    {
      throw std::runtime_error("Could not open file for logging.");
    }
    out_ = file_.get();
  }

  ~Logger()
  {
    if (file_ && file_->is_open())
    {
      file_->close();
    }
  }

  inline void trace(const std::string& message)
  {
    if (log_level_ <= LogLevel::TRACE)
    {
      log("TRACE", message);
    }
  }

  inline void debug(const std::string& message)
  {
    if (log_level_ <= LogLevel::DEBUG)
    {
      log("DEBUG", message);
    }
  }

  inline void info(const std::string& message)
  {
    if (log_level_ <= LogLevel::INFO)
    {
      log("INFO", message);
    }
  }

  inline void warn(const std::string& message)
  {
    if (log_level_ <= LogLevel::WARN)
    {
      log("WARN", message);
    }
  }

  inline void error(const std::string& message)
  {
    if (log_level_ <= LogLevel::ERR)
    {
      log("ERROR", message);
    }
  }

 private:
  std::ostream* out_;
  std::unique_ptr<std::ofstream> file_;
  LogLevel log_level_;

  inline void log(const std::string& level, const std::string& message)
  {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Format the time as YYYY-MM-DD HH:MM:SS
    std::tm utc_time = *std::gmtime(&now_c);  // Convert to UTC time
    if (out_)
    {
      *out_ << "[" << level << "][" << std::put_time(&utc_time, "%Y-%m-%d %H:%M:%S") << "] "
            << message << std::endl;
    }
  }
};
}  // namespace IO

#endif
