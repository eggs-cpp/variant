// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

TEST_CASE("get_if<I>(variant<Ts...>*)", "[variant.elem]")
{
    // no source
    {
        eggs::variant<int, std::string> *v = nullptr;

        int* ptr = eggs::variants::get_if<0>(v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<0>(v)) == true));
    }

    // same source
    {
        eggs::variant<int, std::string> v(42);

        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 42);

        int* ptr = eggs::variants::get_if<0>(&v);

        REQUIRE(ptr != nullptr);
        CHECK(*ptr == 42);

        CHECK((noexcept(eggs::variants::get_if<0>(&v)) == true));
    }

    // different source
    {
        eggs::variant<int, std::string> v(42);

        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 42);

        std::string* ptr = eggs::variants::get_if<1>(&v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<1>(&v)) == true));
    }

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            eggs::variants::get_if<1>(&v)->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

TEST_CASE("get_if<I>(variant<T, T>*)", "[variant.elem]")
{
    eggs::variant<int, int> v(eggs::variants::in_place<0>, 42);

    REQUIRE(v.which() == 0u);
    REQUIRE(v.target() != nullptr);

    int* ptr = eggs::variants::get_if<0>(&v);

    REQUIRE(ptr != nullptr);
    CHECK(*ptr == 42);
}

TEST_CASE("get_if<I>(variant<Ts...> const*)", "[variant.elem]")
{
    // no source
    {
        eggs::variant<int, std::string> *v = nullptr;

        int const* ptr = eggs::variants::get_if<0>(v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<0>(v)) == true));
    }

    // same source
    {
        eggs::variant<int, std::string> const v(42);

        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 42);

        int const* ptr = eggs::variants::get_if<0>(&v);

        REQUIRE(ptr != nullptr);
        CHECK(*ptr == 42);

        CHECK((noexcept(eggs::variants::get_if<0>(&v)) == true));
    }

    // different source
    {
        eggs::variant<int, std::string> v(42);

        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 42);

        std::string const* ptr = eggs::variants::get_if<1>(&v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<1>(&v)) == true));
    }

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        static constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr bool vgb = eggs::variants::get_if<1>(&v)->x == 42;
    }
#endif
}

TEST_CASE("get_if<I>(variant<T, T> const*)", "[variant.elem]")
{
    eggs::variant<int, int> const v(eggs::variants::in_place<0>, 42);

    REQUIRE(v.which() == 0u);
    REQUIRE(v.target() != nullptr);

    int const* ptr = eggs::variants::get_if<0>(&v);

    REQUIRE(ptr != nullptr);
    CHECK(*ptr == 42);
}

TEST_CASE("get_if<T>(variant<Ts...>*)", "[variant.elem]")
{
    // no source
    {
        eggs::variant<int, std::string> *v = nullptr;

        int* ptr = eggs::variants::get_if<int>(v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<int>(v)) == true));
    }

    // same source
    {
        eggs::variant<int, std::string> v(42);

        int* ptr = eggs::variants::get_if<int>(&v);

        REQUIRE(ptr != nullptr);
        CHECK(*ptr == 42);

        CHECK((noexcept(eggs::variants::get_if<int>(&v)) == true));
    }

    // different source
    {
        eggs::variant<int, std::string> v(42);

        std::string* ptr = eggs::variants::get_if<std::string>(&v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<std::string>(&v)) == true));
    }

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            eggs::variants::get_if<Constexpr>(&v)->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

TEST_CASE("get_if<T>(variant<Ts...> const*)", "[variant.elem]")
{
    // no source
    {
        eggs::variant<int, std::string> *v = nullptr;

        int const* ptr = eggs::variants::get_if<int>(v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<int>(v)) == true));
    }

    // same source
    {
        eggs::variant<int, std::string> const v(42);

        int const* ptr = eggs::variants::get_if<int>(&v);

        REQUIRE(ptr != nullptr);
        CHECK(*ptr == 42);

        CHECK((noexcept(eggs::variants::get_if<int>(&v)) == true));
    }

    // different source
    {
        eggs::variant<int, std::string> v(42);

        std::string const* ptr = eggs::variants::get_if<std::string>(&v);

        CHECK(ptr == nullptr);

        CHECK((noexcept(eggs::variants::get_if<std::string>(&v)) == true));
    }

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        static constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr bool vgb = eggs::variants::get_if<Constexpr>(&v)->x == 42;
    }
#endif
}
