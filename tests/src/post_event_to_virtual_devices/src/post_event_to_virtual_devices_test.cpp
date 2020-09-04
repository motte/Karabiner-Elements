#include <catch2/catch.hpp>

#include "../../share/manipulator_helper.hpp"
#include "../../share/json_helper.hpp"
#include "manipulator/manipulators/post_event_to_virtual_devices/post_event_to_virtual_devices.hpp"

TEST_CASE("actual examples") {
  auto helper = std::make_unique<krbn::unit_testing::manipulator_helper>();

  helper->run_tests(krbn::unit_testing::json_helper::load_jsonc("json/tests.json"));

  helper = nullptr;
}

TEST_CASE("mouse_key_handler.count_converter") {
  {
    krbn::manipulator::manipulators::post_event_to_virtual_devices::mouse_key_handler::count_converter count_converter(64);
    REQUIRE(count_converter.update(32) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(32) == static_cast<uint8_t>(1));
    REQUIRE(count_converter.update(32) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(32) == static_cast<uint8_t>(1));

    REQUIRE(count_converter.update(16) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(16) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(16) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(16) == static_cast<uint8_t>(1));

    REQUIRE(count_converter.update(-16) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(-16) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(-16) == static_cast<uint8_t>(0));
    REQUIRE(count_converter.update(-16) == static_cast<uint8_t>(-1));
  }
  {
    krbn::manipulator::manipulators::post_event_to_virtual_devices::mouse_key_handler::count_converter count_converter(64);
    REQUIRE(count_converter.update(128) == static_cast<uint8_t>(2));
    REQUIRE(count_converter.update(128) == static_cast<uint8_t>(2));
    REQUIRE(count_converter.update(-128) == static_cast<uint8_t>(-2));
    REQUIRE(count_converter.update(-128) == static_cast<uint8_t>(-2));
  }
}
