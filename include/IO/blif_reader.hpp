#include <stdlib.h>
#include <memory>
#include <string>
#include "QM/boolean_function.hpp"
#include "QM/literal_map.hpp"

#ifndef IO_BLIF_READER_HPP_
#define IO_BLIF_READER_HPP_

namespace IO
{
namespace File
{

class BlifReader
{
 public:
  BlifReader(bool graceful_err) : graceful_err_(graceful_err)
  {
  }
  ~BlifReader()
  {
  }
  bool read_file(std::string &filename);
  std::shared_ptr<QM::LiteralMap> get_map();

 private:
  bool graceful_err_ = false;
  QM::LiteralMap function_output_map_;
  void convert_to_literals(QM::BooleanFunction &func);
};

}  // namespace File
}  // namespace IO

#endif  // IO_BLIF_READER_HPP_
