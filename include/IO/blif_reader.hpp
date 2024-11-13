#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "IO/file_reader.hpp"
#include "QM/QMUtil.hpp"
#include "QM/minterm_and_dc_map.hpp"

#ifndef IO_BLIF_READER_HPP_
#define IO_BLIF_READER_HPP_

namespace IO
{
namespace File
{

class BlifReader : public FileReader<QM::MintermDCMap>
{
 public:
  explicit BlifReader(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : FileReader<QM::MintermDCMap>(filename, logger)
  {
  }

 protected:
};

}  // namespace File
}  // namespace IO

#endif  // IO_BLIF_READER_HPP_
