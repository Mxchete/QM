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

  static inline FileType get_filetype(std::string& filename)
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
};
}  // namespace IO
