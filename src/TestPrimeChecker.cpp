#define CATCH_CONFIG_MAIN // tells Catch to add a main - should only be done once per test runner
#include "catch.hpp"
#include "PrimeChecker.hpp"

TEST_CASE("Test PrimeChecker. 1"){
    PrimeChecker pc;
    REQUIRE(pc.isPrime(5));
}

TEST_CASE("Test PrimeChecker. 31"){
    PrimeChecker pc;
    REQUIRE(pc.isPrime(31));
}

TEST_CASE("Test PrimeChecker. 56"){
    PrimeChecker pc;
    REQUIRE(pc.isPrime(56) == 0);
}

TEST_CASE("Test PrimeChecker. 15"){
    PrimeChecker pc;
    REQUIRE(pc.isPrime(15) == 0);
}

TEST_CASE("Test PrimeChecker. non-prime"){
    PrimeChecker pc;
    REQUIRE_FALSE(pc.isPrime(6));
}


TEST_CASE("Test PrimeChecker. composite"){
    PrimeChecker pc;
    REQUIRE(pc.isComposite(6));
}
