#ifndef PLA_WRITER_HPP_
#define PLA_WRITER_HPP_

#include <stdlib.h>
#include <cstdint>
#include <memory>
#include <string>
#include "IO/file_writer.hpp"
#include "QM/minterm_and_dc_map.hpp"
#include "QM/minterm_map.hpp"

namespace IO
{
namespace File
{
class PlaWriter : public FileWriter<QM::sMintermMap>
{
 public:
  explicit PlaWriter(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : FileWriter<QM::sMintermMap>(filename, logger)
  {
  }

 protected:
  inline std::string process(const QM::sMintermMap raw_object) override
  {
  }

 private:
};
}  // namespace File
}  // namespace IO

#endif
