#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "IO/file_reader.hpp"
#include "QM/QMUtil.hpp"
#include "QM/minterm_and_dc_map.hpp"

#ifndef IO_BLIF_READER_HPP_
#define IO_BLIF_READER_HPP_

namespace IO
{
namespace File
{

class BlifReader : public FileReader<QM::MintermDCMap>
{
 public:
  explicit BlifReader(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : FileReader<QM::MintermDCMap>(filename, logger)
  {
  }

 protected:
  bool read_line(const std::string& line) override
  {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token[0] == '#')
    {
      return true;
    }

    if (token == ".inputs")
    {
      std::string input;
      while (iss >> input)
      {
        top_level_inputs.push_back(input);
        is_top_level_input[input] = true;
      }
    }
    if (token == ".outputs")
    {
      iss >> output_;
    }
    else if (token == ".names")
    {
      process_subcircuit(line);
    }
    else if (token == ".end")
    {
      return false;  // Signal to stop processing
    }

    return true;  // Continue processing
  }

  QM::MintermDCMap process() override
  {
    // return QM::MintermDCMap(expand(output_map_[output_]), expand(dc_map_[output_]));
  }

 private:
  std::vector<std::string> top_level_inputs;
  std::map<std::string, bool> is_top_level_input;
  std::map<std::string, QM::MintermMap> output_map_;
  std::map<std::string, QM::MintermMap> dc_map_;
  std::string current_token_;
  std::string output_;

  void process_subcircuit(const std::string& firstLine)
  {
    std::istringstream iss(firstLine);
    std::string token;
    iss >> token;  // Skip ".names"

    std::vector<std::string> inputs;
    while (iss >> token)
    {
      inputs.push_back(token);
    }
    std::string output = inputs.back();
    inputs.pop_back();

    // Parse minterms in the block
    std::string line;
    std::vector<QM::MintermMap> local_minterms;
    while (std::getline(iss, line) && !line.empty() && line[0] != '.')
    {
      std::istringstream mintermStream(line);
      std::string inputPattern, outputValue;
      mintermStream >> inputPattern >> outputValue;
      local_minterms.push_back({inputs, output, logger_});
    }

    // output_map_.emplace(output, local_minterms);
  }

  // std::vector<QM::MintermMap> expandAllMinterms()
  // {
  //   std::vector<QM::MintermMap> expandedMinterms;
  //   for (const auto& [output, minterms] : output_map_)
  //   {
  //     expandMinterms(output, expandedMinterms);
  //   }
  //   return expandedMinterms;
  // }

  // void expandMinterms(const std::string& output, std::vector<QM::MintermMap>& expandedMinterms)
  // {
  //   if (is_top_level_input[output]) return;
  //
  //   for (auto& minterm : output_map_[output])
  //   {
  //     if (usesOnlyCircuitInputs(minterm.inputs()))
  //     {
  //       expandedMinterms.push_back(minterm);
  //     }
  //     else
  //     {
  //       for (const auto& subMinterm : expandIntermediateMinterm(minterm))
  //       {
  //         expandedMinterms.push_back(subMinterm);
  //       }
  //     }
  //   }
  // }

  bool usesOnlyCircuitInputs(const std::vector<std::string>& inputs)
  {
    for (const auto& input : inputs)
    {
      if (!is_top_level_input[input])
      {
        return false;
      }
    }
    return true;
  }

  QM::MintermMap expand(QM::MintermMap& minterm)
  {
    std::vector<QM::MintermMap> sub_minterms;
    for (auto& input : minterm.inputs())
    {
      auto output = output_map_.find(input);
      if (output != output_map_.end())
      {
        sub_minterms.push_back(output->second);
      }
    }

    auto root_map = minterm;
    for (auto& sub_map : sub_minterms)
    {
      root_map = root_map + sub_map;
    }
    return root_map;
  }
};

}  // namespace File
}  // namespace IO

#endif  // IO_BLIF_READER_HPP_
