#pragma once

#include "Karabiner-VirtualHIDDevice/dist/include/karabiner_virtual_hid_device_methods.hpp"
#include "types.hpp"
#include <nlohmann/json.hpp>

namespace krbn {
class virtual_hid_device_utility final {
public:
  static nlohmann::json to_json(const pqrs::karabiner_virtual_hid_device::hid_report::modifiers& v) {
    auto json = nlohmann::json::array();

    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::left_control)) {
      json.push_back("left_control");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::left_shift)) {
      json.push_back("left_shift");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::left_option)) {
      json.push_back("left_option");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::left_command)) {
      json.push_back("left_command");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::right_control)) {
      json.push_back("right_control");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::right_shift)) {
      json.push_back("right_shift");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::right_option)) {
      json.push_back("right_option");
    }
    if (v.exists(pqrs::karabiner_virtual_hid_device::hid_report::modifier::right_command)) {
      json.push_back("right_command");
    }

    return json;
  }

  static nlohmann::json to_json(const pqrs::karabiner_virtual_hid_device::hid_report::keys& v,
                                pqrs::hid::usage_page::value_t usage_page) {
    auto json = nlohmann::json::array();

    for (const auto& k : v.get_raw_value()) {
      if (k == 0) {
        continue;
      }

      if (auto key_code = make_key_code(usage_page, pqrs::hid::usage::value_t(k))) {
        json.push_back(make_key_code_name(*key_code));
        continue;
      }

      if (auto consumer_key_code = make_consumer_key_code(usage_page, pqrs::hid::usage::value_t(k))) {
        json.push_back(make_consumer_key_code_name(*consumer_key_code));
        continue;
      }

      json.push_back(k);
    }

    return json;
  }

  static nlohmann::json to_json(const pqrs::karabiner_virtual_hid_device::hid_report::buttons& v) {
    auto json = nlohmann::json::array();

    for (auto b = pointing_button::button1; b <= pointing_button::button32; ++b) {
      if (v.exists(type_safe::get(b))) {
        json.push_back(make_pointing_button_name(b));
      }
    }

    return json;
  }
};
} // namespace krbn

namespace pqrs {
inline void to_json(nlohmann::json& json, const karabiner_virtual_hid_device::hid_report::modifiers& v) {
  json = krbn::virtual_hid_device_utility::to_json(v);
}

inline void to_json(nlohmann::json& json, const karabiner_virtual_hid_device::hid_report::buttons& v) {
  json = krbn::virtual_hid_device_utility::to_json(v);
}
} // namespace pqrs
