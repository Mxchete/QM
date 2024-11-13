#include "QM/qm_process_handler.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include "QM/QMUtil.hpp"
#include "QM/minterm_map.hpp"
#include "QM/types.hpp"

QM::MintermMap QM::QMProcessHandler::process()
{
  logger_->trace("QMProcessHandler::Received the following map:");
  logger_->trace("Minterms:");
  for (auto term : input_map_.get_minterms().get())
  {
    logger_->trace(std::to_string(term.first));
  }
  logger_->trace("Dont Care:");
  for (auto term : input_map_.get_dont_care_terms().get())
  {
    logger_->trace(std::to_string(term.first));
  }

  QM::PrimeImplicants pi_table(QM::QMProcessHandler::generate_pi_table());

  return input_map_.get_minterms();
}

QM::PrimeImplicants QM::QMProcessHandler::generate_pi_table()
{
  logger_->trace("Creating table 1");
  // the int key will be the number of 1's in the minterm the vector will be of size
  // input_map_.input_size() and will contain the state (0, 1, dc) for each bit
  std::map<uint64_t, std::map<std::set<uint64_t>, QM::bin>> table;
  for (auto& term : input_map_.get_minterms().get())
  {
    auto int_val = term.first;
    auto& bin_rep = term.second;
    logger_->trace("For term: " + std::to_string(int_val));
    uint64_t size = input_map_.input_size();
    logger_->trace("Size is: " + std::to_string(size));
    uint64_t num_ones = QM::QMUtil::get_bin_ones(int_val);
    logger_->trace("Number of ones is: " + std::to_string(num_ones));
    logger_->trace("Generated minterm of size " + std::to_string(bin_rep.size()));
    std::set<uint64_t> combined_term_key;
    combined_term_key.emplace(int_val);
    table[num_ones].emplace(combined_term_key, bin_rep);
  }
  for (auto& term : input_map_.get_dont_care_terms().get())
  {
    auto int_val = term.first;
    auto& bin_rep = term.second;
    logger_->trace("For term: " + std::to_string(int_val));
    uint64_t size = input_map_.input_size();
    logger_->trace("Size is: " + std::to_string(size));
    uint64_t num_ones = QM::QMUtil::get_bin_ones(int_val);
    logger_->trace("Number of ones is: " + std::to_string(num_ones));
    logger_->trace("Generated minterm of size " + std::to_string(bin_rep.size()));
    std::set<uint64_t> combined_term_key;
    combined_term_key.emplace(int_val);
    table[num_ones].emplace(combined_term_key, bin_rep);
  }

  QM::QMProcessHandler::find_pi(table);
}

QM::tabular_terms QM::QMProcessHandler::find_pi(QM::tabular_terms& current_table)
{
  for (std::pair<uint64_t, std::map<std::set<uint64_t>, std::vector<QM::States>>> tmp :
       current_table)
  {
    for (auto& mterm : tmp.second)
    {
      std::string str;
      for (auto& state : mterm.second)
      {
        str += std::to_string(state) + " ";
      }
      logger_->trace(std::to_string(tmp.first) + ": " + str);
    }
  }
  for (std::pair<const uint64_t, std::map<std::set<uint64_t>, std::vector<QM::States>>>&
           terms_for_num_ones : current_table)
  {
    auto num_ones = terms_for_num_ones.first;
    auto next_num = num_ones + 1;
    auto& terms = terms_for_num_ones.second;
    if (current_table.find(next_num) == current_table.end())
    {
      continue;
    }
    for (std::pair<const std::set<uint64_t>, std::vector<QM::States>>& term : terms)
    {
    }
  }
}
