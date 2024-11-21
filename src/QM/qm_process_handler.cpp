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

QM::sMintermMap QM::QMProcessHandler::process()
{
  logger_->trace("QMProcessHandler::Received a map with " +
                 std::to_string(input_map_->get_minterms()->size()) + " Minterms");

  // Generates the first stage of the PI table, before essential PI are selected
  QM::PrimeImplicants pi_table = QM::QMProcessHandler::generate_pi_table();

  // TODO: take essential PI table from generator & solve (using Petricks?)
  QM::sMintermMap sop(pi_table.solve());

  // return the final set of minterms that make up the optimized SOP form
  return sop;
}

QM::PrimeImplicants QM::QMProcessHandler::generate_pi_table()
{
  logger_->trace("QMProcessHandler::Creating table 1");
  // tabular terms is, essentially, a map with uint64_t keys, representing the number of 1's in the
  // binary representation of the minterm, and values that are maps to sets of combined minterms
  // (keys) and their binary representation (vectors whose values are 0, 1, or dc)
  QM::tabular_terms table;
  // for creating the first table, we must add the minterms & don't care terms
  add_terms_to_table(table, input_map_->get_minterms());
  add_terms_to_table(table, input_map_->get_dont_care_terms());

  // From the recursive function, we find the raw PI table that we can process into PI class
  QM::tabular_terms prime_implicants = QM::QMProcessHandler::find_pi(table);

  // Create the PI table (created in PI constructor)
  QM::PrimeImplicants pi_table(prime_implicants, input_map_->get_minterms(), logger_);
  return pi_table;
}

// this method should recursively iterate through each table needed & return the table with combined
// terms + terms it couldn't combine for the given iteration of the function
QM::tabular_terms QM::QMProcessHandler::find_pi(QM::tabular_terms& current_table)
{
  // used_terms holds all terms we were able to combine
  std::vector<QM::dual_rep> used_terms;
  // new_table is the combined table we created w/out uncombined terms
  QM::tabular_terms new_table;
  std::vector<std::thread> processing_threads;
  logger_->debug("Started threaded processing for current table");
  // for each number of ones in the table
  for (std::pair<const uint64_t, std::map<std::vector<uint64_t>, QM::bin>>& terms_for_num_ones :
       current_table)
  {
    // create threads for processing num ones, at most n threads are created but n - 1 threads are
    // used, given input with n variables. For every iteration of this find_pi function fewer
    // threads are needed
    processing_threads.push_back(std::thread(&QM::QMProcessHandler::thread_processing, this,
                                             std::ref(terms_for_num_ones), std::ref(current_table),
                                             std::ref(used_terms), std::ref(new_table)));
  }
  logger_->debug("Created all threads");

  for (auto& thread : processing_threads)
  {
    thread.join();
  }
  logger_->debug("All threads rejoined");

  QM::tabular_terms finished_table;
  if (new_table.size() != 0)
  {
    // finished table will send the new combined table back to this function, so long as we were
    // able to combine variables. This means we can recursively comine terms & get their tables to
    // add to our final product
    finished_table = find_pi(new_table);
  }

  logger_->trace("Used terms size: " + std::to_string(used_terms.size()));
  // Now we add all the terms we couldn't combine
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
  // used for adding minterms / dc terms to first table for finding pi
  for (auto& term : terms->get())
  {
    auto int_val = term.first;
    auto& bin_rep = term.second;
    logger_->trace("QMProcessHandler::For term: " + std::to_string(int_val));
    uint64_t size = input_map_->input_size();
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
  // error where we got 2 terms of different size; should be caught by file reader if I remember to
  // implement that
  if (num_one.second.size() != num_two.second.size())
  {
    logger_->error("Trying to compare two terms of different binary size");
    return std::nullopt;
  }

  // error where combined term and uncombined / lesser combined term are being compared
  if (num_one.first.size() != num_two.first.size())
  {
    logger_->error("Trying to compare two terms from different stages of algorithm");
    return std::nullopt;
  }

  int diff_count = 0;
  QM::bin combined_term;
  for (int i = 0; i < num_one.second.size(); i++)
  {
    // we cannot combine terms with more than one difference
    if (diff_count > 1)
    {
      return std::nullopt;
    }
    // add like terms to our new combined term
    if (num_one.second[i] == num_two.second[i])
    {
      combined_term.push_back(num_one.second[i]);
      continue;
    }
    // we cannot combine terms with different don't care positions
    if (num_one.second[i] == QM::States::dc || num_two.second[i] == QM::States::dc)
    {
      return std::nullopt;
    }
    // first (hopefully only) difference, increment diff count & push back a don't care to combined
    // term (this is the part that is combined)
    diff_count += 1;
    combined_term.push_back(QM::States::dc);
  }

  // Log an error if this case occurs, since it should be impossible (TODO: had this occur with
  // malformed input file, should be fixed in PLAReader)
  if (diff_count == 0)
  {
    logger_->error("Two terms found with no difference, but different number of ones");
    return std::nullopt;
  }

  QM::combined_terms new_combination;
  // create the combined integer representation
  for (int i = 0; i < num_one.first.size(); i++)
  {
    new_combination.push_back(num_one.first[i]);
    new_combination.push_back(num_two.first[i]);
  }

  return std::make_pair(new_combination, combined_term);
}

void QM::QMProcessHandler::thread_processing(
    std::pair<const uint64_t, std::map<std::vector<uint64_t>, QM::bin>>& terms_for_num_ones,
    QM::tabular_terms& current_table,
    std::vector<QM::dual_rep>& used_terms,
    QM::tabular_terms& new_table)
{
  // local terms that need to be pushed back into main
  std::vector<QM::dual_rep> local_used;
  QM::tabular_terms local_table;
  // Lock since we are copying from shared data
  std::lock_guard<std::mutex> lock(thread_lock_);
  auto num_ones = terms_for_num_ones.first;
  auto next_num = num_ones + 1;
  auto next_terms_for_num_ones = current_table.find(next_num);
  auto& terms = terms_for_num_ones.second;
  if (next_terms_for_num_ones == current_table.end())
  {
    return;
  }
  // We have finished copying from shared data for now
  thread_lock_.unlock();
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
        logger_->trace("QMProcessHandler::Emplaced term");
      }
      else
      {
        logger_->trace("QMProcessHandler::Term repeated!");
      }
    }
  }

  for (auto& used : local_used)
  {
    std::lock_guard<std::mutex> lock(thread_lock_);
    used_terms.push_back(used);
  }

  for (auto& set_from_num_ones : local_table)
  {
    for (auto& term : set_from_num_ones.second)
    {
      std::lock_guard<std::mutex> lock(thread_lock_);
      new_table[set_from_num_ones.first].emplace(term.first, term.second);
    }
  }
}
