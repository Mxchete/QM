#ifndef FILE_WRITER_HPP_
#define FILE_WRITER_HPP_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include "IO/io_util.hpp"
#include "IO/logger.hpp"

namespace IO
{
namespace File
{
template <typename T>
class FileWriter
{
 public:
  virtual ~FileWriter()
  {
    if (file_ && file_->is_open())
    {
      file_->close();
    }
  }

  virtual void write_file(T& output)
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
    file_ = std::make_unique<std::ofstream>(filename);
    if (!file_->is_open())
    {
      throw std::runtime_error("Could not open file for logging.");
    }
    out_ = file_.get();
    filetype_ = IOUtil::get_filetype(filename_);
  }
  explicit FileWriter(std::shared_ptr<IO::Logger> logger)
      : out_(&std::cout), logger_(std::move(logger))
  {
  }
  std::ostream* out_;
  std::string filename_;
  IOUtil::FileType filetype_;
  std::shared_ptr<IO::Logger> logger_;
  std::unique_ptr<std::ofstream> file_;

  virtual std::string process(const T& raw_object) = 0;
};
}  // namespace File
}  // namespace IO

#endif  // FILE_WRITER_HPP_
