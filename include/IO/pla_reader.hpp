#ifndef PLA_READER_HPP_
#define PLA_READER_HPP_

#include <stdlib.h>
#include <cstdint>
#include <memory>
#include <string>
#include "IO/file_reader.hpp"
#include "QM/minterm_and_dc_map.hpp"
#include "QM/minterm_map.hpp"

namespace IO
{
namespace File
{
class PlaReader : public FileReader<QM::sBooleanFunction>
{
 public:
  explicit PlaReader(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : FileReader<QM::sBooleanFunction>(filename, logger)
  {
    QM::sMintermMap minterms(std::make_shared<QM::MintermMap>(logger));
    QM::sMintermMap dc(std::make_shared<QM::MintermMap>(logger));
    QM::MintermDCMap minterm_dc_map(minterms, dc);
    minterm_dc_map_ = std::make_shared<QM::MintermDCMap>(minterm_dc_map);
  }

 protected:
  bool read_line(const std::string& line) override;

  // return the minterm & dc term map that was created
  inline QM::sBooleanFunction process() override
  {
    return minterm_dc_map_;
  }

 private:
  std::vector<std::string> input_labels_;
  QM::sBooleanFunction minterm_dc_map_;
  std::string current_token_;
  std::string output_;
  uint64_t num_product_terms_ = 0;
  uint64_t num_inputs_;
  uint64_t received_ = 0;
  bool input_labels_provided_ = false;
  bool output_labels_provided_ = false;
};
}  // namespace File
}  // namespace IO

#endif  // PLA_READER_HPP_
