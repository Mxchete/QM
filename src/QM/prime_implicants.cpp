#include "QM/prime_implicants.hpp"
#include <algorithm>
#include <climits>
#include <cstdint>
#include <memory>
#include <string>
#include "QM/minterm_map.hpp"
#include "QM/types.hpp"

// solve gets essential pi & then performs petricks method, return converted essential primes
QM::sMintermMap QM::PrimeImplicants::solve()
{
  logger_->trace("PrimeImplicants::Table:");
  get_essential_pi();
  logger_->trace("Essential PI obtained!");
  petricks_method();
  logger_->trace("Finished with petricks!");

  return convert_to_minterm_map();
}

// find if prime implicant covers a given minterm
bool QM::PrimeImplicants::covers(const std::pair<uint64_t, QM::bin>& num_one,
                                 const QM::dual_rep& num_two)
{
  if (num_one.second.size() != num_two.second.size())
  {
    logger_->error("Cannot cover due to size mismatch");
    return false;
  }

  // num_one is minterm, num_two is prime, if num_one is in num_two, it is covered by num_two
  if (std::find(num_two.first.begin(), num_two.first.end(), num_one.first) != num_two.first.end())
  {
    return true;
  }
  // otherwise, it is not covered
  return false;
}

void QM::PrimeImplicants::get_essential_pi()
{
  std::vector<QM::bin> removable_minterms;
  for (auto& map : pi_table_)
  {
    std::vector<std::pair<QM::bin, bool>> num_covers;
    // get number of implicants that cover this minterm
    std::copy_if(map.second.begin(), map.second.end(), std::back_inserter(num_covers),
                 [](const auto& val) { return val.second; });
    // if only one, this is an essential prime
    if (num_covers.size() == 1)
    {
      essential_pi_.emplace(num_covers[0].first);
      removable_minterms.push_back(map.first);
    }

    // column dominance
    for (auto& other_terms : pi_table_)
    {
      if (other_terms == map)
      {
        continue;
      }
      std::vector<std::pair<QM::bin, bool>> cover_list;
      // essentially find if one cover list is a subset of the other
      std::copy_if(other_terms.second.begin(), other_terms.second.end(),
                   std::back_inserter(cover_list),
                   [&](const auto& val) { return val.second && map.second[val.first]; });
      logger_->trace("cov: " + std::to_string(cover_list.size()) +
                     ", num: " + std::to_string(num_covers.size()));
      if (cover_list.size() == num_covers.size())
      {
        // we can remove dominated terms
        // essential_pi_.emplace(num_covers[0].first);
        removable_minterms.push_back(other_terms.first);
      }
    }
  }

  logger_->trace("Old size: " + std::to_string(pi_table_.size()));
  // remove terms we marked
  for (const auto& removable : removable_minterms)
  {
    pi_table_.erase(removable);
  }
  logger_->trace("New size: " + std::to_string(pi_table_.size()));

  logger_->trace("Old Number of implicants: " + std::to_string(pi_table_.begin()->second.size()));
  for (auto& minterm : pi_table_)
  {
    for (const auto& removable_pi : essential_pi_)
    {
      // remove implicants we marked
      minterm.second.erase(removable_pi);
    }
  }
  logger_->trace("New Number of implicants: " + std::to_string(pi_table_.begin()->second.size()));

  logger_->debug("Current EPI");
  for (const auto& term : essential_pi_)
  {
    std::string dbg;
    for (const auto& state : term)
    {
      if (state == QM::States::dc)
      {
        dbg += '-';
        continue;
      }
      dbg += std::to_string(state);
    }
    logger_->debug(dbg);
  }
}

