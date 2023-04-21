#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <cstdint>

uint32_t factorial( uint32_t number ) {
    return number <= 1 ? number : factorial(number-1) * number;
}

std::uint64_t Fibonacci(std::uint64_t number) {
    return number < 2 ? 1 : Fibonacci(number - 1) + Fibonacci(number - 2);
}

// TEST_CASE( "Factorials are computed", "[factorial]" ) {
//     REQUIRE( factorial( 1) == 1 );
//     REQUIRE( factorial( 2) == 2 );
//     REQUIRE( factorial( 3) == 6 );
//     REQUIRE( factorial(10) == 3'628'800 );
// }

// TEST_CASE( "Fibonacci benchmark", "[benchmark]" ) {
//     BENCHMARK("Fibonacci 20") {
//         return Fibonacci(20);
//     };

//     BENCHMARK("Fibonacci 25") {
//         return Fibonacci(25);
//     };

//     BENCHMARK("Fibonacci 30") {
//         return Fibonacci(30);
//     };

//     BENCHMARK("Fibonacci 35") {
//         return Fibonacci(35);
//     };

//     BENCHMARK_ADVANCED("advanced")(Catch::Benchmark::Chronometer meter) {
//         meter.measure([] { return Fibonacci(15); });
//     };
// }