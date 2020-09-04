#pragma once

#include "device_properties.hpp"
#include <pqrs/json.hpp>

namespace krbn {
namespace connected_devices {
namespace details {
class descriptions {
public:
  descriptions(void) : descriptions("", "") {
  }

  descriptions(const std::string& manufacturer,
               const std::string& product) : manufacturer_(manufacturer),
                                             product_(product) {
  }

  descriptions(const device_properties& device_properties) : descriptions(device_properties.get_manufacturer().value_or(""),
                                                                          device_properties.get_product().value_or("")) {
  }

  static descriptions make_from_json(const nlohmann::json& json) {
    return descriptions(pqrs::json::find<std::string>(json, "manufacturer").value_or(""),
                        pqrs::json::find<std::string>(json, "product").value_or(""));
  }

  nlohmann::json to_json(void) const {
    return nlohmann::json({
        {"manufacturer", manufacturer_},
        {"product", product_},
    });
  }

  const std::string& get_manufacturer(void) const {
    return manufacturer_;
  }

  const std::string& get_product(void) const {
    return product_;
  }

  bool operator==(const descriptions& other) const {
    return manufacturer_ == other.manufacturer_ &&
           product_ == other.product_;
  }
  bool operator!=(const descriptions& other) const {
    return !(*this == other);
  }

private:
  std::string manufacturer_;
  std::string product_;
};

inline void to_json(nlohmann::json& json, const descriptions& descriptions) {
  json = descriptions.to_json();
}
} // namespace details
} // namespace connected_devices
} // namespace krbn
