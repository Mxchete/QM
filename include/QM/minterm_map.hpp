#ifndef QM_MINTERM_MAP_
#define QM_MINTERM_MAP_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>
#include "QM/QMUtil.hpp"

namespace QM
{
class MintermMap
{
 public:
  MintermMap(std::vector<std::string>& input, std::string& output) : input_(input), output_(output)
  {
  }

  MintermMap()
  {
  }

  friend MintermMap operator+(MintermMap& mastermap, MintermMap& submap)
  {
    auto master_inputs = mastermap.inputs();
    auto sub_output_location =
        std::find(master_inputs.begin(), master_inputs.end(), submap.output());
    if (sub_output_location == master_inputs.end())
    {
      QMUtil::throw_error(QMUtil::Error::invalid_map_state);
    }
    auto new_size = master_inputs.size() + submap.inputs().size() - 1;
    std::vector<std::string> new_inputs;
    new_inputs.reserve(new_size);
    for (std::string& input : master_inputs)
    {
      if (input.compare(*sub_output_location) == 0)
      {
        for (std::string new_input : submap.inputs())
        {
          new_inputs.push_back(new_input);
        }
      }
      else
      {
        new_inputs.push_back(input);
      }
    }

    // clang doesn't like me adding this to the constructor directly
    std::string new_out = mastermap.output();
    QM::MintermMap combined_map(new_inputs, new_out);
    auto replace = std::distance(master_inputs.begin(), sub_output_location);

    // keeping for reference on converting and emplacing
    // uint64_t temp = 1024;
    // we want to add this at the end - 1 position
    // uint64_t add = 3;
    // std::bitset<64> temp_bits(temp);
    // std::bitset<64> add_bits(add);
    // auto temp_str = temp_bits.to_string();
    // std::cout << temp_str << " Size: " << temp_str.length() << std::endl;
    // temp_str = temp_str.substr(3);
    // auto add_str = add_bits.to_string();
    // add_str = add_str.substr(add_str.length() - 3);
    // std::cout << temp_str << " Size: " << temp_str.length() << std::endl;
    // std::cout << add_str << " Size: " << add_str.length() << std::endl;
    // auto temp_str_1 = temp_str.substr(0, temp_str.length() - 1);
    // auto temp_str_2 = temp_str.substr(temp_str.length() - 1);
    // auto new_str = temp_str_1 + add_str + temp_str_2;
    // std::cout << temp_str_1 << " Size: " << temp_str_1.length() << std::endl;
    // std::cout << temp_str_2 << " Size: " << temp_str_2.length() << std::endl;
    // std::cout << new_str << " Size: " << new_str.length() << std::endl;
    for (auto& onset : mastermap.get())
    {
      std::bitset<sizeof(onset)> onset_m_bits(onset);
      auto bitstring_m = onset_m_bits.to_string();
      // auto bit_substring_m = bitstring_m.substr(submap.inputs().size());
      // auto new_replace = replace - submap.inputs().size();
      // if we are replacing with the onset
      if (bitstring_m[replace] == '1')
      {
        for (auto& sub_onset : submap.get())
        {
          QMUtil::replace_bit_at(replace, onset, sub_onset, submap.inputs().size());
        }
      }
      // when we are replacing with the offset
      else
      {
        // seems kind of insane?
        for (uint64_t i = 0; i < std::pow(2, submap.inputs().size()); i++)
        {
          if (submap.get().find(i) != submap.get().end())
          {
            continue;
          }
          QMUtil::replace_bit_at(replace, onset, i, submap.inputs().size());
        }
      }
    }
  }

  void add_inputs(const std::vector<std::string>& inputs)
  {
    input_ = inputs;
  }

  void set_output(const std::string& output)
  {
    output_ = output;
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

  inline std::unordered_set<uint64_t> get()
  {
    return std::unordered_set<uint64_t>(map_);
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
  std::unordered_set<uint64_t> map_;
};
}  // namespace QM

#endif  // QM_LITERAL_MAP_
