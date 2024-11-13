#ifndef QM_MINTERM_MAP_
#define QM_MINTERM_MAP_

#include <algorithm>
#include <cmath>
#include <cstdint>
// #include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "IO/logger.hpp"
#include "QM/QMUtil.hpp"
#include "QM/types.hpp"

namespace QM
{
class MintermMap
{
 public:
  MintermMap(std::vector<std::string>& input,
             std::string& output,
             std::shared_ptr<IO::Logger> logger)
      : input_(input), output_(output), logger_(logger)
  {
  }

  MintermMap()
  {
  }

  inline void add_inputs(const std::vector<std::string>& inputs)
  {
    input_ = inputs;
  }

  inline void set_output(const std::string& output)
  {
    output_ = output;
  }

  inline bool fill_onset(std::vector<uint64_t> onset)
  {
    bool success = true;
    map_.reserve(onset.size());
    for (uint64_t term : onset)
    {
      QM::bin in_binary(QMUtil::get_bin_ones(term));
      success &= map_.emplace(term, in_binary).second;
    }
    return success;
  }

  inline std::unordered_map<uint64_t, QM::bin> get()
  {
    return std::unordered_map<uint64_t, QM::bin>(map_);
  }

  inline std::vector<std::string> inputs()
  {
    return input_;
  }

  inline std::string output()
  {
    return output_;
  }

 private:
  uint64_t num_terms_;
  std::vector<std::string> input_;
  std::string output_;
  std::unordered_map<uint64_t, QM::bin> map_;
  std::shared_ptr<IO::Logger> logger_;
};
}  // namespace QM

#endif  // QM_LITERAL_MAP_
