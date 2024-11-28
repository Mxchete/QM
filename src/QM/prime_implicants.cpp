#include "QM/prime_implicants.hpp"
#include <cstdint>
#include <unordered_map>
#include "QM/minterm_map.hpp"

QM::sMintermMap QM::PrimeImplicants::solve()
{
  logger_->trace("PrimeImplicants::Table:");
  get_essential_pi();
  simplify_row_dominance();
  simplify_column_dominance();
  petricks_method();

  return convert_to_minterm_map();
}

bool QM::PrimeImplicants::covers(const std::pair<uint64_t, QM::bin>& num_one,
                                 const QM::dual_rep& num_two)
{
  if (num_one.second.size() != num_two.second.size())
  {
    logger_->error("Cannot cover due to size mismatch");
    return false;
  }

  if (std::find(num_two.first.begin(), num_two.first.end(), num_one.first) != num_two.first.end())
  {
    return true;
  }
  return false;
}

void QM::PrimeImplicants::get_essential_pi()
{
  std::vector<QM::bin> removable_minterms;
  for (auto& map : pi_table_)
  {
    std::vector<std::pair<QM::bin, bool>> num_covers;
    std::copy_if(map.second.begin(), map.second.end(), std::back_inserter(num_covers),
                 [](const auto& val) { return val.second; });
    if (num_covers.size() == 1)
    {
      essential_pi_.emplace(num_covers[0].first);
      removable_minterms.push_back(map.first);
    }

    // column dominance
    for (auto& other_terms : pi_table_)
    {
      if (other_terms == map || std::find(removable_minterms.begin(), removable_minterms.end(),
                                          other_terms.first) != removable_minterms.end())
      {
        continue;
      }
      std::vector<std::pair<QM::bin, bool>> cover_list;
      // essentially find if one cover list is a subset of the other
      std::copy_if(other_terms.second.begin(), other_terms.second.end(),
                   std::back_inserter(cover_list),
                   [&](const auto& val) { return val.second && map.second[val.first]; });
      if (cover_list.size() == num_covers.size())
      {
        essential_pi_.emplace(num_covers[0].first);
      }
    }
  }

  // row dominance here? or maybe above loop (add another for loop iterating through pi table again)
  // for (auto& map : pi_table_)
  // {
  // }

  logger_->trace("Old size: " + std::to_string(pi_table_.size()));
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
      minterm.second.erase(removable_pi);
    }
  }
  logger_->trace("New Number of implicants: " + std::to_string(pi_table_.begin()->second.size()));
}

// maybe not needed if its better to do while finding epi
void QM::PrimeImplicants::simplify_row_dominance()
{
}

// also may be able to do while finding epi
void QM::PrimeImplicants::simplify_column_dominance()
{
}

// probably still need
void QM::PrimeImplicants::petricks_method()
{
  if (pi_table_.size() == 0)
  {
    return;
  }
  // maybe first create a map of pi to int "names"
  std::map<uint64_t, QM::bin> int_to_bin;
  std::map<QM::bin, uint64_t> bin_to_int;
  uint64_t new_name = 0;
  for (const auto& implicant : pi_table_.begin()->second)
  {
    int_to_bin.emplace(new_name, implicant.first);
    bin_to_int.emplace(implicant.first, new_name);
    new_name += 1;
  }

  // create an equation in POS form
  std::vector<std::set<std::set<uint64_t>>> equation;
  for (const auto& term : pi_table_)
  {
    std::set<std::set<uint64_t>> single_sum;
    for (const auto& implicant : term.second)
    {
      if (!implicant.second)
      {
        continue;
      }
      std::set<uint64_t> int_sum;
      int_sum.emplace(bin_to_int[implicant.first]);
      single_sum.emplace(int_sum);
    }
    equation.push_back(single_sum);
  }

  // reduce equation to sop
}

std::set<std::set<uint64_t>> QM::PrimeImplicants::pos_to_sop(
    const std::vector<std::set<std::set<uint64_t>>>& equation)
{
  if (equation.size() == 0)
  {
    logger_->fatal("Empty equation in petrick's method!");
  }
  if (equation.size() == 1)
  {
    return equation[0];
  }

  auto sum_one = equation.begin();
  auto sum_two = sum_one++;
  std::vector<std::set<std::set<uint64_t>>> new_equation;
  while (sum_two != equation.end())
  {
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
        bool term_one_larger = (term_one.size() >= term_two.size());
        auto& larger = term_one_larger ? term_one : term_two;
        auto& smaller = term_one_larger ? term_two : term_one;

        // using includes from algorithm header to find a subset
        if (std::includes(larger.begin(), larger.end(), smaller.begin(), smaller.end()))
        {
          remove_list.push_back(larger);
        }
      }
    }

    for (const auto& removable : remove_list)
    {
      new_sum.erase(removable);
    }

    new_equation.push_back(new_sum);
    sum_one = sum_two++;
    if (sum_one != equation.end())
    {
      sum_two = sum_one++;
    }
    else
    {
      break;
    }
  }

  return pos_to_sop(new_equation);
}

QM::sMintermMap QM::PrimeImplicants::convert_to_minterm_map()
{
}
