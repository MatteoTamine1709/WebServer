#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("getExtension", "[utils]") {
    REQUIRE(utils::getExtension("test.txt") == "txt");
    REQUIRE(utils::getExtension("test.txt.gz") == "gz");
    REQUIRE(utils::getExtension("test") == "");
    REQUIRE(utils::getExtension("test.") == "");
    REQUIRE(utils::getExtension("test..") == "");
    REQUIRE(utils::getExtension("test.txt.") == "");
    REQUIRE(utils::getExtension("test.txt..") == "");
    REQUIRE(utils::getExtension("test.txt.gz.") == "");
    REQUIRE(utils::getExtension("test.txt.gz") == "gz");
    REQUIRE(utils::getExtension("test.txt") == "txt");
}
