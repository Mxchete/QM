#ifndef QM_MINTERM_DC_MAP_
#define QM_MINTERM_DC_MAP_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>
#include "QM/QMUtil.hpp"
#include "QM/minterm_map.hpp"

namespace QM
{
class MintermDCMap
{
 public:
  MintermDCMap(MintermMap& minterms, MintermMap& dc) : minterms_(minterms), dc_(dc)
  {
  }

  MintermDCMap()
  {
  }

  uint64_t input_size()
  {
    return minterms_.inputs().size();
  }

  MintermMap get_minterms()
  {
    return minterms_;
  }

  MintermMap get_dont_care_terms()
  {
    return dc_;
  }

  bool add_minterm(std::vector<uint64_t>& terms)
  {
    return minterms_.fill_onset(terms);
  }

  bool add_dc_term(std::vector<uint64_t>& terms)
  {
    return dc_.fill_onset(terms);
  }

  void add_inputs(const std::vector<std::string>& inputs)
  {
    minterms_.add_inputs(inputs);
    dc_.add_inputs(inputs);
  }

  void add_output(const std::string& output)
  {
    minterms_.set_output(output);
    dc_.set_output(output);
  }

 private:
  MintermMap minterms_;
  MintermMap dc_;
};
}  // namespace QM

#endif
