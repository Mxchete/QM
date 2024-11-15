#include "IO/pla_reader.hpp"

bool IO::File::PlaReader::read_line(const std::string& line)
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
    logger_->trace("PLAReader::num_inputs: " + std::to_string(num_inputs_));
  }
  else if (token == ".ilb")
  {
    input_labels_provided_ = true;
    std::string input;
    while (iss >> input)
    {
      input_labels_.push_back(input);
    }
    logger_->trace("PLAReader::Received inputs of size: " + std::to_string(input_labels_.size()));
    minterm_dc_map_->add_inputs(input_labels_);
  }
  else if (token == ".ob")
  {
    output_labels_provided_ = true;
    std::string output;
    iss >> output;
    minterm_dc_map_->add_output(output);
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
  else if (token == ".e")
  {
    return false;
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
    QM::States which_map = QM::QMUtil::get_state(out[0]);
    if (which_map == QM::States::one)
    {
      minterm_dc_map_->add_minterm(processed_term_list);
    }
    else if (which_map == QM::States::dc)
    {
      minterm_dc_map_->add_dc_term(processed_term_list);
    }
    else
    {
      IOUtil::error_handler(IOUtil::Error::malformed_file);
    }
    received_ += 1;
  }

  return success;
}
