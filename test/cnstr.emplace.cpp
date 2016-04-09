// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <typeinfo>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

TEST_CASE("variant<Ts...>::variant(in_place<I>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(eggs::variants::in_place<0>, 42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(int));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(eggs::variants::in_place<1>, 42);
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr bool vttb = v.target<Constexpr>()->x == 42;

#  if EGGS_CXX98_HAS_RTTI
        constexpr std::type_info const& vtt = v.target_type();
#  endif

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(eggs::variants::in_place<1>, 42);
            v.target<Constexpr>()->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

TEST_CASE("variant<T, T>::variant(in_place<I>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, int> v(eggs::variants::in_place<0>, 42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    REQUIRE(v.target() != nullptr);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(int));
#endif
}

#if EGGS_CXX11_HAS_INITIALIZER_LIST_OVERLOADING
TEST_CASE("variant<Ts...>::variant(in_place<I>, std::initializer_list<U>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(eggs::variants::in_place<1>, {'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    REQUIRE(v.target<std::string>() != nullptr);
    CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(eggs::variants::in_place<1>, {4, 2});
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr bool vttb = v.target<Constexpr>()->x == 4;

#  if EGGS_CXX98_HAS_RTTI
        constexpr std::type_info const& vtt = v.target_type();
#  endif

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(eggs::variants::in_place<1>, {4, 2});
            v.target<Constexpr>()->x = 5;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

TEST_CASE("variant<T, T>::variant(in_place<I>, std::initializer_list<U>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<std::string, std::string> v(eggs::variants::in_place<1>, {'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    REQUIRE(v.target() != nullptr);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(std::string));
#endif
}
#endif

TEST_CASE("variant<Ts...>::variant(in_place<T>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(eggs::variants::in_place<int>, 42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(int));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(eggs::variants::in_place<Constexpr>, 42);
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr bool vttb = v.target<Constexpr>()->x == 42;

#  if EGGS_CXX98_HAS_RTTI
        constexpr std::type_info const& vtt = v.target_type();
#  endif

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(eggs::variants::in_place<Constexpr>, 42);
            v.target<Constexpr>()->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

#if EGGS_CXX11_HAS_INITIALIZER_LIST_OVERLOADING
TEST_CASE("variant<Ts...>::variant(in_place<T>, std::initializer_list<U>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(eggs::variants::in_place<std::string>, {'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    REQUIRE(v.target<std::string>() != nullptr);
    CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(eggs::variants::in_place<Constexpr>, {4, 2});
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr bool vttb = v.target<Constexpr>()->x == 4;

#  if EGGS_CXX98_HAS_RTTI
        constexpr std::type_info const& vtt = v.target_type();
#  endif

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(eggs::variants::in_place<Constexpr>, {4, 2});
            v.target<Constexpr>()->x = 5;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}
#endif
