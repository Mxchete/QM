#ifndef QMUTIL_HPP_
#define QMUTIL_HPP_

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "QM/types.hpp"

namespace QM
{
class QMUtil
{
 public:
  // maximum value supported by program
  static const uint8_t max = 64;

  // convert string to vector of integers
  static std::vector<uint64_t> termtoveci(const std::string& term)
  {
    // expand terms to get rid of dc
    auto expanded = expand_terms(term);
    std::vector<uint64_t> term_int_list;
    // convert from string of binary values to int
    for (auto& term_str : expanded)
    {
      auto term_int = bstrtoi(term_str);
      term_int_list.push_back(term_int);
    }

    return term_int_list;
  }

  // recursively expand terms to get rid of dont care terms
  static std::vector<std::string> expand_terms(const std::string& term)
  {
    // get state of first bit
    States state = get_state(term[0]);
    std::vector<std::string> recursive_return;
    std::vector<std::string> completed_term;
    if (term.length() > 1)
    {
      recursive_return = expand_terms(term.substr(1));
    }
    else
    {
      recursive_return.push_back("");
    }
    // for every string from returned strings
    for (auto& substr : recursive_return)
    {
      switch (state)
      {
        case States::zero:
          completed_term.push_back('0' + substr);
          break;
        case States::one:
          completed_term.push_back('1' + substr);
          break;
        case States::dc:
          completed_term.push_back('0' + substr);
          completed_term.push_back('1' + substr);
          break;
        case States::invalid_state:
          error_handler(Error::invalid_minterm);
          break;
      }
    }
    return completed_term;
  }

  // get number of ones in bin rep of int term
  static uint64_t get_bin_ones(uint64_t term)
  {
    return std::bitset<max>(term).count();
  }

  // get number of ones from bin rep
  static uint64_t get_num_ones_from_bin(QM::bin& term)
  {
    return std::count(term.begin(), term.end(), States::one);
  }

  // convert binary string to integer
  static uint64_t bstrtoi(const std::string& binary_string_representation)
  {
    // convert string to bitset
    std::bitset<max> as_bits(binary_string_representation);
    // from bitset, get as ulong
    return as_bits.to_ullong();
  }

  // converts 0, 1, - to state
  static States get_state(char b)
  {
    switch (b)
    {
      case '0':
        return States::zero;
      case '1':
        return States::one;
      case '-':
        return States::dc;
      default:
        return States::invalid_state;
    }
  }

  // get integer as QM::bin
  static QM::bin get_states(uint64_t term, uint64_t size)
  {
    QM::bin bin_rep;
    std::bitset<max> bin(term);
    std::string bin_str(bin.to_string());
    std::string bin_substr(bin_str.substr(max - size));

    for (char c : bin_substr)
    {
      auto cur_state = get_state(c);
      bin_rep.push_back(cur_state);
    }

    return bin_rep;
  }

  // this was going to be used for reading in blif files, for substituting sub functions into the
  // main minterm map
  static std::string replace_bit_at(uint64_t replace_idx,
                                    uint64_t old_bits,
                                    uint64_t new_bits,
                                    uint64_t new_bits_size)
  {
    std::bitset<sizeof(old_bits)> onset_m_bits(old_bits);
    auto bitstring_m = onset_m_bits.to_string();
    std::bitset<sizeof(new_bits)> subset_bits(new_bits);
    auto subset_str = subset_bits.to_string();
    auto bit_substring_m_part_one = bitstring_m.substr(new_bits_size, replace_idx);
    auto bit_substring_m_part_two = bitstring_m.substr(replace_idx);
    auto new_substr = subset_str.substr(subset_str.length() - new_bits_size);
    auto new_str = bit_substring_m_part_one + new_substr + bit_substring_m_part_two;
    return new_str;
  }

  enum Error
  {
    invalid_minterm,
    prime_implicant_proc_err
  };

  static void error_handler(Error e)
  {
    if (e == invalid_minterm)
    {
      std::cout << "Error when evaluating bit state for minterm" << std::endl;
    }
    else if (e == prime_implicant_proc_err)
    {
      std::cout << "Error with processing prime implicants" << std::endl;
    }

    exit(EXIT_FAILURE);
  }
};
}  // namespace QM

#endif  // QMUTIL_HPP_
