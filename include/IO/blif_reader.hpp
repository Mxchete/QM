#include <stdlib.h>
#include <memory>
#include <string>
// #include "QM/boolean_function.hpp"
#include "QM/minterm_map.hpp"

#ifndef IO_BLIF_READER_HPP_
#define IO_BLIF_READER_HPP_

namespace IO
{
namespace File
{

class BlifReader
{
 public:
  BlifReader()
  {
  }
  ~BlifReader()
  {
  }
  bool read_file(std::string &filename);
  std::shared_ptr<QM::MintermMap> get_map();

 private:
  QM::MintermMap function_output_map_;
  // void convert_to_literals(QM::BooleanFunction &func);
};

}  // namespace File
}  // namespace IO

#endif  // IO_BLIF_READER_HPP_
