#ifndef QMUTIL_HPP_
#define QMUTIL_HPP_

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "QM/minterm_map.hpp"

namespace QM
{
class QMUtil
{
 public:
  enum States
  {
    zero,
    one,
    dc,
    invalid_state
  };

  States get_state(char b)
  {
    switch (b)
    {
      case '0':
        return States::zero;
      case '1':
        return States::one;
      case '-':
        return States::dc;
      default:
        return States::invalid_state;
    }
  }

  // add submap to its master
  QM::MintermMap combine(QM::MintermMap& mastermap, QM::MintermMap& submap)
  {
    auto master_inputs = mastermap.inputs();
    auto sub_output_location =
        std::find(master_inputs.begin(), master_inputs.end(), submap.output());
    if (sub_output_location == master_inputs.end())
    {
      throw_error(Error::invalid_map_state);
    }
    auto new_size = master_inputs.size() + submap.inputs().size() - 1;
    std::vector<std::string> new_inputs;
    new_inputs.reserve(new_size);
    for (std::string& input : master_inputs)
    {
      if (input.compare(*sub_output_location))
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

    QM::MintermMap combined_map(new_inputs, mastermap.output());
  }

 private:
  enum Error
  {
    invalid_map_state
  };
  static void throw_error(Error e)
  {
  }
};
}  // namespace QM

#endif  // QMUTIL_HPP_
