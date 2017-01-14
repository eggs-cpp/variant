# Eggs.Variant
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)

project(Eggs.Variant CXX)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  option(WITH_TESTS "Compile with unit tests." ON)
  set(HAVE_TESTS ${WITH_TESTS})
else()
  option(EGGS_VARIANT_WITH_TESTS "Compile with unit tests." ON)
  set(HAVE_TESTS ${EGGS_VARIANT_WITH_TESTS})
endif()

# Build
add_library(_eggs_variant INTERFACE)
target_include_directories(_eggs_variant INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
set_target_properties(_eggs_variant
  PROPERTIES EXPORT_NAME Eggs::Variant)

add_library(Eggs::Variant ALIAS _eggs_variant)

# Test
if (HAVE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

# Install
set(_headers
  eggs/variant.hpp
  eggs/variant/bad_variant_access.hpp
  eggs/variant/in_place.hpp
  eggs/variant/variant.hpp
  eggs/variant/detail/apply.hpp
  eggs/variant/detail/pack.hpp
  eggs/variant/detail/storage.hpp
  eggs/variant/detail/utility.hpp
  eggs/variant/detail/visitor.hpp
  eggs/variant/detail/config/prefix.hpp
  eggs/variant/detail/config/suffix.hpp)
foreach (_header ${_headers})
  get_filename_component(_directory "${_header}" DIRECTORY)
  install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${_header}"
    DESTINATION "include/${_directory}")
endforeach()

install(TARGETS _eggs_variant EXPORT _targets)
install(EXPORT _targets
  DESTINATION lib/cmake/eggs.variant
  FILE eggs.variant-targets.cmake)

include(CMakePackageConfigHelpers)

configure_file(
  cmake/eggs.variant-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/eggs.variant-config.cmake"
  @ONLY)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/eggs.variant-config-version.cmake"
  VERSION 0.0.0
  COMPATIBILITY AnyNewerVersion)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/eggs.variant-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/eggs.variant-config-version.cmake"
  DESTINATION lib/cmake/eggs.variant)
