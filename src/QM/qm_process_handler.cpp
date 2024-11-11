#include "QM/qm_process_handler.hpp"
#include <iostream>
#include "QM/minterm_map.hpp"

QM::MintermMap QM::QMProcessHandler::process()
{
  std::cout << "QM::Process" << std::endl;
  for (auto term : input_map_.get_minterms().get())
  {
    std::cout << term << std::endl;
  }
  for (auto term : input_map_.get_dont_care_terms().get())
  {
    std::cout << term << std::endl;
  }

  return input_map_.get_minterms();
}
