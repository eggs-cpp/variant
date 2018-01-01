// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>
#include <eggs/variant/detail/utility.hpp>

using eggs::variants::detail::move;

#include "catch.hpp"
#include "constexpr.hpp"

struct Maleficent
{
    int x;

    Maleficent(int i) : x(i) {}
    template <typename T>
    Maleficent(T&&) : x(-1) {}
    ~Maleficent() {} // not trivially copyable
};

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
struct MovableOnly
{
    MovableOnly() {};
    MovableOnly(MovableOnly&&) {};
    MovableOnly& operator=(MovableOnly&&) { return *this; }
};

struct NonCopyConstructible
{
    NonCopyConstructible() {}
    NonCopyConstructible(NonCopyConstructible const&) = delete;
    NonCopyConstructible& operator=(NonCopyConstructible const&) { return *this; }; // not trivially copyable
};

struct NonCopyConstructibleTrivial
{
    NonCopyConstructibleTrivial() {}
    NonCopyConstructibleTrivial(NonCopyConstructibleTrivial const&) = delete;
    NonCopyConstructibleTrivial& operator=(NonCopyConstructibleTrivial const&) = default;
};

#  if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE || !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_copyable<NonCopyConstructibleTrivial> : std::true_type {};
    template <> struct is_trivially_destructible<NonCopyConstructibleTrivial> : std::true_type {};
}}}
#  endif
#endif

TEST_CASE("variant<Ts...>::variant(variant<Ts...> const&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> const v1(42);

    REQUIRE(bool(v1) == true);
    REQUIRE(v1.which() == 0u);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<int, std::string> v2(v1);

    CHECK(bool(v2) == true);
    CHECK(v2.which() == v1.which());
    REQUIRE(v1.target<int>() != nullptr);
    CHECK(*v1.target<int>() == 42);
    REQUIRE(v2.target<int>() != nullptr);
    CHECK(*v2.target<int>() == 42);

    // trivially_copyable
    {
        eggs::variant<int, float> const v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
        CHECK(std::is_trivially_copyable<decltype(v1)>::value == true);
#endif

        eggs::variant<int, float> v2(v1);

        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
        constexpr eggs::variant<int, ConstexprTrivial> v2(v1);
    }
#endif

    // sfinae
    {
        eggs::variant<Maleficent> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(v1.target<Maleficent>()->x == 42);

        eggs::variant<Maleficent> v2(v1);

        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v1.target<Maleficent>() != nullptr);
        CHECK(v1.target<Maleficent>()->x == 42);
        REQUIRE(v2.target<Maleficent>() != nullptr);
        CHECK(v2.target<Maleficent>()->x == 42);

        eggs::variant<Maleficent> const& vc1 = v1;
        eggs::variant<Maleficent> v3(::move(vc1));

        CHECK(bool(v3) == true);
        CHECK(v3.which() == v1.which());
        REQUIRE(v1.target<Maleficent>() != nullptr);
        CHECK(v1.target<Maleficent>()->x == 42);
        REQUIRE(v3.target<Maleficent>() != nullptr);
        CHECK(v3.target<Maleficent>()->x == 42);

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
        CHECK(
            !std::is_copy_constructible<
                eggs::variant<MovableOnly>
            >::value);
        CHECK(
            !std::is_copy_constructible<
                eggs::variant<NonCopyConstructible>
            >::value);
        CHECK(
            !std::is_copy_constructible<
                eggs::variant<NonCopyConstructibleTrivial>
            >::value);
#endif
    }
}

TEST_CASE("variant<>::variant(variant<> const&)", "[variant.cnstr]")
{
    eggs::variant<> const v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == eggs::variant_npos);

    eggs::variant<> v2(v1);

    CHECK(bool(v2) == false);
    CHECK(v2.which() == v1.which());

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v1;
        constexpr eggs::variant<> v2(v1);
    }
#endif
}
