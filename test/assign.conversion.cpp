// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>
#include <typeinfo>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"
#include "dtor.hpp"
#include "throw.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::operator=(T&&)", "[variant.assign]")
{
    // empty target
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v = 42;

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
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

    // same target
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v = 42;

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

        // dtor
        {
            eggs::variant<int, Dtor> v(eggs::variants::in_place<Dtor>);

            REQUIRE(v.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v = Dtor();

            CHECK(v.which() == 1u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
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

    // different target
    {
        eggs::variant<int, std::string> v(std::string(""));

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v = 42;

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

        // dtor
        {
            eggs::variant<int, Dtor> v(eggs::variants::in_place<Dtor>);

            REQUIRE(v.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v = 42;

            CHECK(v.which() == 0u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v = Throw{});

            CHECK(bool(v) == false);
            CHECK(v.which() == npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
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

    // implicit conversion
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v = "42";

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif
    }

    // sfinae
    {
        CHECK((
            !std::is_assignable<
                eggs::variant<int>&, std::string
            >::value));
        CHECK((
            !std::is_assignable<
                eggs::variant<int, int>&, int
            >::value));
        CHECK((
            !std::is_assignable<
                eggs::variant<int, int const>&, int
            >::value));
    }
}
