#ifndef QM_MINTERM_DC_MAP_
#define QM_MINTERM_DC_MAP_

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "QM/minterm_map.hpp"

// simple container for storing minterms & dc terms
namespace QM
{
class MintermDCMap
{
 public:
  MintermDCMap(sMintermMap& minterms, sMintermMap& dc) : minterms_(minterms), dc_(dc)
  {
  }

  MintermDCMap()
  {
  }

  inline uint64_t input_size()
  {
    return minterms_->inputs().size();
  }

  inline sMintermMap get_minterms()
  {
    return minterms_;
  }

  inline sMintermMap get_dont_care_terms()
  {
    return dc_;
  }

  inline bool add_minterm(std::vector<uint64_t>& terms)
  {
    return minterms_->fill_onset(terms);
  }

  inline bool add_dc_term(std::vector<uint64_t>& terms)
  {
    return dc_->fill_onset(terms);
  }

  inline void add_inputs(const std::vector<std::string>& inputs)
  {
    minterms_->add_inputs(inputs);
    dc_->add_inputs(inputs);
  }

  inline void add_output(const std::string& output)
  {
    minterms_->set_output(output);
    dc_->set_output(output);
  }

 private:
  sMintermMap minterms_;
  sMintermMap dc_;
};

using sBooleanFunction = std::shared_ptr<MintermDCMap>;
}  // namespace QM

#endif  // QM_MINTERM_DC_MAP_
