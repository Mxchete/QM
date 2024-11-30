#include "IO/pla_reader.hpp"
#include <string>
#include "IO/io_util.hpp"

// implementation of read_line for pla reader
bool IO::File::PlaReader::read_line(const std::string& line)
{
  // turn line into stream
  std::istringstream iss(line);
  std::string token;
  // get first word
  iss >> token;
  bool success = true;

  // if first char of first word is '#', consider the line as a comment
  if (token[0] == '#')
  {
    return true;
  }

  // get number of inputs
  if (token == ".i")
  {
    std::string input;
    iss >> input;
    num_inputs_ = std::stoi(input);
    logger_->trace("PLAReader::num_inputs: " + std::to_string(num_inputs_));
  }
  // get names of inputs
  else if (token == ".ilb")
  {
    input_labels_provided_ = true;
    std::string input;
    uint64_t size = 0;
    while (iss >> input)
    {
      input_labels_.push_back(input);
      size += 1;
    }
    if (num_inputs_ == 0)
    {
      num_inputs_ = size;
    }
    logger_->trace("PLAReader::Received inputs of size: " + std::to_string(input_labels_.size()));
    minterm_dc_map_->add_inputs(input_labels_);
  }
  // get name of output
  else if (token == ".ob")
  {
    output_labels_provided_ = true;
    std::string output;
    iss >> output;
    minterm_dc_map_->add_output(output);
  }
  // get number of outputs (should be 1)
  else if (token == ".o")
  {
    std::string output;
    iss >> output;
    uint64_t num_output = std::stoi(output);
  }
  // get number of minterms
  else if (token == ".p")
  {
    std::string val;
    iss >> val;
    if (!input_labels_provided_)
    {
      for (int i = 0; i < num_inputs_; i++)
      {
        input_labels_.push_back(std::to_string(i));
      }
      minterm_dc_map_->add_inputs(input_labels_);
    }
    if (!output_labels_provided_)
    {
      minterm_dc_map_->add_output("1");
    }
    num_product_terms_ = std::stoi(val);
  }
  // we have finished reading in if we read '.e'
  else if (token == ".e")
  {
    return false;
  }
  // otherwise, if line is not blank, we assume it is a term
  else if (token != "")
  {
    if (num_inputs_ == 0)
    {
      IOUtil::error_handler(IOUtil::Error::malformed_file);
    }
    // term & whether it produces 1 or dc
    std::string term = token;
    if (term.size() != num_inputs_)
    {
      logger_->fatal("Received term with: " + std::to_string(term.size()) +
                     " literals. Expected Size: " + std::to_string(num_inputs_));
      logger_->fatal("Term: " + term);
      IOUtil::error_handler(IOUtil::Error::malformed_file);
    }
    std::string out;
    iss >> out;
    // get expanded term (remove dc)
    std::vector<uint64_t> processed_term_list = QM::QMUtil::termtoveci(term);
    // get whether output of term is 1 or dc
    QM::States which_map = QM::QMUtil::get_state(out[0]);
    // add to either minterm or dc term maps based on output
    if (which_map == QM::States::one)
    {
      minterm_dc_map_->add_minterm(processed_term_list);
    }
    else if (which_map == QM::States::dc)
    {
      minterm_dc_map_->add_dc_term(processed_term_list);
    }
    // anything else produces error
    else
    {
      IOUtil::error_handler(IOUtil::Error::malformed_file);
    }
    received_ += 1;
  }

  return success;
}
