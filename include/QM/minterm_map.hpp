#include <cmath>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>

#ifndef QM_MINTERM_MAP_
#define QM_MINTERM_MAP_

namespace QM
{
class MintermMap
{
 public:
  MintermMap(std::vector<char>& input, char output) : input_(input), output_(output)
  {
  }

  inline bool fill_onset(std::vector<uint64_t> onset)
  {
    bool success = true;
    map_.reserve(onset.size());
    for (uint64_t term : onset)
    {
      success &= map_.emplace(term).second;
    }
    return success;
  }

  inline std::shared_ptr<std::unordered_set<uint64_t>> get()
  {
    return std::make_shared<std::unordered_set<uint64_t>>(map_);
  }

 private:
  uint64_t num_terms_;
  std::vector<char> input_;
  char output_;
  std::unordered_set<uint64_t> map_;
};
}  // namespace QM

#endif  // QM_LITERAL_MAP_grep -q -i avx2 /proc/cpuinfo && echo true
