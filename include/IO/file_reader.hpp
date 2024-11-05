#include <string>
#include "IO/io_util.hpp"
#include "IO/logic_source_file.hpp"

#ifndef FILE_READER_HPP_
#define FILE_READER_HPP_

namespace IO
{
class FileReader
{
 public:
  FileReader(std::string& filename)
  {
    switch (IOUtil::get_filetype(filename))
    {
      case IOUtil::FileType::blif:
        logic_reader_ = new BlifReader();
        break;
      case IOUtil::FileType::pla:
        break;
      default:
        break;
    }
  }

 private:
  LogicalFileReader logic_reader_;
};
}  // namespace IO
#endif
