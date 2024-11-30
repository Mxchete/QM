#ifndef FILE_READER_FACTORY_HPP_
#define FILE_READER_FACTORY_HPP_

#include <memory>
#include "IO/blif_reader.hpp"
#include "IO/file_reader.hpp"
#include "IO/io_util.hpp"
#include "IO/logger.hpp"
#include "IO/pla_reader.hpp"

namespace IO
{
namespace File
{
template <typename T>
class FileReaderFactory
{
 public:
  static std::unique_ptr<FileReader<T>> create(const std::string& filename,
                                               std::shared_ptr<IO::Logger> logger)
  {
    IOUtil::FileType filetype = IOUtil::get_filetype(filename);
    switch (filetype)
    {
      case IOUtil::FileType::blif:
        // Note for implementation: blif does not need to be supported, only PLA
        logger->fatal("Blif filetypes are not supported!");
        IOUtil::error_handler(IOUtil::Error::bad_filetype);
        // return std::make_unique<BlifReader>(filename, logger);
        break;
      case IOUtil::FileType::pla:
        return std::make_unique<PlaReader>(filename, logger);
      default:
        IOUtil::error_handler(IOUtil::Error::bad_filetype);
        break;
    }
    return nullptr;
  }
};
}  // namespace File
}  // namespace IO

#endif  // FILE_READER_FACTORY_HPP_
