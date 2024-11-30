#ifndef PLA_WRITER_HPP_
#define PLA_WRITER_HPP_

#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <string>
#include "IO/file_writer.hpp"
#include "QM/minterm_map.hpp"
#include "QM/types.hpp"

namespace IO
{
namespace File
{
class PlaWriter : public FileWriter<QM::sMintermMap>
{
 public:
  explicit PlaWriter(const std::string& filename, std::shared_ptr<IO::Logger> logger)
      : FileWriter<QM::sMintermMap>(filename, logger)
  {
  }

  explicit PlaWriter(std::shared_ptr<IO::Logger> logger) : FileWriter<QM::sMintermMap>(logger)
  {
  }

  explicit PlaWriter() : FileWriter<QM::sMintermMap>(nullptr)
  {
  }

 protected:
  std::string process(const QM::sMintermMap& raw_object) override
  {
    // get the time that the file was created
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm utc_time = *std::gmtime(&now_c);
    std::ostringstream time_out_str;
    time_out_str << std::put_time(&utc_time, "%Y-%m-%d %H:%M:%S");

    // print the time in a comment on the top
    std::string result =
        "# PLA Output file generated by QM Simplifier on " + time_out_str.str() + "\n";
    result += ".i " + std::to_string(raw_object->input_size()) + "\n";
    result += ".o " + std::to_string(raw_object->output().size()) + "\n";
    result += ".ilb ";
    for (auto& input : raw_object->inputs())
    {
      result += input + " ";
    }
    result.pop_back();
    result += "\n";
    result += ".ob " + raw_object->output() + "\n";
    result += ".p " + std::to_string(raw_object->num_terms()) + "\n";
    // print all simplified minterms
    for (const auto& minterm : raw_object->get())
    {
      for (const auto& bit_value : minterm.second)
      {
        if (bit_value == QM::States::dc)
        {
          result += "-";
        }
        else
        {
          result += std::to_string(bit_value);
        }
      }
      result += " 1\n";
    }
    result += ".e\n";
    return result;
  }

 private:
};
}  // namespace File
}  // namespace IO

#endif  // PLA_WRITER_HPP_
