// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

using eggs::variants::nullvariant;

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("operator<(variant<Ts...> const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(43);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        CHECK(v1 < v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
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

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        CHECK(v1 < v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
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

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(std::string{""});

        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<std::string>() == "");

        CHECK(v1 < v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
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
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(v1 < 43);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool vltb = v1 < Constexpr(43);
            constexpr bool vgtb = v1 > Constexpr(43);
            constexpr bool vlteb = v1 <= Constexpr(43);
            constexpr bool vgteb = v1 >= Constexpr(43);
        }
#endif
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK(v1 < 42);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool vltb = v1 < Constexpr(43);
            constexpr bool vgtb = v1 > Constexpr(43);
            constexpr bool vlteb = v1 <= Constexpr(43);
            constexpr bool vgteb = v1 >= Constexpr(43);
        }
#endif
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(v1 < std::string{""});

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr bool vltb = v1 < Constexpr(43);
            constexpr bool vgtb = v1 > Constexpr(43);
            constexpr bool vlteb = v1 <= Constexpr(43);
            constexpr bool vgteb = v1 >= Constexpr(43);
        }
#endif
    }
}

TEST_CASE("operator<(T const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(41 < v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool vltb = Constexpr(41) < v1;
            constexpr bool vgtb = Constexpr(41) > v1;
            constexpr bool vlteb = Constexpr(41) <= v1;
            constexpr bool vgteb = Constexpr(41) >= v1;
        }
#endif
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK((42 < v1) == false);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool vltb = Constexpr(41) < v1;
            constexpr bool vgtb = Constexpr(41) > v1;
            constexpr bool vlteb = Constexpr(41) <= v1;
            constexpr bool vgteb = Constexpr(41) >= v1;
        }
#endif
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(std::string{""});

        REQUIRE(v1.which() == 1u);
        REQUIRE(*v1.target<std::string>() == "");

        CHECK(42 < v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr bool vltb = Constexpr(41) < v1;
            constexpr bool vgtb = Constexpr(41) > v1;
            constexpr bool vlteb = Constexpr(41) <= v1;
            constexpr bool vgteb = Constexpr(41) >= v1;
        }
#endif
    }
}

TEST_CASE("operator<(variant<Ts...> const&, nullvariant_t)", "[variant.rel]")
{
    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK((v1 < nullvariant) == false);
        CHECK(v1 <= nullvariant);
        CHECK((v1 > nullvariant) == false);
        CHECK(v1 >= nullvariant);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool vltb = v1 < nullvariant;
            constexpr bool vgtb = v1 > nullvariant;
            constexpr bool vlteb = v1 <= nullvariant;
            constexpr bool vgteb = v1 >= nullvariant;
        }
#endif
    }

    SECTION("non-empty members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK((v1 < nullvariant) == false);
        CHECK((v1 <= nullvariant) == false);
        CHECK(v1 > nullvariant);
        CHECK(v1 >= nullvariant);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool vltb = v1 < nullvariant;
            constexpr bool vgtb = v1 > nullvariant;
            constexpr bool vlteb = v1 <= nullvariant;
            constexpr bool vgteb = v1 >= nullvariant;
        }
#endif
    }
}

TEST_CASE("operator<(nullvariant_t, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK((nullvariant < v1) == false);
        CHECK(nullvariant <= v1);
        CHECK((nullvariant > v1) == false);
        CHECK(nullvariant >= v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool vltb = nullvariant < v1;
            constexpr bool vgtb = nullvariant > v1;
            constexpr bool vlteb = nullvariant <= v1;
            constexpr bool vgteb = nullvariant >= v1;
        }
#endif
    }

    SECTION("non-empty members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(nullvariant < v1);
        CHECK(nullvariant <= v1);
        CHECK((nullvariant > v1) == false);
        CHECK((nullvariant >= v1) == false);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool vltb = nullvariant < v1;
            constexpr bool vgtb = nullvariant > v1;
            constexpr bool vlteb = nullvariant <= v1;
            constexpr bool vgteb = nullvariant >= v1;
        }
#endif
    }
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
    SECTION("constexpr")
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
