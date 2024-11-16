#include "QM/qm_process_handler.hpp"
#include <algorithm>
#include <map>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include "QM/QMUtil.hpp"
#include "QM/minterm_map.hpp"
#include "QM/types.hpp"

QM::MintermMap QM::QMProcessHandler::process()
{
  logger_->trace("QMProcessHandler::Received the following map:");
  logger_->trace("Minterms:");
  for (auto term : input_map_.get_minterms()->get())
  {
    logger_->trace(std::to_string(term.first));
  }
  logger_->trace("Dont Care:");
  for (auto term : input_map_.get_dont_care_terms()->get())
  {
    logger_->trace(std::to_string(term.first));
  }

  QM::PrimeImplicants pi_table(QM::QMProcessHandler::generate_pi_table());

  // TODO: take essential PI table from generator & solve (using Petricks?)

  return *input_map_.get_minterms();
}

QM::PrimeImplicants QM::QMProcessHandler::generate_pi_table()
{
  logger_->trace("QMProcessHandler::Creating table 1");
  // tabular terms is, essentially, a map with uint64_t keys, representing the number of 1's in the
  // binary representation of the minterm, and values that are maps to sets of combined minterms
  // (keys) and their binary representation (vectors whose values are 0, 1, or dc)
  QM::tabular_terms table;
  add_terms_to_table(table, input_map_.get_minterms());
  add_terms_to_table(table, input_map_.get_dont_care_terms());

  QM::tabular_terms prime_implicants = QM::QMProcessHandler::find_pi(table);

  logger_->trace("found pi");
  for (auto& pi : prime_implicants)
  {
    logger_->trace("QMProcessHandler::For " + std::to_string(pi.first) + " ones");
    for (auto& imp : pi.second)
    {
      std::string str;
      for (auto& state : imp.second)
      {
        if (state == QM::States::dc)
        {
          str += '-';
          continue;
        }
        str += std::to_string(state) + "";
      }
      std::string cmb_term_str = "<";
      for (int i = 0; i < imp.first.size(); i++)
      {
        cmb_term_str += std::to_string(imp.first[i]) + ", ";
      }
      cmb_term_str += ">";
      logger_->trace(cmb_term_str + ": " + str);
    }
  }

  // TODO: create essential PI table from prime implicants, which will be returned to main process
  // function
  // QM::PrimeImplicants pi_table(prime_implicants);
  // return pi_table;
}

// this method should recursively iterate through each table needed & return the table with combined
// terms + terms it couldn't combine for the given iteration of the function
QM::tabular_terms QM::QMProcessHandler::find_pi(QM::tabular_terms& current_table)
{
  // this loop cant be removed, only for debugging to ensure that proper table is received
  for (std::pair<uint64_t, std::map<std::vector<uint64_t>, QM::bin>> tmp : current_table)
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
  std::vector<QM::dual_rep> used_terms;
  QM::tabular_terms new_table;
  std::vector<std::thread> processing_threads;
  // for each number of ones in the table
  for (std::pair<const uint64_t, std::map<std::vector<uint64_t>, QM::bin>>& terms_for_num_ones :
       current_table)
  {
    processing_threads.push_back(std::thread(&QM::QMProcessHandler::thread_processing, this,
                                             std::ref(terms_for_num_ones), std::ref(current_table),
                                             std::ref(used_terms), std::ref(new_table)));
  }

  for (auto& thread : processing_threads)
  {
    thread.join();
  }

  QM::tabular_terms finished_table;
  if (new_table.size() != 0)
  {
    finished_table = find_pi(new_table);
  }

  logger_->trace("Used terms size: " + std::to_string(used_terms.size()));
  for (std::pair<const uint64_t, std::map<std::vector<uint64_t>, QM::bin>>& terms_for_num_ones :
       current_table)
  {
    auto num_ones = terms_for_num_ones.first;
    auto& terms = terms_for_num_ones.second;
    // for each term in the given number of ones
    for (std::pair<const std::vector<uint64_t>, QM::bin>& comparison_term : terms)
    {
      // if comparison_term not in used_terms, add it to finished_table
      if (std::count_if(used_terms.begin(), used_terms.end(),
                        [&](const auto& e) { return e.second == comparison_term.second; }) == 0)
      {
        finished_table[num_ones].emplace(comparison_term);
      }
    }
  }

  return finished_table;
}

