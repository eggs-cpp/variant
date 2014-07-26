// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "dtor.hpp"
#include "throw.hpp"

constexpr std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::operator=(variant<Ts...> const&)", "[variant.assign]")
{
    SECTION("empty source")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(bool(v1) == false);
        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);

        v2 = v1;

        REQUIRE(bool(v2) == false);
        REQUIRE(v2.which() == v1.which());
    }

    SECTION("empty target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2;

        REQUIRE(bool(v2) == false);
        REQUIRE(v2.which() == npos);

        v2 = v1;

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v1.target<int>() == 42);
        REQUIRE(*v2.target<int>() == 42);
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(43);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2 = v1;

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v1.target<int>() == 42);
        REQUIRE(*v2.target<int>() == 42);
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(std::string{""});

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 1);
        REQUIRE(*v2.target<std::string>() == "");

        v2 = v1;

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v1.target<int>() == 42);
        REQUIRE(*v2.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v1;
            v1.emplace<Throw>();

            REQUIRE(bool(v1) == true);
            REQUIRE(v1.which() == 1);

            eggs::variant<Dtor, Throw> v2;
            v2.emplace<Dtor>();

            REQUIRE(bool(v2) == true);
            REQUIRE(v2.which() == 0);
            REQUIRE(Dtor::called == false);

            bool exception_thrown = false;
            try
            {
                v2 = v1;
            } catch (...) {
                exception_thrown = true;
            }
            REQUIRE(exception_thrown);
            REQUIRE(bool(v1) == true);
            REQUIRE(bool(v2) == false);
            REQUIRE(v1.which() == 1);
            REQUIRE(v2.which() == npos);
            REQUIRE(Dtor::called == true);
        }
        Dtor::called = false;
    }
}

TEST_CASE("variant<>::operator=(variant<> const&)", "[variant.assign]")
{
    eggs::variant<> v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == npos);

    eggs::variant<> v2;

    REQUIRE(bool(v2) == false);
    REQUIRE(v2.which() == npos);

    v2 = v1;

    REQUIRE(bool(v2) == false);
    REQUIRE(v2.which() == v1.which());
}
