#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("toLower", "[utils]") {
    SECTION("empty string") {
        std::string result = utils::toLower("");
        REQUIRE(result == "");
    }
    SECTION("string with spaces") {
        std::string result = utils::toLower(" a ");
        REQUIRE(result == " a ");
    }
    SECTION("Capitalized string") {
        std::string result = utils::toLower("A");
        REQUIRE(result == "a");
    }
    SECTION("string with spaces and tabs") {
        std::string result = utils::toLower(" a\t");
        REQUIRE(result == " a\t");
    }
    SECTION("string with spaces and tabs and newlines") {
        std::string result = utils::toLower(" a\t\n");
        REQUIRE(result == " a\t\n");
    }
}