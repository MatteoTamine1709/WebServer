#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("join", "[utils]") {
    SECTION("empty tokens") {
        std::string result = utils::join({}, " ");
        REQUIRE(result == "");
    }
    SECTION("one token") {
        std::string result = utils::join({"a"}, " ");
        REQUIRE(result == "a");
    }
    SECTION("two tokens") {
        std::string result = utils::join({"a", "b"}, " ");
        REQUIRE(result == "a b");
    }
    SECTION("two tokens with empty delimiter") {
        std::string result = utils::join({"a", "b"}, "");
        REQUIRE(result == "ab");
    }
}