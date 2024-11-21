#include "QM/prime_implicants.hpp"
#include "QM/minterm_map.hpp"

QM::sMintermMap QM::PrimeImplicants::solve()
{
  // logger_->trace("PrimeImplicants::Table:");
}

bool QM::PrimeImplicants::covers(const QM::bin& num_one, const QM::bin& num_two)
{
  if (num_one.size() != num_two.size())
  {
    logger_->error("Cannot cover due to size mismatch");
    return false;
  }

  for (int i = 0; i < num_one.size(); i++)
  {
    if (num_one[i] == num_two[i])
    {
      continue;
    }
    if (num_one[i] == QM::States::dc || num_two[i] == QM::States::dc)
    {
      continue;
    }
    return false;
  }

  return true;
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
