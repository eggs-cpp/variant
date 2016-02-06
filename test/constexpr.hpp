// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_TEST_CONSTEXPR_HPP
#define EGGS_VARIANT_TEST_CONSTEXPR_HPP

#include <initializer_list>

#include <eggs/variant/detail/config/prefix.hpp>

#if EGGS_CXX11_HAS_CONSTEXPR
struct Constexpr
{
    int x;

    constexpr Constexpr() : x(0) {}
    constexpr Constexpr(int i) : x(i) {}
#  if EGGS_CXX14_HAS_CONSTEXPR
    constexpr Constexpr(std::initializer_list<int> is) : x(*is.begin()) {}
#  endif

    constexpr Constexpr(Constexpr const& rhs) : x(rhs.x) {} // not trivially copyable

    constexpr bool operator==(Constexpr rhs) const { return x == rhs.x; }
    constexpr bool operator<(Constexpr rhs) const { return x < rhs.x; }
};

struct ConstexprTrivial
{
    constexpr ConstexprTrivial() {}
    constexpr ConstexprTrivial(int) {}
#  if EGGS_CXX14_HAS_CONSTEXPR
    constexpr ConstexprTrivial(std::initializer_list<int>) {}
#  endif
};

namespace std
{
#if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
    template <> struct is_pod<Constexpr> : true_type {};
    template <> struct is_pod<ConstexprTrivial> : true_type {};
#elif !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
    template <> struct is_pod<ConstexprTrivial> : true_type {};
#endif
}
#endif

#endif /*EGGS_VARIANT_TEST_CONSTEXPR_HPP*/
