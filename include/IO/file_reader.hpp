#include <fstream>
// #include <iostream>
// #include <memory>
// #include <sstream>
#include <string>
#include <vector>
#include "IO/io_util.hpp"

#ifndef FILE_READER_HPP_
#define FILE_READER_HPP_

namespace IO
{
namespace File
{
template <typename T>
class FileReader
{
 public:
  virtual ~FileReader() = default;

  std::vector<T> read_file()
  {
    std::ifstream file(filename_);
    if (!file.is_open())
    {
      // handle error from io_util
    }

    std::string line;
    while (std::getline(file, line))
    {
      if (line.empty()) continue;
      if (!read_line(line))
      {
        break;
      }
    }

    file.close();
    return process();  // Returns the processed data
  }

 protected:
  explicit FileReader(const std::string& filename) : filename_(filename)
  {
    filetype_ = IOUtil::get_filetype(filename_);
  }
  std::string filename_;
  IOUtil::FileType filetype_;

  virtual bool read_line(const std::string& line) = 0;
  virtual std::vector<T> process() = 0;

 private:
};
}  // namespace File
}  // namespace IO
#endif
