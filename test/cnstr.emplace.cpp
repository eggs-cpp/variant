// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>

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

    // sfinae
    {
        CHECK((
            !std::is_constructible<
                eggs::variant<int>,
                eggs::variants::in_place_index_t<0>, std::string
            >::value));
        CHECK((
            !std::is_constructible<
                eggs::variant<int>,
                eggs::variants::in_place_index_t<1>
            >::value));
    }
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

    // sfinae
    {
        CHECK((
            !std::is_constructible<
                eggs::variant<std::string>,
                eggs::variants::in_place_index_t<0>, std::initializer_list<int>
            >::value));
        CHECK((
            !std::is_constructible<
                eggs::variant<std::string>,
                eggs::variants::in_place_index_t<1>, std::initializer_list<int>
            >::value));
    }
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

    // sfinae
    {
        CHECK((
            !std::is_constructible<
                eggs::variant<int>,
                eggs::variants::in_place_type_t<int>, std::string
            >::value));
        CHECK((
            !std::is_constructible<
                eggs::variant<int, int>,
                eggs::variants::in_place_type_t<int>
            >::value));
        CHECK((
            !std::is_constructible<
                eggs::variant<int, int const>,
                eggs::variants::in_place_type_t<int>
            >::value));
    }
}

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

    // sfinae
    {
        CHECK((
            !std::is_constructible<
                eggs::variant<std::string>,
                eggs::variants::in_place_type_t<std::string>, std::initializer_list<int>
            >::value));
        CHECK((
            !std::is_constructible<
                eggs::variant<std::string, std::string>,
                eggs::variants::in_place_type_t<std::string>, std::initializer_list<int>
            >::value));
        CHECK((
            !std::is_constructible<
                eggs::variant<std::string, std::string const>,
                eggs::variants::in_place_type_t<std::string>, std::initializer_list<int>
            >::value));
    }
}
