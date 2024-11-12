#include "QM/qm_process_handler.hpp"
#include <iostream>
#include "QM/minterm_map.hpp"

QM::MintermMap QM::QMProcessHandler::process()
{
  logger_->trace("QMProcessHandler::Received the following map:");
  logger_->trace("Minterms:");
  for (auto term : input_map_.get_minterms().get())
  {
    logger_->trace(std::to_string(term));
  }
  logger_->trace("Dont Care:");
  for (auto term : input_map_.get_dont_care_terms().get())
  {
    logger_->trace(std::to_string(term));
  }

  QM::PrimeImplicants pi_table(QM::QMProcessHandler::generate_pi_table());

  return input_map_.get_minterms();
}

QM::PrimeImplicants QM::QMProcessHandler::generate_pi_table()
{
}
