// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <typeinfo>
#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>
#include <eggs/variant/detail/utility.hpp>

using eggs::variants::detail::move;

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"
#include "dtor.hpp"
#include "throw.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::operator=(variant<Ts...>&&)", "[variant.assign]")
{
    // empty source
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(bool(v1) == false);
        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        v2 = ::move(v1);

        CHECK(bool(v1) == false);
        CHECK(bool(v2) == false);
        CHECK(v2.which() == v1.which());

        // dtor
        {
            eggs::variant<int, Dtor> v1;
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == npos);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v2 = ::move(v1);

            CHECK(v2.which() == npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1;
                eggs::variant<int, ConstexprTrivial> v2(ConstexprTrivial(42));
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // empty target
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2;

        REQUIRE(bool(v2) == false);
        REQUIRE(v2.which() == npos);

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2;
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(43);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(eggs::variants::in_place<Dtor>);
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == 1u);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v2 = ::move(v1);

            CHECK(v2.which() == 1u);
            CHECK(Dtor::calls == 0u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(ConstexprTrivial(43));
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(std::string(""));

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<std::string>() == "");

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(42);
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == 0u);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v2 = ::move(v1);

            CHECK(v2.which() == 0u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v1;
            v1.emplace<1>();

            REQUIRE(bool(v1) == true);
            REQUIRE(v1.which() == 1u);

            eggs::variant<Dtor, Throw> v2;
            v2.emplace<0>();

            REQUIRE(bool(v2) == true);
            REQUIRE(v2.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v2 = ::move(v1));

            CHECK(bool(v1) == true);
            CHECK(bool(v2) == false);
            CHECK(v1.which() == 1u);
            CHECK(v2.which() == npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(43);
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // list-initialization
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v = {};

        CHECK(bool(v) == false);
        CHECK(v.which() == npos);
        CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(void));
#endif
    }

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
    // trivially_copyable
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

        v2 = ::move(v1);

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

    v2 = ::move(v1);

    CHECK(bool(v1) == false);
    CHECK(bool(v2) == false);
    CHECK(v2.which() == v1.which());

    // list-initialization
    {
        eggs::variant<> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v = {};

        CHECK(bool(v) == false);
        CHECK(v.which() == npos);
        CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(void));
#endif
    }

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        struct test { static constexpr int call()
        {
            eggs::variant<> v1;
            eggs::variant<> v2;
            v2 = ::move(v1);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}
