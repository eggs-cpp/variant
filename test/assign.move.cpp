// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "dtor.hpp"
#include "throw.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::operator=(variant<Ts...>&&)", "[variant.assign]")
{
    SECTION("empty source")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(bool(v1) == false);
        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        v2 = std::move(v1);

        CHECK(bool(v1) == false);
        CHECK(bool(v2) == false);
        CHECK(v2.which() == v1.which());
    }

    SECTION("empty target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2;

        REQUIRE(bool(v2) == false);
        REQUIRE(v2.which() == npos);

        v2 = std::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(43);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2 = std::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(std::string{""});

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<std::string>() == "");

        v2 = std::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v1;
            v1.emplace<Throw>();

            REQUIRE(bool(v1) == true);
            REQUIRE(v1.which() == 1u);

            eggs::variant<Dtor, Throw> v2;
            v2.emplace<Dtor>();

            REQUIRE(bool(v2) == true);
            REQUIRE(v2.which() == 0u);
            REQUIRE(Dtor::called == false);

            CHECK_THROWS(v2 = std::move(v1));

            CHECK(bool(v1) == true);
            CHECK(bool(v2) == false);
            CHECK(v1.which() == 1u);
            CHECK(v2.which() == npos);
            CHECK(Dtor::called == true);
        }
        Dtor::called = false;
    }

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
    SECTION("trivially_copyable")
    {
        eggs::variant<int, float> v1(42);

        REQUIRE(std::is_trivially_copyable<decltype(v1)>::value == true);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, float> v2(42.f);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<float>() == 42.f);

        v2 = std::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }
#endif
}

TEST_CASE("variant<>::operator=(variant<>&&)", "[variant.assign]")
{
    eggs::variant<> v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == npos);

    eggs::variant<> v2;

    REQUIRE(bool(v2) == false);
    REQUIRE(v2.which() == npos);

    v2 = std::move(v1);

    CHECK(bool(v1) == false);
    CHECK(bool(v2) == false);
    CHECK(v2.which() == v1.which());
}