void QM::QMProcessHandler::add_terms_to_table(QM::tabular_terms& table, const QM::sMintermMap terms)
{
  for (auto& term : terms->get())
  {
    auto int_val = term.first;
    auto& bin_rep = term.second;
    logger_->trace("QMProcessHandler::For term: " + std::to_string(int_val));
    uint64_t size = input_map_.input_size();
    logger_->trace("QMProcessHandler::Size is: " + std::to_string(size));
    uint64_t num_ones = QM::QMUtil::get_bin_ones(int_val);
    logger_->trace("QMProcessHandler::Number of ones is: " + std::to_string(num_ones));
    logger_->trace("QMProcessHandler::Generated minterm of size " + std::to_string(bin_rep.size()));
    std::vector<uint64_t> combined_term_key;
    combined_term_key.push_back(int_val);
    table[num_ones].emplace(combined_term_key, bin_rep);
  }
}

std::optional<QM::dual_rep> QM::QMProcessHandler::combine(const QM::dual_rep& num_one,
                                                          const QM::dual_rep& num_two)
{
  if (num_one.second.size() != num_two.second.size())
  {
    logger_->error("Trying to compare two terms of different binary size");
    return std::nullopt;
  }

  // if this condition triggers
  if (num_one.first.size() != num_two.first.size())
  {
    logger_->error("Trying to compare two terms from different stages of algorithm");
    return std::nullopt;
  }

  int diff_count = 0;
  QM::bin combined_term;
  for (int i = 0; i < num_one.second.size(); i++)
  {
    if (diff_count > 1)
    {
      return std::nullopt;
    }
    if (num_one.second[i] == num_two.second[i])
    {
      combined_term.push_back(num_one.second[i]);
      continue;
    }
    if (num_one.second[i] == QM::States::dc || num_two.second[i] == QM::States::dc)
    {
      return std::nullopt;
    }
    diff_count += 1;
    combined_term.push_back(QM::States::dc);
  }

  // Log an error if this case occurs, since it should be impossible
  if (diff_count == 0)
  {
    logger_->error("Two terms found with no difference, but different number of ones");
    return std::nullopt;
  }

  QM::combined_terms new_combination;
  for (int i = 0; i < num_one.first.size(); i++)
  {
    new_combination.push_back(num_one.first[i]);
    new_combination.push_back(num_two.first[i]);
  }
  std::string str;
  for (auto& state : combined_term)
  {
    if (state == QM::States::dc)
    {
      str += '-';
      continue;
    }
    str += std::to_string(state) + "";
  }
  std::string cmb_term_str = "<";
  for (int i = 0; i < new_combination.size(); i++)
  {
    cmb_term_str += std::to_string(new_combination[i]) + ", ";
  }
  cmb_term_str += ">";
  logger_->trace(cmb_term_str + ": " + str);

  return std::make_pair(new_combination, combined_term);
}

void QM::QMProcessHandler::thread_processing(
    std::pair<const uint64_t, std::map<std::vector<uint64_t>, QM::bin>>& terms_for_num_ones,
    QM::tabular_terms& current_table,
    std::vector<QM::dual_rep>& used_terms,
    QM::tabular_terms& new_table)
{
  std::vector<QM::dual_rep> local_used;
  QM::tabular_terms local_table;
  auto num_ones = terms_for_num_ones.first;
  auto next_num = num_ones + 1;
  auto next_terms_for_num_ones = current_table.find(next_num);
  auto& terms = terms_for_num_ones.second;
  if (next_terms_for_num_ones == current_table.end())
  {
    return;
  }
  // for each term in the given number of ones
  for (std::pair<const std::vector<uint64_t>, QM::bin>& comparison_term : terms)
  {
    // for each term in next number of ones
    for (std::pair<const std::vector<uint64_t>, QM::bin>& next_term :
         next_terms_for_num_ones->second)
    {
      std::optional<QM::dual_rep> combined = combine(comparison_term, next_term);
      if (!combined)
      {
        continue;
      }
      auto new_num_ones = QM::QMUtil::get_num_ones_from_bin(combined->second);
      local_used.push_back(comparison_term);
      local_used.push_back(next_term);
      if (std::count_if(local_table[new_num_ones].begin(), local_table[new_num_ones].end(),
                        [&](const auto& e) { return e.second == combined->second; }) == 0)
      {
        local_table[new_num_ones].emplace(combined->first, combined->second);
        logger_->debug("QMProcessHandler::Emplaced term");
      }
      else
      {
        logger_->debug("QMProcessHandler::Term repeated!");
      }
    }
  }

  for (auto& used : local_used)
  {
    std::lock_guard<std::mutex> lock(thread_lock_);
    used_terms.push_back(used);
  }

  // TODO: push local table back into main, using locking
  for (auto& set_from_num_ones : local_table)
  {
    for (auto& term : set_from_num_ones.second)
    {
      std::lock_guard<std::mutex> lock(thread_lock_);
      new_table[set_from_num_ones.first].emplace(term.first, term.second);
    }
  }
}
