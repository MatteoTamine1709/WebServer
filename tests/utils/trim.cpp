#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("trim", "[utils]") {
    SECTION("empty string") {
        std::string result = utils::trim("");
        REQUIRE(result == "");
    }
    SECTION("string with spaces") {
        std::string result = utils::trim(" a ");
        REQUIRE(result == "a");
    }
    SECTION("string with spaces and tabs") {
        std::string result = utils::trim(" a\t");
        REQUIRE(result == "a");
    }
    SECTION("string with spaces and tabs and newlines") {
        std::string result = utils::trim(" a\t\n");
        REQUIRE(result == "a");
    }
}
