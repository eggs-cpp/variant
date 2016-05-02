
**Eggs.Variant** is a **C++11/14** generic, type-safe, discriminated union. See the documentation at http://eggs-cpp.github.io/variant/.

# Contents

- [Introduction](http://eggs-cpp.github.io/variant/introduction.html "Introduction - Eggs.Variant")
- [Design](http://eggs-cpp.github.io/variant/design.html "Design - Eggs.Variant")
- [Reference](http://eggs-cpp.github.io/variant/reference.html "Reference - Eggs.Variant")
- [Config](http://eggs-cpp.github.io/variant/config.html "Config - Eggs.Variant")

# Requirements

The library requires a standard conformant implementation of **C++11**; a few additional features are only available under **C++14**. Some effort was done to support incomplete language or standard library implementations &mdash;see [Config](http://eggs-cpp.github.io/variant/config.html "Config - Eggs.Variant")&mdash;. There are no external dependencies.

The library is continuously tested with the following configurations:
[![Build Status](https://travis-ci.org/eggs-cpp/variant.svg?branch=master)](https://travis-ci.org/eggs-cpp/variant)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/eggs-cpp/variant?branch=master&svg=true)](https://ci.appveyor.com/project/K-ballo/variant)

 - _Clang_ 3.5, 3.6
 - _GCC_ 4.9, 5
 - _Apple LLVM_ 7.0
 - _MSVC_ 2013, 2015

_[Note:_ Older versions of _Clang_ (3.0+) and _GCC_ (4.6+) are no longer tested,
but should still work; please report any issues if you are using them. Older
versions of _MSVC_ are not supported._]_

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2014-2016
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
