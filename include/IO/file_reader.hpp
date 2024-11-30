#ifndef FILE_READER_HPP_
#define FILE_READER_HPP_

#include <fstream>
#include <memory>
#include <string>
#include "IO/io_util.hpp"
#include "IO/logger.hpp"

namespace IO
{
namespace File
{
template <typename T>
class FileReader
{
 public:
  virtual ~FileReader() = default;

  // generic function for reading file, specifies how file input should be read in for all file
  // readers
  T read_file()
  {
    std::ifstream file(filename_);
    if (!file.is_open())
    {
      // handle error from io_util
    }

    logger_->trace("FileReader::Opened File");
    std::string line;
    while (std::getline(file, line))
    {
      line_num_ += 1;
      logger_->trace("FileReader::Reading line " + std::to_string(line_num_) + ": " + line);
      if (line.empty()) continue;
      if (!read_line(line))
      {
        break;
      }
    }

    file.close();
    // Returns the processed data
    return process();
  }

 protected:
  // constructor to set variables
  explicit FileReader(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : filename_(filename), logger_(std::move(logger))
  {
    filetype_ = IOUtil::get_filetype(filename_);
  }
  std::string filename_;
  IOUtil::FileType filetype_;
  std::shared_ptr<IO::Logger> logger_;
  uint64_t line_num_ = 0;

  // virtual functions that should be overwritten for classes that inherit from this class
  virtual bool read_line(const std::string& line) = 0;
  virtual T process() = 0;
};
}  // namespace File
}  // namespace IO

#endif  // FILE_READER_HPP_
