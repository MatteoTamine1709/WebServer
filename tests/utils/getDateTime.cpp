#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("getDateTime", "[utils]") {
    SECTION("Get DateTime") {
        std::string result = utils::getDateTime();
        REQUIRE(result.length() > 0);
    }
}