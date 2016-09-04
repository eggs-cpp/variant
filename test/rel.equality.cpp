// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <cmath>
#include <string>
#include <type_traits>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
template <typename ...Ts>
struct _void
{
    using type = void;
};

template <typename T, typename U, typename Enable = void>
struct has_equal
  : std::false_type
{};

template <typename T, typename U>
struct has_equal<
    T, U, typename _void<
        decltype(std::declval<T>() == std::declval<U>())
    >::type
> : std::true_type
{};

template <typename T, typename U, typename Enable = void>
struct has_not_equal
  : std::false_type
{};

template <typename T, typename U>
struct has_not_equal<
    T, U, typename _void<
        decltype(std::declval<T>() != std::declval<U>())
    >::type
> : std::true_type
{};
#endif

TEST_CASE("operator==(variant<Ts...> const&, variant<Ts...> const&)", "[variant.rel]")
{
    // same members
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 42);

        CHECK((v1 == v2) == true);
        CHECK((v1 != v2) == false);

        // partial order
        {
            eggs::variant<float, std::string> const v(NAN);

            REQUIRE(v.which() == 0u);

            CHECK((v == v) == false);
            CHECK((v != v) == true);
        }

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(42));
            constexpr bool veb = v1 == v2;
            constexpr bool vneb = v1 != v2;
        }
#endif
    }

    // empty member
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        CHECK((v1 == v2) == false);
        CHECK((v1 != v2) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(42));
            constexpr bool veb = v1 == v2;
            constexpr bool vneb = v1 != v2;
        }
#endif
    }

    // different members
    {
        eggs::variant<int, std::string> const v1(std::string(""));

        REQUIRE(v1.which() == 1u);
        REQUIRE(*v1.target<std::string>() == "");

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        CHECK((v1 == v2) == false);
        CHECK((v1 != v2) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(42));
            constexpr bool veb = v1 == v2;
            constexpr bool vneb = v1 != v2;
        }
#endif
    }
}

TEST_CASE("operator==(variant<Ts...> const&, T const&)", "[variant.rel]")
{
    // same members
    {
        eggs::variant<int, std::string> const v(42);

        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 42);

        CHECK((v == 42) == true);
        CHECK((v != 42) == false);

        // partial order
        {
            eggs::variant<float, std::string> const v(NAN);

            REQUIRE(v.which() == 0u);

            CHECK((v == NAN) == false);
            CHECK((v != NAN) == true);
        }

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
            constexpr bool veb = v == Constexpr(42);
            constexpr bool vneb = v != Constexpr(42);
        }
#endif
    }

    // empty member
    {
        eggs::variant<int, std::string> const v;

        REQUIRE(v.which() == npos);

        CHECK((v == 42) == false);
        CHECK((v != 42) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v;
            constexpr bool veb = v == Constexpr(42);
            constexpr bool vneb = v != Constexpr(42);
        }
#endif
    }

    // different members
    {
        eggs::variant<int, std::string> const v(std::string(""));

        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        CHECK((v == 42) == false);
        CHECK((v != 42) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(42);
            constexpr bool veb = v == Constexpr(42);
            constexpr bool vneb = v != Constexpr(42);
        }
#endif
    }

    // implicit conversion
    {
        eggs::variant<int, std::string> v("42");

        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "42");

        CHECK((v == "42") == true);
        CHECK((v != "42") == false);
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK((
            !has_equal<
                eggs::variant<int>, std::string
            >::value));
        CHECK((
            !has_not_equal<
                eggs::variant<int>, std::string
            >::value));
    }
#endif
}

TEST_CASE("operator==(T const&, variant<Ts...> const&)", "[variant.rel]")
{
    // same members
    {
        eggs::variant<int, std::string> const v(42);

        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 42);

        CHECK((42 == v) == true);
        CHECK((42 != v) == false);

        // partial order
        {
            eggs::variant<float, std::string> const v(NAN);

            REQUIRE(v.which() == 0u);

            CHECK((NAN == v) == false);
            CHECK((NAN != v) == true);
        }

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
            constexpr bool veb = Constexpr(42) == v;
            constexpr bool vneb = Constexpr(42) != v;
        }
#endif
    }

    // empty member
    {
        eggs::variant<int, std::string> const v;

        REQUIRE(v.which() == npos);

        CHECK((42 == v) == false);
        CHECK((42 != v) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v;
            constexpr bool veb = Constexpr(42) == v;
            constexpr bool vneb = Constexpr(42) != v;
        }
#endif
    }

    // different members
    {
        eggs::variant<int, std::string> const v(std::string(""));

        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        CHECK((42 == v) == false);
        CHECK((42 != v) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(42);
            constexpr bool veb = Constexpr(42) == v;
            constexpr bool vneb = Constexpr(42) != v;
        }
#endif
    }

    // implicit conversion
    {
        eggs::variant<int, std::string> v("42");

        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "42");

        CHECK(("42" == v) == true);
        CHECK(("42" != v) == false);
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK((
            !has_equal<
                std::string, eggs::variant<int>
            >::value));
        CHECK((
            !has_not_equal<
                std::string, eggs::variant<int>
            >::value));
    }
#endif
}

TEST_CASE("operator==(variant<> const&, variant<> const&)", "[variant.rel]")
{
    eggs::variant<> const v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> const v2;

    REQUIRE(v2.which() == npos);

    CHECK((v1 == v2) == true);
    CHECK((v1 != v2) == false);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v1;
        constexpr eggs::variant<> v2;
        constexpr bool veb = v1 == v2;
        constexpr bool vneb = v1 != v2;
    }
#endif
}
