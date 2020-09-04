#include <catch2/catch.hpp>

#include "keyboard_repeat_detector.hpp"

TEST_CASE("is_repeating") {
  krbn::keyboard_repeat_detector keyboard_repeat_detector;
  REQUIRE(keyboard_repeat_detector.is_repeating() == false);

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_spacebar)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_spacebar)),
                               krbn::event_type::key_up);
  REQUIRE(keyboard_repeat_detector.is_repeating() == false);

  // ----------------------------------------
  // Ignore modifier keys

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_spacebar)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_spacebar)),
                               krbn::event_type::key_down);
  REQUIRE(keyboard_repeat_detector.is_repeating() == true);

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_left_shift)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_left_shift)),
                               krbn::event_type::key_down);
  REQUIRE(keyboard_repeat_detector.is_repeating() == true);

  // ----------------------------------------
  // Cancel by key_up

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_spacebar)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_spacebar)),
                               krbn::event_type::key_down);
  REQUIRE(keyboard_repeat_detector.is_repeating() == true);

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_escape)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_escape)),
                               krbn::event_type::key_down);
  REQUIRE(keyboard_repeat_detector.is_repeating() == true);

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_left_shift)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_left_shift)),
                               krbn::event_type::key_up);
  REQUIRE(keyboard_repeat_detector.is_repeating() == true);

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_spacebar)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_spacebar)),
                               krbn::event_type::key_up);
  REQUIRE(keyboard_repeat_detector.is_repeating() == true);

  keyboard_repeat_detector.set(*(krbn::make_hid_usage_page(krbn::key_code::keyboard_escape)),
                               *(krbn::make_hid_usage(krbn::key_code::keyboard_escape)),
                               krbn::event_type::key_up);
  REQUIRE(keyboard_repeat_detector.is_repeating() == false);

  // ----------------------------------------
  // hid_value

  {
    pqrs::osx::iokit_hid_value hid_value(krbn::absolute_time_point(0),
                                         1,
                                         *(krbn::make_hid_usage_page(krbn::key_code::keyboard_spacebar)),
                                         *(krbn::make_hid_usage(krbn::key_code::keyboard_spacebar)));
    keyboard_repeat_detector.set(hid_value);
    REQUIRE(keyboard_repeat_detector.is_repeating() == true);
  }
  {
    pqrs::osx::iokit_hid_value hid_value(krbn::absolute_time_point(0),
                                         0,
                                         *(krbn::make_hid_usage_page(krbn::key_code::keyboard_spacebar)),
                                         *(krbn::make_hid_usage(krbn::key_code::keyboard_spacebar)));
    keyboard_repeat_detector.set(hid_value);
    REQUIRE(keyboard_repeat_detector.is_repeating() == false);
  }
}
