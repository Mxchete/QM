#include "QM/prime_implicants.hpp"
#include "QM/minterm_map.hpp"

QM::sMintermMap QM::PrimeImplicants::solve()
{
  logger_->trace("PrimeImplicants::Table:");
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
  for (auto& minterm : pi_table_)
  {
    if (std::count_if(minterm.second.begin(), minterm.second.end(),
                      [&](const auto& e) { return e.second == true; }) == 0)
    {
      // push back essential pi & erase minterm since we have covered it
    }
  }
}
