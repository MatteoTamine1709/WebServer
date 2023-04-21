#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("endsWith", "[utils]") {
    SECTION("empty string") {
        bool result = utils::endsWith("", "");
        REQUIRE(result == true);
    }
    SECTION("empty string and non-empty string") {
        bool result = utils::endsWith("", "a");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and empty string") {
        bool result = utils::endsWith("a", "");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("a", "a");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("a", "b");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("ab", "a");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("ab", "b");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("ab", "ab");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("ab", "ba");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("abc", "ab");
        REQUIRE(result == false);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("abc", "bc");
        REQUIRE(result == true);
    }
    SECTION("non-empty string and non-empty string") {
        bool result = utils::endsWith("abc", "abc");
        REQUIRE(result == true);
    }
}