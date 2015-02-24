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

using eggs::variants::nullvariant;

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("operator==(variant<Ts...> const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 42);

        CHECK(v1 == v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(42));
            constexpr bool veb = v1 == v2;
            constexpr bool vneb = v1 != v2;
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

        CHECK(v1 != v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr eggs::variant<int, Constexpr> v2(Constexpr(42));
            constexpr bool veb = v1 == v2;
            constexpr bool vneb = v1 != v2;
        }
#endif
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(std::string{""});

        REQUIRE(v1.which() == 1u);
        REQUIRE(*v1.target<std::string>() == "");

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        CHECK(v1 != v2);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
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
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(v1 == 42);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool veb = v1 == Constexpr(42);
            constexpr bool vneb = v1 != Constexpr(42);
        }
#endif
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK(v1 != 42);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool veb = v1 == Constexpr(42);
            constexpr bool vneb = v1 != Constexpr(42);
        }
#endif
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(std::string{""});

        REQUIRE(v1.which() == 1u);
        REQUIRE(*v1.target<std::string>() == "");

        CHECK(v1 != 42);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr bool veb = v1 == Constexpr(42);
            constexpr bool vneb = v1 != Constexpr(42);
        }
#endif
    }
}

TEST_CASE("operator==(T const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(42 == v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool veb = Constexpr(42) == v1;
            constexpr bool vneb = Constexpr(42) != v1;
        }
#endif
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK(42 != v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool veb = Constexpr(42) == v1;
            constexpr bool vneb = Constexpr(42) != v1;
        }
#endif
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(std::string{""});

        REQUIRE(v1.which() == 1u);
        REQUIRE(*v1.target<std::string>() == "");

        CHECK(42 != v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(42);
            constexpr bool veb = Constexpr(42) == v1;
            constexpr bool vneb = Constexpr(42) != v1;
        }
#endif
    }
}

TEST_CASE("operator==(variant<Ts...> const&, nullvariant_t)", "[variant.rel]")
{
    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK(v1 == nullvariant);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool veb = v1 == nullvariant;
            constexpr bool vneb = v1 != nullvariant;
        }
#endif
    }

    SECTION("non-empty members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(v1 != nullvariant);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool veb = v1 == nullvariant;
            constexpr bool vneb = v1 != nullvariant;
        }
#endif
    }
}

TEST_CASE("operator==(nullvariant_t, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        CHECK(nullvariant == v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1;
            constexpr bool veb = nullvariant == v1;
            constexpr bool vneb = nullvariant != v1;
        }
#endif
    }

    SECTION("non-empty members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(nullvariant != v1);

#if EGGS_CXX11_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
            constexpr bool veb = nullvariant == v1;
            constexpr bool vneb = nullvariant != v1;
        }
#endif
    }
}

TEST_CASE("operator==(variant<> const&, variant<> const&)", "[variant.rel]")
{
    eggs::variant<> const v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> const v2;

    REQUIRE(v2.which() == npos);

    CHECK(v1 == v2);

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<> v1;
        constexpr eggs::variant<> v2;
        constexpr bool veb = v1 == v2;
        constexpr bool vneb = v1 != v2;
    }
#endif
}
