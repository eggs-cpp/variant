// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

struct HasMemberAddressofOperator
{
    void operator&() const {}
};

struct HasFreeAddressofOperator {};

void operator&(HasFreeAddressofOperator const&) {}

TEST_CASE("variant<T, Fundamental>::target<Fundamental>()", "[variant.obs]")
{
    eggs::variant<int, float> const v(42.f);

    REQUIRE(v.which() == 1u);

    CHECK(v.target<float>() == v.target());

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, float> v(42.f);
        constexpr bool vttb = *v.target<float>() == 42.f;
    }
#endif
}

TEST_CASE("variant<T, Class>::target<Class>()", "[variant.obs]")
{
    eggs::variant<int, std::string> const v(std::string{"42"});

    REQUIRE(v.which() == 1u);

    CHECK(v.target<std::string>() == v.target());

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr bool vttb = v.target<Constexpr>()->x == 42.f;
    }
#endif
}

TEST_CASE("variant<T, HasMemberAddressofOperator>::target<HasMemberAddressofOperator>()", "[variant.obs]")
{
    eggs::variant<int, HasMemberAddressofOperator> const v(HasMemberAddressofOperator{});

    REQUIRE(v.which() == 1u);

    CHECK(v.target<HasMemberAddressofOperator>() == v.target());
}

TEST_CASE("variant<T, HasFreeAddressofOperator>::target<HasFreeAddressofOperator>()", "[variant.obs]")
{
    eggs::variant<int, HasFreeAddressofOperator> const v(HasFreeAddressofOperator{});

    REQUIRE(v.which() == 1u);

    CHECK(v.target<HasFreeAddressofOperator>() == v.target());
}
