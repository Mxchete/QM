#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "IO/io_util.hpp"
#include "IO/logger.hpp"

#ifndef FILE_READER_HPP_
#define FILE_READER_HPP_

namespace IO
{
namespace File
{
template <typename T>
class FileWriter
{
 public:
  virtual ~FileWriter() = default;

  void write_file(T& output)
  {
    std::string result = process(output);

    if (out_)
    {
      (*out_) << result;
    }
  }

 protected:
  explicit FileWriter(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : filename_(filename), logger_(std::move(logger))
  {
    filetype_ = IOUtil::get_filetype(filename_);
  }
  std::ostream* out_;
  std::string filename_;
  IOUtil::FileType filetype_;
  std::shared_ptr<IO::Logger> logger_;
  std::unique_ptr<std::ofstream> file_;
  uint64_t line_num_ = 0;

  virtual std::string process(T& raw_object) = 0;
};
}  // namespace File
}  // namespace IO

#endif
