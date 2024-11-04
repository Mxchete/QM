#include "IO/blif_reader.hpp"
#include <memory>
#include <optional>

bool IO::File::BlifReader::read_file(std::string &filename)
{
  return true;
}
std::shared_ptr<QM::MintermMap> IO::File::BlifReader::get_map()
{
  return std::make_shared<QM::MintermMap>(function_output_map_);
}
