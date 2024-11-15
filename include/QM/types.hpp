#ifndef QM_TYPES_HPP_
#define QM_TYPES_HPP_

#include <cstdint>
#include <map>
#include <set>
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
using tabular_terms = std::map<uint64_t, std::map<combined_terms, bin>>;
}  // namespace QM

#endif
