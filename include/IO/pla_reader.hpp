#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "IO/file_reader.hpp"
#include "QM/minterm_map.hpp"

namespace IO
{
namespace File
{
class PlaReader : FileReader<QM::MintermMap>
{
 public:
  explicit PlaReader(const std::string& filename) : FileReader<QM::MintermMap>(filename)
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

    if (token == ".i")
    {
      std::string input;
      while (iss >> input)
      {
        top_level_inputs.push_back(input);
        is_top_level_input[input] = true;
      }
    }
    if (token == ".o")
    {
      std::string output;
      iss >> output;
    }
    else if (token == ".p")
    {
      std::string val;
      iss >> val;
      num_product_terms_ = stoi(val);
    }
    else if (token == ".end")
    {
      return false;  // Signal to stop processing
    }

    return true;  // Continue processing
  }

  std::vector<QM::MintermMap> process() override
  {
    return expandAllMinterms();
  }

 private:
  std::vector<std::string> top_level_inputs;
  std::unordered_map<std::string, bool> is_top_level_input;
  std::unordered_map<std::string, std::vector<QM::MintermMap>> name_blocks;
  std::string current_token_;
  uint64_t num_product_terms_;

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
      local_minterms.push_back({inputs, inputPattern, output, outputValue});
    }

    name_blocks[output] = local_minterms;
  }

  std::vector<QM::MintermMap> expandAllMinterms()
  {
    std::vector<QM::MintermMap> expandedMinterms;
    for (const auto& [output, minterms] : name_blocks)
    {
      expandMinterms(output, expandedMinterms);
    }
    return expandedMinterms;
  }

  void expandMinterms(const std::string& output, std::vector<QM::MintermMap>& expandedMinterms)
  {
    if (is_top_level_input[output]) return;

    for (auto& minterm : name_blocks[output])
    {
      if (usesOnlyCircuitInputs(minterm.inputs()))
      {
        expandedMinterms.push_back(minterm);
      }
      else
      {
        for (const auto& subMinterm : expandIntermediateMinterm(minterm))
        {
          expandedMinterms.push_back(subMinterm);
        }
      }
    }
  }

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

  std::vector<QM::MintermMap> expandIntermediateMinterm(const QM::MintermMap& minterm)
  {
    std::vector<QM::MintermMap> expandedMinterms;
    // Recursive expansion logic here if needed
    return expandedMinterms;
  }
};
}  // namespace File
}  // namespace IO
