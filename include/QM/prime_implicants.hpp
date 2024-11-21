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
  PrimeImplicants(QM::tabular_terms& prime_implicants,
                  QM::sMintermMap minterms,
                  std::shared_ptr<IO::Logger> logger)
      : logger_(std::move(logger))
  {
    logger_->debug("PrimeImplicants::Received raw prime implicants from table");
    for (auto& minterm : minterms->get())
    {
      auto& term_int = minterm.first;
      for (auto& term_list : prime_implicants)
      {
        for (auto& term : term_list.second)
        {
          pi_table_[minterm.second][term.second] = covers(minterm.second, term.second);
        }
      }
    }
    logger_->debug("PrimeImplicants::initial prime implicants table successfully created from raw");
  }

  QM::sMintermMap solve();

 private:
  std::map<QM::bin, std::map<QM::bin, bool>> pi_table_;
  std::vector<QM::bin> essential_pi_;
  std::shared_ptr<IO::Logger> logger_;

  void get_essential_pi();
  bool covers(const QM::bin& num_one, const QM::bin& num_two);
};
}  // namespace QM

#endif
