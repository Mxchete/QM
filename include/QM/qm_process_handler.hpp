#ifndef QM_PROCESSOR_HPP_
#define QM_PROCESSOR_HPP_

#include <map>
#include <set>
#include "IO/logger.hpp"
#include "QM/minterm_and_dc_map.hpp"
#include "QM/prime_implicants.hpp"
#include "QM/types.hpp"

namespace QM
{
class QMProcessHandler
{
 public:
  QMProcessHandler(MintermDCMap& map, std::shared_ptr<IO::Logger> logger)
      : input_map_(map), logger_(logger)
  {
  }

  QM::MintermMap process();

 private:
  QM::MintermDCMap input_map_;
  std::shared_ptr<IO::Logger> logger_;

  PrimeImplicants generate_pi_table();
  QM::tabular_terms find_pi(QM::tabular_terms& current_table);
};
}  // namespace QM

#endif
