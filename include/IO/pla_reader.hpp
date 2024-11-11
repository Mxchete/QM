#include <stdlib.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include "IO/file_reader.hpp"
#include "IO/io_util.hpp"
#include "QM/QMUtil.hpp"
#include "QM/minterm_and_dc_map.hpp"
#include "QM/minterm_map.hpp"

namespace IO
{
namespace File
{
class PlaReader : public FileReader<QM::MintermDCMap>
{
 public:
  explicit PlaReader(const std::string& filename) : FileReader<QM::MintermDCMap>(filename)
  {
  }

 protected:
  bool read_line(const std::string& line) override
  {
    std::istringstream iss(line);
    std::string token;
    iss >> token;
    bool success = true;

    if (token[0] == '#')
    {
      return true;
    }

    if (token == ".i")
    {
      std::string input;
      iss >> input;
      num_inputs_ = std::stoi(input);
      std::cout << "num_inputs_: " << input << std::endl;
    }
    else if (token == ".ilb")
    {
      input_labels_provided_ = true;
      std::string input;
      iss >> input;
      while (iss >> input)
      {
        input_labels_.push_back(input);
      }
      minterm_dc_map_.add_inputs(input_labels_);
    }
    else if (token == ".ob")
    {
      output_labels_provided_ = true;
      std::string output;
      iss >> output;
      minterm_dc_map_.add_output(output);
    }
    else if (token == ".o")
    {
      std::string output;
      iss >> output;
      uint64_t num_output = std::stoi(output);
    }
    else if (token == ".p")
    {
      std::string val;
      iss >> val;
      num_product_terms_ = std::stoi(val);
    }
    else if (token == ".e")
    {
      return false;  // Signal to stop processing
    }
    else if (token != "")
    {
      if (num_inputs_ == 0)
      {
        IOUtil::error_handler(IOUtil::Error::malformed_file);
      }
      std::string term = token;
      std::string out;
      iss >> out;
      std::vector<uint64_t> processed_term_list = QM::QMUtil::termtoveci(term);
      QM::QMUtil::States which_map = QM::QMUtil::get_state(out[0]);
      if (which_map == QM::QMUtil::States::one)
      {
        minterm_dc_map_.add_minterm(processed_term_list);
      }
      else if (which_map == QM::QMUtil::States::dc)
      {
        minterm_dc_map_.add_dc_term(processed_term_list);
      }
      else
      {
        IOUtil::error_handler(IOUtil::Error::malformed_file);
      }
      received_ += 1;
    }

    return success;
  }

  QM::MintermDCMap process() override
  {
    if (!input_labels_provided_)
    {
      for (int i = 0; i < num_inputs_; i++)
      {
        input_labels_.push_back(std::to_string(i));
      }
      minterm_dc_map_.add_inputs(input_labels_);
    }
    if (!output_labels_provided_)
    {
      minterm_dc_map_.add_output("1");
    }
    return minterm_dc_map_;
  }

 private:
  std::vector<std::string> input_labels_;
  QM::MintermDCMap minterm_dc_map_;
  std::string current_token_;
  std::string output_;
  uint64_t num_product_terms_ = 0;
  uint64_t num_inputs_;
  uint64_t received_ = 0;
  bool input_labels_provided_ = false;
  bool output_labels_provided_ = false;

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
      local_minterms.push_back({inputs, output});
    }

    // output_map_.emplace(output, local_minterms);
  }
};
}  // namespace File
}  // namespace IO
