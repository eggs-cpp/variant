// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_TEST_DTOR_HPP
#define EGGS_VARIANT_TEST_DTOR_HPP

struct Dtor
{
    static bool called;
    Dtor() = default;
    ~Dtor() { called = true; }
};

bool Dtor::called = false;

#endif /*EGGS_VARIANT_TEST_DTOR_HPP*/
