#include "QM/qm_process_handler.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include "QM/QMUtil.hpp"
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
  logger_->trace("Creating table 1");
  // the int key will be the number of 1's in the minterm the vector will be of size
  // input_map_.input_size() and will contain the state (0, 1, dc) for each bit
  std::map<uint64_t, std::map<std::set<uint64_t>, std::vector<QMUtil::States>>> table;
  for (auto& term : input_map_.get_minterms().get())
  {
    logger_->trace("For term: " + std::to_string(term));
    uint64_t size = input_map_.input_size();
    logger_->trace("Size is: " + std::to_string(size));
    uint64_t num_ones = QM::QMUtil::get_bin_ones(term);
    logger_->trace("Number of ones is: " + std::to_string(num_ones));
    std::vector<QMUtil::States> minterm = QM::QMUtil::get_states(term, size);
    logger_->trace("Generated minterm of size " + std::to_string(minterm.size()));
    std::set<uint64_t> combined_term_key;
    combined_term_key.emplace(term);
    table[num_ones].emplace(combined_term_key, minterm);
  }
  for (auto& term : input_map_.get_dont_care_terms().get())
  {
    logger_->trace("For term: " + std::to_string(term));
    uint64_t size = input_map_.input_size();
    logger_->trace("Size is: " + std::to_string(size));
    uint64_t num_ones = QM::QMUtil::get_bin_ones(term);
    logger_->trace("Number of ones is: " + std::to_string(num_ones));
    std::vector<QMUtil::States> minterm = QM::QMUtil::get_states(term, size);
    logger_->trace("Generated minterm of size " + std::to_string(minterm.size()));
    std::set<uint64_t> combined_term_key;
    combined_term_key.emplace(term);
    table[num_ones].emplace(combined_term_key, minterm);
  }

  QM::QMProcessHandler::find_pi(table);
}

std::map<uint64_t, std::map<std::set<uint64_t>, std::vector<QM::QMUtil::States>>>
QM::QMProcessHandler::find_pi(
    std::map<uint64_t, std::map<std::set<uint64_t>, std::vector<QM::QMUtil::States>>>&
        current_table)
{
  for (std::pair<uint64_t, std::map<std::set<uint64_t>, std::vector<QMUtil::States>>> tmp :
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
  for (std::pair<const uint64_t, std::map<std::set<uint64_t>, std::vector<QMUtil::States>>>&
           terms_for_num_ones : current_table)
  {
    auto num_ones = terms_for_num_ones.first;
    auto next_num = num_ones + 1;
    auto& terms = terms_for_num_ones.second;
    if (current_table.find(next_num) == current_table.end())
    {
      continue;
    }
    for (std::pair<const std::set<uint64_t>, std::vector<QMUtil::States>>& term : terms)
    {
    }
  }
}
