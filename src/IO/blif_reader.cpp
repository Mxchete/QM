#include "IO/blif_reader.hpp"
#include <memory>
#include <optional>

bool IO::File::BlifReader::read_file(std::string &filename)
{
  return true;
}
std::shared_ptr<QM::LiteralMap> IO::File::BlifReader::get_map()
{
  return std::make_shared<QM::LiteralMap>(function_output_map_);
}
