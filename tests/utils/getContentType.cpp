#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("getContentType", "[utils]") {
    utils::initializeMimeMap();
    SECTION("empty string") {
        std::string result = utils::getContentType("");
        REQUIRE(result == "application/octet-stream");
    }
    SECTION("not existing extension") {
        std::string result = utils::getContentType("notexisting");
        REQUIRE(result == "application/octet-stream");
    }
    SECTION("existing extension") {
        std::string result = utils::getContentType("test.txt");
        REQUIRE(result == "text/plain");
    }
    SECTION("existing extension with uppercase") {
        std::string result = utils::getContentType("test.TXT");
        REQUIRE(result == "text/plain");
    }
}