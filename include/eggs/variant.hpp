//! \file eggs/variant.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_HPP
#define EGGS_VARIANT_HPP

#include "variant/bad_variant_access.hpp"
#include "variant/variant.hpp"

#include <cstddef>

#include "variant/detail/config/prefix.hpp"

namespace eggs
{
    //! using variants::variant;
    using variants::variant;

    //! using variants::variant_size;
    using variants::variant_size;
#if EGGS_CXX14_HAS_VARIABLE_TEMPLATES
    //! using variants::variant_size_v;
    using variants::variant_size_v;
#endif

    //! using variants::variant_element;
    using variants::variant_element;
    //! using variants::variant_element_t;
    using variants::variant_element_t;

    //! constexpr std::size_t variant_npos = std::size_t(-1);
    EGGS_CXX11_CONSTEXPR std::size_t const variant_npos = std::size_t(-1);

    //! using variants::bad_variant_access;
    using variants::bad_variant_access;

    //! using variants::get;
    using variants::get;

    //! using variants::get_if;
    using variants::get_if;

    //! using variants::apply;
    using variants::apply;
}

#include "variant/detail/config/suffix.hpp"

#endif /*EGGS_VARIANT_HPP*/
