# Config

Minimal support for broken or incomplete implementations is attained by littering the code with macros. The library uses these macros to downgrade or remove functionality that would otherwise result in a compilation error.

Macro                                          | Replacement             | Fallback
:--------------------------------------------- | :---------------------: | :-------------:
`EGGS_CXX11_HAS_CONSTEXPR`                     | `1`                     | `0`
`EGGS_CXX11_CONSTEXPR`                         | `constexpr`             | ``
`EGGS_CXX11_STATIC_CONSTEXPR`                  | `static constexpr`      | `static const`
`EGGS_CXX14_HAS_CONSTEXPR`                     | `1`                     | `0`
`EGGS_CXX14_CONSTEXPR`                         | `constexpr`             | ``
`EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS`           | `1`                     | `0`
`EGGS_CXX11_HAS_DELETED_FUNCTIONS`             | `1`                     | `0`
`EGGS_CXX98_HAS_RTTI`                          | `1`                     | `0`
`EGGS_CXX98_HAS_EXCEPTIONS`                    | `1`                     | `0`
`EGGS_CXX11_NOEXCEPT`                          | `noexcept`              | ``
`EGGS_CXX11_NOEXCEPT_IF(...)`                  | `noexcept(__VA_ARGS__)` | ``
`EGGS_CXX11_NOEXCEPT_EXPR(...)`                | `noexcept(__VA_ARGS__)` | `false`
`EGGS_CXX11_NORETURN`                          | `[[noreturn]]`          | ``
`EGGS_CXX11_HAS_INITIALIZER_LIST_OVERLOADING`  | `1`                     | `0`
`EGGS_CXX11_HAS_TEMPLATE_ARGUMENT_OVERLOADING` | `1`                     | `0`
`EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS`        | `1`                     | `0`
`EGGS_CXX11_HAS_UNRESTRICTED_UNIONS`           | `1`                     | `0`
`EGGS_CXX14_HAS_VARIABLE_TEMPLATES`            | `1`                     | `0`
`EGGS_CXX11_STD_HAS_ALIGNED_UNION`             | `1`                     | `0`
`EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS`         | `1`                     | `0`
`EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE`     | `1`                     | `0`
`EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE` | `1`                     | `0`

The macros are defined to their corresponding _replacement_, except for known incomplete implementations where they are defined to their corresponding _fallback_ instead. These macros can be overriden by the user by defining them before including any library header.

_[Note:_ The configuration macros are not part of the public interface of the library, and are not leaked into user code._]_

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2014-2016
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
