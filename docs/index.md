
**Eggs.Variant** is a **C++11/14** generic, type-safe, discriminated union. See the documentation at http://eggs-cpp.github.io/variant/.

# Contents

- [Introduction](http://eggs-cpp.github.io/variant/introduction.html "Introduction - Eggs.Variant")
- [Design](http://eggs-cpp.github.io/variant/design.html "Design - Eggs.Variant")
- [Reference](http://eggs-cpp.github.io/variant/reference.html "Reference - Eggs.Variant")
- [Config](http://eggs-cpp.github.io/variant/config.html "Config - Eggs.Variant")

# Requirements

The library requires a standard conformant implementation of **C++11**; a few additional features are only available under **C++14**. Some effort was done to support incomplete language or standard library implementations &mdash;see [Config](http://eggs-cpp.github.io/variant/config.html "Config - Eggs.Variant")&mdash;. There are no external dependencies.

The library was successfully tested with the following configurations [*]:

 - _Clang_ 3.3, 3.4 (both with _libc++_ and _libstdc++_)
 - _GCC_ 4.8, 4.9
 - _MSVC_ 2013

[*] Only _Clang_ with _libc++_ implement enough functionality to support every feature of the library.

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2014-2016
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
