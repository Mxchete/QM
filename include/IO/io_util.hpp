#ifndef IO_UTIL_HPP_
#define IO_UTIL_HPP_

#include <iostream>
#include <string>

namespace IO
{
class IOUtil
{
 public:
  enum FileType
  {
    blif,
    pla,
    other
  };

  enum Error
  {
    bad_filetype,
    unexpected_eof,
    malformed_file
  };

  static inline FileType get_filetype(const std::string& filename)
  {
    std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if (ext == "blif")
    {
      return FileType::blif;
    }
    else if (ext == "pla")
    {
      return FileType::pla;
    }
    else
    {
      return FileType::other;
    }
  }

  static void error_handler(Error e)
  {
    std::cout << "IO_PLACEHOLDER_ERR";
  }
};
}  // namespace IO

#endif  // IO_UTIL_HPP_
