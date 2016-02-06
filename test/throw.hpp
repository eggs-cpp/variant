// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_TEST_THROW_HPP
#define EGGS_VARIANT_TEST_THROW_HPP

#include <initializer_list>

#include <eggs/variant/detail/config/prefix.hpp>

#if EGGS_CXX98_HAS_EXCEPTIONS
struct Throw
{
    Throw() = default;
    Throw(int) { throw 0; }
    Throw(std::initializer_list<int>) { throw 0; }
    Throw(Throw const&) { throw 0; }
    Throw(Throw&&) { throw 0; }
    Throw& operator=(Throw const&) { throw 0; }
    Throw& operator=(Throw&&) { throw 0; }
    ~Throw() = default;
};
#endif

#endif /*EGGS_VARIANT_TEST_THROW_HPP*/
