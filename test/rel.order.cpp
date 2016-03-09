// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
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
struct has_less
  : std::false_type
{};

template <typename T, typename U>
struct has_less<
    T, U, typename _void<
        decltype(std::declval<T>() < std::declval<U>())
    >::type
> : std::true_type
{};

template <typename T, typename U, typename Enable = void>
struct has_less_equal
  : std::false_type
{};

template <typename T, typename U>
struct has_less_equal<
    T, U, typename _void<
        decltype(std::declval<T>() <= std::declval<U>())
    >::type
> : std::true_type
{};

template <typename T, typename U, typename Enable = void>
struct has_greater
  : std::false_type
{};

template <typename T, typename U>
struct has_greater<
    T, U, typename _void<
        decltype(std::declval<T>() > std::declval<U>())
    >::type
> : std::true_type
{};

template <typename T, typename U, typename Enable = void>
struct has_greater_equal
  : std::false_type
{};

template <typename T, typename U>
struct has_greater_equal<
    T, U, typename _void<
        decltype(std::declval<T>() >= std::declval<U>())
    >::type
> : std::true_type
{};
#endif

TEST_CASE("operator<(variant<Ts...> const&, variant<Ts...> const&)", "[variant.rel]")
{
    // same members
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(43);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        CHECK(v1 < v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(43));
            constexpr bool vltb = v1 < v2;
            constexpr bool vgtb = v1 > v2;
            constexpr bool vlteb = v1 <= v2;
            constexpr bool vgteb = v1 >= v2;
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

        CHECK(v1 < v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(43));
            constexpr bool vltb = v1 < v2;
            constexpr bool vgtb = v1 > v2;
            constexpr bool vlteb = v1 <= v2;
            constexpr bool vgteb = v1 >= v2;
        }
#endif
    }

    // different members
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(std::string(""));

        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<std::string>() == "");

        CHECK(v1 < v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(43));
            constexpr bool vltb = v1 < v2;
            constexpr bool vgtb = v1 > v2;
            constexpr bool vlteb = v1 <= v2;
            constexpr bool vgteb = v1 >= v2;
        }
#endif
    }
}

TEST_CASE("operator<(variant<Ts...> const&, T const&)", "[variant.rel]")
{
    // same members
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(v1 < 43);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool vltb = v1 < Constexpr(43);
            constexpr bool vgtb = v1 > Constexpr(43);
            constexpr bool vlteb = v1 <= Constexpr(43);
            constexpr bool vgteb = v1 >= Constexpr(43);
        }
#endif
    }

    // empty member
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK(v1 < 42);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool vltb = v1 < Constexpr(43);
            constexpr bool vgtb = v1 > Constexpr(43);
            constexpr bool vlteb = v1 <= Constexpr(43);
            constexpr bool vgteb = v1 >= Constexpr(43);
        }
#endif
    }

    // different members
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(v1 < std::string(""));

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr bool vltb = v1 < Constexpr(43);
            constexpr bool vgtb = v1 > Constexpr(43);
            constexpr bool vlteb = v1 <= Constexpr(43);
            constexpr bool vgteb = v1 >= Constexpr(43);
        }
#endif
    }

    // implicit conversion
    {
        eggs::variant<int, std::string> v("42");

        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "42");

        CHECK(v < "43");
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK((
            !has_less<
                eggs::variant<int>, std::string
            >::value));
        CHECK((
            !has_less_equal<
                eggs::variant<int>, std::string
            >::value));
        CHECK((
            !has_greater<
                eggs::variant<int>, std::string
            >::value));
        CHECK((
            !has_greater_equal<
                eggs::variant<int>, std::string
            >::value));
    }
#endif
}

TEST_CASE("operator<(T const&, variant<Ts...> const&)", "[variant.rel]")
{
    // same members
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(41 < v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool vltb = Constexpr(41) < v1;
            constexpr bool vgtb = Constexpr(41) > v1;
            constexpr bool vlteb = Constexpr(41) <= v1;
            constexpr bool vgteb = Constexpr(41) >= v1;
        }
#endif
    }

    // empty member
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK((42 < v1) == false);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool vltb = Constexpr(41) < v1;
            constexpr bool vgtb = Constexpr(41) > v1;
            constexpr bool vlteb = Constexpr(41) <= v1;
            constexpr bool vgteb = Constexpr(41) >= v1;
        }
#endif
    }

    // different members
    {
        eggs::variant<int, std::string> const v1(std::string(""));

        REQUIRE(v1.which() == 1u);
        REQUIRE(*v1.target<std::string>() == "");

        CHECK(42 < v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr bool vltb = Constexpr(41) < v1;
            constexpr bool vgtb = Constexpr(41) > v1;
            constexpr bool vlteb = Constexpr(41) <= v1;
            constexpr bool vgteb = Constexpr(41) >= v1;
        }
#endif
    }

    // implicit conversion
    {
        eggs::variant<int, std::string> v("43");

        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "43");

        CHECK("42" < v);
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK((
            !has_less<
                std::string, eggs::variant<int>
            >::value));
        CHECK((
            !has_less_equal<
                std::string, eggs::variant<int>
            >::value));
        CHECK((
            !has_greater<
                std::string, eggs::variant<int>
            >::value));
        CHECK((
            !has_greater_equal<
                std::string, eggs::variant<int>
            >::value));
    }
#endif
}

TEST_CASE("operator<(variant<> const&, variant<> const&)", "[variant.rel]")
{
    eggs::variant<> const v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> const v2;

    REQUIRE(v2.which() == npos);

    CHECK((v1 < v2) == false);
    CHECK((v2 < v1) == false);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v1;
        constexpr eggs::variant<> v2;
        constexpr bool vltb = v1 < v2;
        constexpr bool vgtb = v1 > v2;
        constexpr bool vlteb = v1 <= v2;
        constexpr bool vgteb = v1 >= v2;
    }
#endif
}
