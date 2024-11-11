#ifndef QM_PROCESSOR_HPP_
#define QM_PROCESSOR_HPP_

#include "QM/minterm_and_dc_map.hpp"

namespace QM
{
class QMProcessHandler
{
 public:
  QMProcessHandler(MintermDCMap& map) : input_map_(map)
  {
  }

  QM::MintermMap process();

 private:
  QM::MintermDCMap input_map_;
};
}  // namespace QM

#endif
