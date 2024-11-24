#include "QM/prime_implicants.hpp"
#include "QM/minterm_map.hpp"

QM::sMintermMap QM::PrimeImplicants::solve()
{
  logger_->trace("PrimeImplicants::Table:");
  get_essential_pi();
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
      essential_pi_.push_back(num_covers[0].first);
      removable_minterms.push_back(map.first);
    }
  }

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
