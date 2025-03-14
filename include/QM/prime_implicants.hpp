#ifndef PRIME_IMPLICANTS_HPP_
#define PRIME_IMPLICANTS_HPP_

#include <cstdint>
#include <map>
#include <set>
#include "QM/minterm_map.hpp"
#include "QM/types.hpp"

namespace QM
{
class PrimeImplicants
{
 public:
  PrimeImplicants(QM::combined_list& prime_implicants,
                  QM::sMintermMap minterms,
                  std::shared_ptr<IO::Logger> logger)
      : logger_(std::move(logger)),
        num_inputs_(minterms->input_size()),
        input_(minterms->inputs()),
        output_(minterms->output())
  {
    // for constructor, use prime implicants & minterms to create PI table
    logger_->debug("PrimeImplicants::Received raw prime implicants from table");
    for (auto& minterm : minterms->get())
    {
      auto& term_int = minterm.first;
      for (auto& term : prime_implicants)
      {
        pi_table_[minterm.second][term.second] = covers(minterm, term);
      }
    }
    logger_->debug("PrimeImplicants::initial prime implicants table successfully created from raw");
  }

  // function to solve PI table
  QM::sMintermMap solve();

 private:
  std::map<QM::bin, std::map<QM::bin, bool>> pi_table_;
  // move implicants we want in final product into this set from pi_table_
  std::set<QM::bin> essential_pi_;
  std::shared_ptr<IO::Logger> logger_;
  uint64_t num_inputs_ = 0;
  std::vector<std::string> input_;
  std::string output_;

  void get_essential_pi();
  void petricks_method();
  QM::sMintermMap convert_to_minterm_map();
  bool covers(const std::pair<uint64_t, QM::bin>& num_one, const QM::dual_rep& num_two);
  std::set<std::set<uint64_t>> pos_to_sop(
      const std::vector<std::set<std::set<uint64_t>>>& equation);
};
}  // namespace QM

#endif  // PRIME_IMPLICANTS_HPP_
