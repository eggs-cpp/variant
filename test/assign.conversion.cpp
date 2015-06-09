// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"
#include "dtor.hpp"
#include "throw.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::operator=(T&&)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v = 42;

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                v = ConstexprTrivial(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v = 42;

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                v = ConstexprTrivial(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v = 42;

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            CHECK_THROWS(v = Throw{});

            CHECK(bool(v) == false);
            CHECK(v.which() == npos);
            CHECK(Dtor::called == true);
        }
        Dtor::called = false;

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                v = ConstexprTrivial(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("implicit conversion")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v = "42";

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");
    }
}
