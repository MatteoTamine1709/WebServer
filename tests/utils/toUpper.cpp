#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("toUpper", "[utils]") {
    SECTION("empty string") {
        std::string result = utils::toUpper("");
        REQUIRE(result == "");
    }
    SECTION("string with spaces") {
        std::string result = utils::toUpper(" a ");
        REQUIRE(result == " A ");
    }
    SECTION("Lowercase string") {
        std::string result = utils::toUpper("a");
        REQUIRE(result == "A");
    }
    SECTION("string with spaces and tabs") {
        std::string result = utils::toUpper(" a\t");
        REQUIRE(result == " A\t");
    }
    SECTION("string with spaces and tabs and newlines") {
        std::string result = utils::toUpper(" a\t\n");
        REQUIRE(result == " A\t\n");
    }
}