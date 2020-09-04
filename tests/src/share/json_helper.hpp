#pragma once

#include "json_utility.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <pqrs/string.hpp>
#include <sstream>
#include <string>

namespace krbn {
namespace unit_testing {
class json_helper final {
public:
  static bool compare_files(const std::string& file_path1,
                            const std::string& file_path2) {
    std::ifstream stream1(file_path1);
    std::ifstream stream2(file_path2);

    if (stream1 && stream2) {
      try {
        nlohmann::json json1;
        nlohmann::json json2;
        stream1 >> json1;
        stream2 >> json2;

        return json1 == json2;
      } catch (...) {
      }
    }

    return false;
  }

  // Rough implementation. Only entire line comments are supported.
  //
  // `// Supported`
  // `[1,2,3], // Not supported this comment`
  // `/* Not supported this comment */`
  static nlohmann::json load_jsonc(const std::string& file_path) {
    std::ifstream json_file(file_path);
    if (!json_file) {
      throw std::runtime_error(file_path + " is not found");
    }

    return json_utility::parse_jsonc(json_file);
  }
};
} // namespace unit_testing
} // namespace krbn
