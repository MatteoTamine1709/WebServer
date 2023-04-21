#include "src/utils.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("split", "[utils]") {
    SECTION("empty delimiters") {
        std::vector<std::string> tokens = utils::split("a", {});
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "a");
    }
    SECTION("empty string") {
        std::vector<std::string> tokens = utils::split("", {"a"});
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "");
    }
    SECTION("empty string and delimiters") {
        std::vector<std::string> tokens = utils::split("", {});
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "");
    }
    SECTION("no delimiters in string") {
        std::vector<std::string> tokens = utils::split("a", {"b"});
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "a");
    }
    SECTION("one delimiter") {
        std::vector<std::string> tokens = utils::split("a b", {" "});
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "a");
        REQUIRE(tokens[1] == "b");
    }
    SECTION("two delimiters") {
        std::vector<std::string> tokens = utils::split("a b c", {" ", "b"});
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "a");
        REQUIRE(tokens[1] == "c");
    }
    SECTION("two delimiters with empty string") {
        std::vector<std::string> tokens = utils::split("a b c", {"", "b"});
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens[0] == "a");
        REQUIRE(tokens[2] == "b");
        REQUIRE(tokens[4] == "c");
    }
    SECTION("long delimiter") {
        std::vector<std::string> tokens = utils::split("a b c", {"b c"});
        for (auto token : tokens) {
            std::cout << token << std::endl;
        }
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "a ");
    }
    
}