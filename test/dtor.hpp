// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_TEST_DTOR_HPP
#define EGGS_VARIANT_TEST_DTOR_HPP

#include <cstddef>

struct Dtor
{
    static std::size_t calls;
    Dtor() = default;
    ~Dtor() { ++calls; }
};

std::size_t Dtor::calls = 0u;

void swap(Dtor&, Dtor&) {}

#endif /*EGGS_VARIANT_TEST_DTOR_HPP*/
