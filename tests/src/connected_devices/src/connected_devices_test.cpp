#include <catch2/catch.hpp>

#include "connected_devices/connected_devices.hpp"
#include "json_utility.hpp"

TEST_CASE("connected_devices") {
  {
    krbn::connected_devices::connected_devices connected_devices;
    REQUIRE(connected_devices.to_json() == nlohmann::json::array());
  }
  {
    krbn::connected_devices::connected_devices connected_devices;

    {
      krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                  "product1");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                           pqrs::hid::product_id::value_t(5678),
                                           true,
                                           false);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      true,
                                                      false);
      connected_devices.push_back_device(device);
    }
    {
      krbn::connected_devices::details::descriptions descriptions("manufacturer1 (ignored)",
                                                                  "product1 (ignored)");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                           pqrs::hid::product_id::value_t(5678),
                                           true,
                                           false);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      true,
                                                      false);
      connected_devices.push_back_device(device);
    }
    {
      krbn::connected_devices::details::descriptions descriptions("manufacturer3",
                                                                  "product3");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(2345),
                                           pqrs::hid::product_id::value_t(6789),
                                           false,
                                           true);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      false,
                                                      false);
      connected_devices.push_back_device(device);
    }
    {
      krbn::connected_devices::details::descriptions descriptions("manufacturer2",
                                                                  "product2");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(1234),
                                           pqrs::hid::product_id::value_t(5679),
                                           false,
                                           true);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      false,
                                                      true);
      connected_devices.push_back_device(device);
    }
    {
      krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                  "product4");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(123),
                                           pqrs::hid::product_id::value_t(678),
                                           false,
                                           true);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      false,
                                                      true);
      connected_devices.push_back_device(device);
    }
    {
      krbn::connected_devices::details::descriptions descriptions("manufacturer1",
                                                                  "product4");
      krbn::device_identifiers identifiers(pqrs::hid::vendor_id::value_t(123),
                                           pqrs::hid::product_id::value_t(678),
                                           true,
                                           false);
      krbn::connected_devices::details::device device(descriptions,
                                                      identifiers,
                                                      true,
                                                      false);
      connected_devices.push_back_device(device);
    }

    REQUIRE(connected_devices.is_loaded() == false);
    REQUIRE(connected_devices.get_devices().size() == 5);
    REQUIRE(connected_devices.get_devices()[0].get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
    REQUIRE(connected_devices.get_devices()[0].get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(5678));
    REQUIRE(connected_devices.get_devices()[1].get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(1234));
    REQUIRE(connected_devices.get_devices()[1].get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(5679));
    REQUIRE(connected_devices.get_devices()[2].get_identifiers().get_vendor_id() == pqrs::hid::vendor_id::value_t(2345));
    REQUIRE(connected_devices.get_devices()[2].get_identifiers().get_product_id() == pqrs::hid::product_id::value_t(6789));

    std::ifstream ifs("json/connected_devices.json");

    REQUIRE(connected_devices.to_json() == krbn::json_utility::parse_jsonc(ifs));
  }

  {
    krbn::connected_devices::connected_devices connected_devices("json/connected_devices.json");

    REQUIRE(connected_devices.is_loaded() == true);
    REQUIRE(connected_devices.get_devices().size() == 5);
    REQUIRE(connected_devices.get_devices()[0].get_is_built_in_keyboard() == true);
    REQUIRE(connected_devices.get_devices()[0].get_is_built_in_trackpad() == false);
    REQUIRE(connected_devices.get_devices()[1].get_is_built_in_keyboard() == false);
    REQUIRE(connected_devices.get_devices()[1].get_is_built_in_trackpad() == true);
  }

  {
    krbn::connected_devices::connected_devices connected_devices("json/not_found.json");

    REQUIRE(connected_devices.is_loaded() == false);
    REQUIRE(connected_devices.get_devices().size() == 0);
  }

  {
    krbn::connected_devices::connected_devices connected_devices("json/broken.json");

    REQUIRE(connected_devices.is_loaded() == false);
    REQUIRE(connected_devices.get_devices().size() == 0);
  }
}
