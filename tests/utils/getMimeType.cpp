#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("getMimeType", "[utils]") {
    utils::initializeMimeMap();
    SECTION("empty string") {
        std::string result = utils::getMimeType("");
        REQUIRE(result == "application/octet-stream");
    }
    SECTION("not existing extension") {
        std::string result = utils::getMimeType("notexisting");
        REQUIRE(result == "application/octet-stream");
    }
    SECTION("existing extension") {
        std::string result = utils::getMimeType("txt");
        REQUIRE(result == "text/plain");
    }
    SECTION("existing extension with uppercase") {
        std::string result = utils::getMimeType("TXT");
        REQUIRE(result == "text/plain");
    }
    SECTION("existing extension with uppercase and spaces") {
        std::string result = utils::getMimeType(" TXT ");
        REQUIRE(result == "text/plain");
    }
}