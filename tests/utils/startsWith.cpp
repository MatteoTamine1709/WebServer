#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("startsWith", "[utils]") {
    SECTION("empty string") {
        bool result = utils::startsWith("", "");
        REQUIRE(result == true);
    }
    SECTION("empty string and non-empty string") {
        bool result = utils::startsWith("", "a");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and empty string") {
        bool result = utils::startsWith("a", "");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("a", "a");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("a", "b");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("ab", "a");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("ab", "b");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("ab", "ab");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("ab", "ba");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("abc", "ab");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("abc", "bc");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::startsWith("abc", "abc");
        REQUIRE(result == true);
    }
}