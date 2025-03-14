#ifndef QM_TYPES_HPP_
#define QM_TYPES_HPP_

#include <cstdint>
#include <map>
#include <vector>

namespace QM
{
enum States
{
  zero,
  one,
  dc,
  invalid_state
};

using bin = std::vector<States>;
using combined_terms = std::vector<uint64_t>;
using dual_rep = std::pair<combined_terms, bin>;
using combined_list = std::vector<dual_rep>;
using dual_list = std::map<combined_terms, bin>;
using tabular_terms = std::map<uint64_t, combined_list>;
}  // namespace QM

#endif  // QM_TYPES_HPP_
