// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <type_traits>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("variant_size<T>", "[variant.helper]")
{
    std::size_t variant_size_0 =
        eggs::variants::variant_size<
            eggs::variant<>
        >::value;
    std::size_t variant_size_1 =
        eggs::variants::variant_size<
            eggs::variant<int>
        >::value;
    std::size_t variant_size_2 =
        eggs::variants::variant_size<
            eggs::variant<int, std::string>
        >::value;

    CHECK(variant_size_0 == 0u);
    CHECK(variant_size_1 == 1u);
    CHECK(variant_size_2 == 2u);

    std::size_t const_variant_size_0 =
        eggs::variants::variant_size<
            eggs::variant<> const
        >::value;

    CHECK(const_variant_size_0 == 0u);
}

TEST_CASE("variant_element<I, T>", "[variant.helper]")
{
    using variant_element_int =
        eggs::variants::variant_element<
            0, eggs::variant<int, std::string>
        >::type;

    using variant_element_string =
        eggs::variants::variant_element<
            1, eggs::variant<int, std::string>
        >::type;

    CHECK((std::is_same<variant_element_int, int>::value));
    CHECK((std::is_same<variant_element_string, std::string>::value));

    using const_variant_element_int =
        eggs::variants::variant_element<
            0, eggs::variant<int, std::string> const
        >::type;

    CHECK((std::is_same<const_variant_element_int, int const>::value));
}
