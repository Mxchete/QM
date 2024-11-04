#include <string>
#include "IO/file_reader.hpp"
#include "IO/io_util.hpp"

namespace IO
{
class LogicalFileReader : FileReader
{
 public:
  LogicalFileReader(std::string filename)
  {
    switch (IOUtil::get_filetype(filename))
    {
      case IOUtil::FileType::blif:
        break;
      case IOUtil::FileType::pla:
        break;
      default:
        break;
    }
  }

 private:
};
}  // namespace IO