void QM::PrimeImplicants::petricks_method()
{
  if (pi_table_.size() == 0 || pi_table_.begin()->second.size() == 0)
  {
    return;
  }

  // edge case where only 1 prime implicant is left
  if (pi_table_.begin()->second.size() == 1)
  {
    essential_pi_.emplace(pi_table_.begin()->second.begin()->first);
    return;
  }
  std::map<uint64_t, QM::bin> int_to_bin;
  std::map<QM::bin, uint64_t> bin_to_int;
  uint64_t new_name = 0;
  // map implicants to "names" for petrick
  for (const auto& implicant : pi_table_.begin()->second)
  {
    int_to_bin.emplace(new_name, implicant.first);
    bin_to_int.emplace(implicant.first, new_name);
    new_name += 1;
  }

  // create an equation in POS form
  // inner set holds anding of literals, i.e. AB'
  // outer set is ORing of literals, i.e. A+B
  // vector is anding of ORed literals, i.e. (A+B)(B+C)
  std::vector<std::set<std::set<uint64_t>>> equation;
  for (const auto& term : pi_table_)
  {
    std::set<std::set<uint64_t>> single_sum;
    logger_->trace("sum");
    for (const auto& implicant : term.second)
    {
      if (!implicant.second)
      {
        logger_->trace("Cannot get implicant!");
        continue;
      }
      logger_->trace("adding term: " + std::to_string(bin_to_int[implicant.first]));
      std::set<uint64_t> int_sum;
      int_sum.emplace(bin_to_int[implicant.first]);
      single_sum.emplace(int_sum);
    }
    equation.push_back(single_sum);
  }

  // reduce equation to sop
  logger_->trace("Getting SOP");
  const auto sop_eq = pos_to_sop(equation);
  logger_->trace("SOP obtained");
  // now pick the lowest terms
  uint64_t min_size = ULONG_MAX;
  for (const auto& term : sop_eq)
  {
    if (term.size() < min_size)
    {
      min_size = term.size();
    }
  }
  logger_->trace("Min size: " + std::to_string(min_size));
  std::vector<std::set<uint64_t>> smallest_terms;
  // get all terms whose size is lowest
  for (auto& small_term : sop_eq)
  {
    if (small_term.size() > min_size)
    {
      continue;
    }
    smallest_terms.push_back(small_term);
  }

  logger_->trace("Smallest terms found: " + std::to_string(smallest_terms.size()));
  std::set<uint64_t> selection = *smallest_terms.begin();
  uint64_t lowest_num_literals = ULLONG_MAX;
  // select the term from the smallest terms that has the fewest number of literals
  for (const auto& potential_selection : smallest_terms)
  {
    // double map since for each literal A there are two literals: A and A'
    std::vector<bool> literal_has_appeared(num_inputs_, false);
    std::vector<bool> inv_literal_has_appeared(num_inputs_, false);
    for (const auto& Px : potential_selection)
    {
      const auto& bin_rep = int_to_bin[Px];
      for (int i = 0; i < num_inputs_; i++)
      {
        try
        {
          if (bin_rep[i] == QM::States::one)
          {
            // mark literal as true if appears as 1
            literal_has_appeared[i] = true;
          }
          else if (bin_rep[i] == QM::States::zero)
          {
            // mark as true for inv if appears as 0
            inv_literal_has_appeared[i] = true;
          }
        }
        catch (...)
        {
          logger_->fatal("Could not get literals of term");
        }
      }
    }

    // for literal & inv literal, count truth
    const auto number_of_literals =
        std::count_if(literal_has_appeared.begin(), literal_has_appeared.end(),
                      [&](const auto& e) { return e; }) +
        std::count_if(inv_literal_has_appeared.begin(), inv_literal_has_appeared.end(),
                      [&](const auto& e) { return e; });

    // if new number of literals is lower, choose this
    if (number_of_literals < lowest_num_literals)
    {
      lowest_num_literals = number_of_literals;
      selection = potential_selection;
    }
  }

  // push all prime implicants from the smallest term
  for (const auto& implicant_int : selection)
  {
    logger_->trace(std::to_string(implicant_int) + " ");
    const auto& bin_rep = int_to_bin[implicant_int];
    essential_pi_.emplace(bin_rep);
  }
}

std::set<std::set<uint64_t>> QM::PrimeImplicants::pos_to_sop(
    const std::vector<std::set<std::set<uint64_t>>>& equation)
{
  if (equation.size() == 0)
  {
    logger_->fatal("Empty equation in petrick's method!");
  }
  // we have found the SOP equation
  if (equation.size() == 1)
  {
    logger_->trace("Final EQ reached: " + std::to_string(equation[0].size()));
    return equation[0];
  }

  // compare sets of two terms together
  logger_->trace("number of sums left to multiply: " + std::to_string(equation.size()));
  auto sum_one = equation.begin();
  auto sum_two = std::next(sum_one);
  std::vector<std::set<std::set<uint64_t>>> new_equation;
  while (sum_one != equation.end())
  {
    // break if there are an odd number of sums
    if (sum_two == equation.end())
    {
      logger_->trace("Odd num");
      new_equation.push_back(*sum_one);
      break;
    }
    // combine two terms
    // for every term in old eq
    std::set<std::set<uint64_t>> new_sum;
    for (const auto& term_one : *sum_one)
    {
      // compare it to term in other old equation
      for (const auto& term_two : *sum_two)
      {
        std::set<uint64_t> new_term;
        new_term.insert(term_one.begin(), term_one.end());
        new_term.insert(term_two.begin(), term_two.end());
        new_sum.emplace(new_term);
      }
    }
    // now remove repeat terms
    // compare each term and find the smallest
    std::vector<std::set<uint64_t>> remove_list;
    for (const auto& term_one : new_sum)
    {
      for (const auto& term_two : new_sum)
      {
        if (term_one == term_two)
        {
          continue;
        }
        bool term_one_larger = (term_one.size() >= term_two.size());
        auto& larger = term_one_larger ? term_one : term_two;
        auto& smaller = term_one_larger ? term_two : term_one;
        logger_->trace(std::to_string(larger.size()));
        logger_->trace(std::to_string(smaller.size()));

        // using includes from algorithm header to find a subset
        if (std::includes(larger.begin(), larger.end(), smaller.begin(), smaller.end()))
        {
          remove_list.push_back(larger);
        }
      }
    }

    // remove larger terms that can be eliminated via boolean algebra
    for (const auto& removable : remove_list)
    {
      new_sum.erase(removable);
    }

    // add this sum, then increment which terms we look at
    new_equation.push_back(new_sum);
    sum_one = std::next(sum_two);
    if (sum_one != equation.end())
    {
      sum_two = std::next(sum_one);
    }
    else
    {
      break;
    }
  }

  // recursively iterate until we have reached SOP form
  return pos_to_sop(new_equation);
}

QM::sMintermMap QM::PrimeImplicants::convert_to_minterm_map()
{
  QM::sMintermMap result = std::make_shared<QM::MintermMap>(input_, output_, logger_);
  int i = 0;
  // just adding to onset with i since we need to tie each minterm to an int
  for (const auto& new_term : essential_pi_)
  {
    result->add_onset_term(i, new_term);
    i += 1;
  }
  logger_->trace("Created sMintermMap with " + std::to_string(result->num_terms()) + " minterms");

  return result;
}
