#include <catch2/catch.hpp>

#include "manipulator/manipulators/basic/basic.hpp"
#include "manipulator/types.hpp"

TEST_CASE("to_if_held_down") {
  namespace basic = krbn::manipulator::manipulators::basic;
  using krbn::manipulator::event_definition;

  // object

  {
    auto json = nlohmann::json::object({
        {"to_if_held_down", nlohmann::json::object({
                                {"key_code", "tab"},
                            })},
    });

    basic::basic b(json,
                   krbn::core_configuration::details::complex_modifications_parameters());
    REQUIRE(b.get_to_if_held_down());
    REQUIRE(b.get_to_if_held_down()->get_to().size() == 1);
    {
      auto& d = b.get_to_if_held_down()->get_to()[0].get_event_definition();
      REQUIRE(d.get_type() == event_definition::type::key_code);
      REQUIRE(mpark::get<krbn::key_code::value_t>(d.get_value()) == krbn::key_code::keyboard_tab);
    }
  }

  // array

  {
    auto json = nlohmann::json::object({
        {"to_if_held_down", nlohmann::json::array({
                                nlohmann::json::object({{"key_code", "tab"}}),
                                nlohmann::json::object({{"key_code", "spacebar"}}),
                            })},
    });

    basic::basic b(json,
                   krbn::core_configuration::details::complex_modifications_parameters());
    REQUIRE(b.get_to_if_held_down());
    REQUIRE(b.get_to_if_held_down()->get_to().size() == 2);
    {
      auto& d = b.get_to_if_held_down()->get_to()[0].get_event_definition();
      REQUIRE(d.get_type() == event_definition::type::key_code);
      REQUIRE(mpark::get<krbn::key_code::value_t>(d.get_value()) == krbn::key_code::keyboard_tab);
    }
    {
      auto& d = b.get_to_if_held_down()->get_to()[1].get_event_definition();
      REQUIRE(d.get_type() == event_definition::type::key_code);
      REQUIRE(mpark::get<krbn::key_code::value_t>(d.get_value()) == krbn::key_code::keyboard_spacebar);
    }
  }
}
