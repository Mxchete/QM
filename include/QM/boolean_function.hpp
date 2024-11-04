#include <functional>
#include <memory>
#include <vector>
#include "QM/literal_map.hpp"

namespace QM
{
class BooleanFunction
{
 public:
  std::shared_ptr<LiteralMap> get_func_minterms();
  std::shared_ptr<BooleanFunction> raw_logic_to_function(std::vector<LiteralMap>& logic_literals);

 private:
  std::vector<char> variables;
  std::shared_ptr<BooleanFunction> combine_subfunctions(std::vector<BooleanFunction>& subfunc);
};
}  // namespace QM
