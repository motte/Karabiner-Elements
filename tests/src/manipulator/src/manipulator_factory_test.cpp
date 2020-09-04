#include <catch2/catch.hpp>

#include "../../share/manipulator_conditions_helper.hpp"
#include "../../share/manipulator_helper.hpp"

namespace modifier_definition = krbn::manipulator::modifier_definition;
using krbn::manipulator::event_definition;
using krbn::manipulator::to_event_definition;

TEST_CASE("manipulator.manipulator_factory") {
  {
    nlohmann::json json({
        {"type", "basic"},
        {
            "from",
            {
                {
                    "key_code",
                    "escape",
                },
                {
                    "modifiers",
                    {
                        {"mandatory", {
                                          "left_shift",
                                          "left_option",
                                      }},
                        {"optional", {
                                         "any",
                                     }},
                    },
                },
            },
        },
        {
            "to",
            {
                {
                    {
                        "pointing_button",
                        "button1",
                    },
                },
            },
        },
    });
    krbn::core_configuration::details::complex_modifications_parameters parameters;
    auto manipulator = krbn::manipulator::manipulator_factory::make_manipulator(json,
                                                                                parameters);
    REQUIRE(dynamic_cast<krbn::manipulator::manipulators::basic::basic*>(manipulator.get()) != nullptr);
    REQUIRE(dynamic_cast<krbn::manipulator::manipulators::nop*>(manipulator.get()) == nullptr);
    REQUIRE(manipulator->get_valid() == true);
    REQUIRE(manipulator->active() == false);

    auto basic = dynamic_cast<krbn::manipulator::manipulators::basic::basic*>(manipulator.get());
    REQUIRE(basic->get_from().get_event_definitions().size() == 1);
    REQUIRE(basic->get_from().get_event_definitions().front().get_type() == event_definition::type::key_code);
    REQUIRE(basic->get_from().get_event_definitions().front().get_key_code() == krbn::key_code::keyboard_escape);
    REQUIRE(basic->get_from().get_event_definitions().front().get_pointing_button() == std::nullopt);
    REQUIRE(basic->get_from().get_from_modifiers_definition().get_mandatory_modifiers() == std::set<modifier_definition::modifier>({
                                                                                               modifier_definition::modifier::left_shift,
                                                                                               modifier_definition::modifier::left_option,
                                                                                           }));
    REQUIRE(basic->get_from().get_from_modifiers_definition().get_optional_modifiers() == std::set<modifier_definition::modifier>({
                                                                                              modifier_definition::modifier::any,
                                                                                          }));
    REQUIRE(basic->get_to().size() == 1);
    REQUIRE(basic->get_to()[0].get_event_definition().get_type() == event_definition::type::pointing_button);
    REQUIRE(basic->get_to()[0].get_event_definition().get_key_code() == std::nullopt);
    REQUIRE(basic->get_to()[0].get_event_definition().get_pointing_button() == krbn::pointing_button::button1);
    REQUIRE(basic->get_to()[0].get_modifiers() == std::set<modifier_definition::modifier>());
  }
}

TEST_CASE("manipulator_factory::make_device_if_condition") {
  krbn::unit_testing::manipulator_conditions_helper manipulator_conditions_helper;

  nlohmann::json json({
      {"identifiers", {
                          {
                              "vendor_id",
                              1234,
                          },
                          {
                              "product_id",
                              5678,
                          },
                          {
                              "is_keyboard",
                              true,
                          },
                          {
                              "is_pointing_device",
                              false,
                          },
                      }},
  });
  krbn::core_configuration::details::device device(json);

  auto device_id_1234_5678_keyboard = manipulator_conditions_helper.prepare_device(
      pqrs::hid::vendor_id::value_t(1234), pqrs::hid::product_id::value_t(5678), std::nullopt, true, false);

  auto device_id_1234_5678_mouse = manipulator_conditions_helper.prepare_device(
      pqrs::hid::vendor_id::value_t(1234), pqrs::hid::product_id::value_t(5678), std::nullopt, false, true);

  auto device_id_1234_5000_keyboard = manipulator_conditions_helper.prepare_device(
      pqrs::hid::vendor_id::value_t(1234), pqrs::hid::product_id::value_t(5000), std::nullopt, true, false);

  auto c = krbn::manipulator::manipulator_factory::make_device_if_condition(device);

  {
    auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1234_5678_keyboard);
    REQUIRE(c->is_fulfilled(e, manipulator_conditions_helper.get_manipulator_environment()) == true);
  }
  {
    auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1234_5678_mouse);
    REQUIRE(c->is_fulfilled(e, manipulator_conditions_helper.get_manipulator_environment()) == false);
  }
  {
    auto e = manipulator_conditions_helper.make_event_queue_entry(device_id_1234_5000_keyboard);
    REQUIRE(c->is_fulfilled(e, manipulator_conditions_helper.get_manipulator_environment()) == false);
  }
}

TEST_CASE("errors") {
  {
    nlohmann::json json;
    krbn::core_configuration::details::complex_modifications_parameters parameters;
    REQUIRE_THROWS_AS(
        krbn::manipulator::manipulator_factory::make_manipulator(json, parameters),
        pqrs::json::unmarshal_error);
    REQUIRE_THROWS_WITH(
        krbn::manipulator::manipulator_factory::make_manipulator(json, parameters),
        "`type` must be specified: null");
  }
}
