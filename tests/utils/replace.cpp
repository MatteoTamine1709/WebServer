#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("replace", "[utils]") {
    SECTION("empty string") {
        std::string result = utils::replace("", "", "");
        REQUIRE(result == "");
    }
    SECTION("empty string and non-empty string") {
        std::string result = utils::replace("", "", "a");
        REQUIRE(result == "");
    }
    SECTION("non-empty string and empty string") {
        std::string result = utils::replace("a", "", "");
        REQUIRE(result == "a");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("a", "", "a");
        REQUIRE(result == "a");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("a", "a", "");
        REQUIRE(result == "");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("a", "a", "a");
        REQUIRE(result == "a");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("a", "a", "b");
        REQUIRE(result == "b");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("a", "b", "a");
        REQUIRE(result == "a");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("a", "b", "b");
        REQUIRE(result == "a");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("ab", "a", "b");
        REQUIRE(result == "bb");
    }
    SECTION("non-empty string and non-empty string") {
        std::string result = utils::replace("ab", "b", "a");
        REQUIRE(result == "aa");
    }
}