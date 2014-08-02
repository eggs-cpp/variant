**Eggs.Variant**
==================

# Introduction #

**Eggs.Variant** is a **C++14** type-safe, generic, discriminated union.

# Synopsis #

	namespace eggs { namespace variants {
	  // variant for object types
	  template <class ...Ts> class variant;

	  // In-place construction
	  struct in_place_t {};
	  template <std::size_t I>
	  in_place_t in_place(unspecified<I>);
	  template <class T>
	  in_place_t in_place(unspecified<T>);

	  // No-active-member state indicator
	  struct nullvariant_t {see below};
	  constexpr nullvariant_t nullvariant(unspecified);

	  // Class bad_variant_access
	  class bad_variant_access;

	  // Class template variant
	  template <class ...Ts>
	  class variant {
	  public:
		static constexpr std::size_t npos = std::size_t(-1);

		// Constructors
		variant() noexcept;
		variant(nullvariant_t) noexcept;
		variant(variant const&);
		variant(variant&&) noexcept(see below);
		template <class U>
		variant(U&&);
		template <std::size_t I, class ...Args>
		explicit variant(unspecified<I>, Args&&...);
		template <std::size_t I, class U, class ...Args>
		explicit variant(unspecified<I>, std::initializer_list<U>, Args&&...);
		template <class T, class ...Args>
		explicit variant(unspecified<T>, Args&&...);
		template <class T, class U, class ...Args>
		explicit variant(unspecified<T>, std::initializer_list<U>, Args&&...);

		// Destructor
		~variant();

		// Assignment
		variant& operator=(nullvariant_t) noexcept;
		variant& operator=(variant const&);
		variant& operator=(variant&&) noexcept(see below);
		template <class U>
		variant& operator=(U&&);
		template <std::size_t I, class ...Args>
		void emplace(Args&&...);
		template <std::size_t I, class U, class ...Args>
		void emplace(std::initializer_list<U>, Args&&...);
		template <class T, class ...Args>
		void emplace(Args&&...);
		template <class T, class U, class ...Args>
		void emplace(std::initializer_list<U>, Args&&...);

		// Swap
		void swap(variant&) noexcept(see below);

		// Observers
		explicit operator bool() const noexcept;
		std::size_t which() const noexcept;
		std::type_info const& target_type() const noexcept;
		void* target() noexcept;
		void const* target() const noexcept;
		template <class T>
		T* target() noexcept;
		template <class T>
		T const* target() const noexcept;
	  };

	  // Class bad_variant_access
	  class bad_variant_access : public std::logic_error {
	  public:
		bad_variant_access();
		explicit bad_variant_access(std::string const&);
		explicit bad_variant_access(char const*);
	  };

	  // Helper classes
	  template <class T>
	  struct variant_size; // undefined
	  template <class ...Ts>
	  struct variant_size<variant<Ts...>>;
	  template <class T>
	  struct variant_size<T const>;

	  template <class T>
	  constexpr std::size_t variant_size_v = variant_size<T>::value;

	  template <std::size_t I, class T>
	  struct variant_element; // undefined
	  template <std::size_t I, class ...Ts>
	  struct variant_element<I, variant<Ts...>>;
	  template <std::size_t I, class T>
	  struct variant_element<I, T const>;

	  template <std::size_t I, class T>
	  using variant_element_t =  class variant_element<I, T>::type;

	  // Element access
	  template <std::size_t I, class ...Ts>
	  variant_element_t<I, variant<Ts...>>& get(variant<Ts...>&);
	  template <std::size_t I, class ...Ts>
	  variant_element_t<I, variant<Ts...>> const& get(variant<Ts...> const&);
	  template <std::size_t I, class ...Ts>
	  variant_element_t<I, variant<Ts...>>&& get(variant<Ts...>&&);
	  template <class T, class ...Ts>
	  T& get(variant<Ts...>&);
	  template <class T, class ...Ts>
	  T const& get(variant<Ts...> const&);
	  template <class T, class ...Ts>
	  T&& get(variant<Ts...>&&);

	  // Relational operators
	  template <class ...Ts>
	  bool operator==(variant<Ts...> const&, variant<Ts...> const&);
	  template <class ...Ts>
	  bool operator!=(variant<Ts...> const&, variant<Ts...> const&);
	  template <class ...Ts>
	  bool operator<(variant<Ts...> const&, variant<Ts...> const&);
	  template <class ...Ts>
	  bool operator>(variant<Ts...> const&, variant<Ts...> const&);
	  template <class ...Ts>
	  bool operator<=(variant<Ts...> const&, variant<Ts...> const&);
	  template <class ...Ts>
	  bool operator>=(variant<Ts...> const&, variant<Ts...> const&);

	  // Comparison with nullvariant
	  template <class ...Ts>
	  bool operator==(variant<Ts...> const&, nullvariant_t) noexcept;
	  template <class ...Ts>
	  bool operator==(nullvariant_t, variant<Ts...> const&) noexcept;
	  template <class ...Ts>
	  bool operator!=(variant<Ts...> const&, nullvariant_t) noexcept;
	  template <class ...Ts>
	  bool operator!=(nullvariant_t, variant<Ts...> const&) noexcept;
	  template <class ...Ts>
	  bool operator<(variant<Ts...> const&, nullvariant_t) noexcept;
	  template <class ...Ts>
	  bool operator<(nullvariant_t, variant<Ts...> const&) noexcept;
	  template <class ...Ts>
	  bool operator>(variant<Ts...> const&, nullvariant_t) noexcept;
	  template <class ...Ts>
	  bool operator>(nullvariant_t, variant<Ts...> const&) noexcept;
	  template <class ...Ts>
	  bool operator<=(variant<Ts...> const&, nullvariant_t) noexcept;
	  template <class ...Ts>
	  bool operator<=(nullvariant_t, variant<Ts...> const&) noexcept;
	  template <class ...Ts>
	  bool operator>=(variant<Ts...> const&, nullvariant_t) noexcept;
	  template <class ...Ts>
	  bool operator>=(nullvariant_t, variant<Ts...> const&) noexcept;

	  // Comparison with Ts
	  template <class ...Ts, class T>
	  bool operator==(variant<Ts...> const&, T const&);
	  template <class T, class ...Ts>
	  bool operator==(T const&, variant<Ts...> const&);
	  template <class ...Ts, class T>
	  bool operator!=(variant<Ts...> const&, T const&);
	  template <class T, class ...Ts>
	  bool operator!=(T const&, variant<Ts...> const&);
	  template <class ...Ts, class T>
	  bool operator<(variant<Ts...> const&, T const&);
	  template <class T, class ...Ts>
	  bool operator<(T const&, variant<Ts...> const&);
	  template <class ...Ts, class T>
	  bool operator>(variant<Ts...> const&, T const&);
	  template <class T, class ...Ts>
	  bool operator>(T const&, variant<Ts...> const&);
	  template <class ...Ts, class T>
	  bool operator<=(variant<Ts...> const&, T const&);
	  template <class T, class ...Ts>
	  bool operator<=(T const&, variant<Ts...> const&);
	  template <class ...Ts, class T>
	  bool operator>=(variant<Ts...> const&, T const&);
	  template <class T, class ...Ts>
	  bool operator>=(T const&, variant<Ts...> const&);

	  // Calling a function with variants of arguments
	  template <class R, class F, class ...Vs>
	  R apply(F&&, Vs&&...);
	  template <class F, class ...Vs>
	  see below apply(F&&, Vs&&...);

	  // Specialized algorithms
	  template <class ...Ts>
	  void swap(variant<Ts...>&, variant<Ts...>&) noexcept(see below);
	}}

	// Hash support
	namespace std {
	  template <class ...Ts>
	  struct hash<::eggs::variants::variant<Ts...>>;
	}

	namespace eggs {
	  using variants::variant;
	}

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2014
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
