#include <fstream>
// #include <iostream>
// #include <memory>
// #include <sstream>
#include <iostream>
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

  T read_file()
  {
    std::ifstream file(filename_);
    if (!file.is_open())
    {
      // handle error from io_util
    }

    std::cout << "opened file" << std::endl;
    std::string line;
    while (std::getline(file, line))
    {
      if (line.empty()) continue;
      std::cout << "reading line: " << line << std::endl;
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
  virtual T process() = 0;
};
}  // namespace File
}  // namespace IO
#endif
