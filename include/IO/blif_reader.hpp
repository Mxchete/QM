#include <stdlib.h>
#include <string>
#include "QM/boolean_function.hpp"
#include "QM/literal_map.hpp"

namespace IO
{
namespace File
{

class BlifReader
{
 public:
  BlifReader(std::string &filename);
  ~BlifReader();
  QM::LiteralMap get_map();

 private:
  QM::LiteralMap function_output_map_;
  bool read_file(std::string &filename);
  QM::LiteralMap convert_to_literals(QM::BooleanFunction &func);
};

}  // namespace File
}  // namespace IO
