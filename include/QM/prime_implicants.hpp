#ifndef PRIME_IMPLICANTS_HPP_
#define PRIME_IMPLICANTS_HPP_

#include <map>
#include "QM/minterm_map.hpp"
#include "QM/types.hpp"

namespace QM
{
class PrimeImplicants
{
 public:
  PrimeImplicants(QM::tabular_terms& prime_implicants, QM::sMintermMap minterms)
  {
    for (auto& minterm : minterms->get())
    {
      auto& term_int = minterm.first;
      for (auto& term_list : prime_implicants)
      {
        for (auto& term : term_list.second)
        {
          pi_table_[minterm.first].emplace(term.second, false);
        }
      }
    }
  }

 private:
  std::map<uint64_t, std::map<QM::bin, bool>> pi_table_;
};
}  // namespace QM

#endif
