# C++ skeleton for Bison

# Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008
# Free Software Foundation, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


## --------- ##
## variant.  ##
## --------- ##

# b4_symbol_variant(YYTYPE, YYVAL, ACTION, [ARGS])
# ------------------------------------------------
# Run some ACTION ("build", or "destroy") on YYVAL of symbol type
# YYTYPE.
m4_define([b4_symbol_variant],
[m4_pushdef([b4_dollar_dollar],
            [$2.$3< $][3 >(m4_shift3($@))])dnl
  switch ($1)
    {
b4_type_foreach([b4_type_action_])[]dnl
      default:
        break;
    }
m4_popdef([b4_dollar_dollar])dnl
])


# _b4_char_sizeof_counter
# -----------------------
# A counter used by _b4_char_sizeof_dummy to create fresh symbols.
m4_define([_b4_char_sizeof_counter],
[0])

# _b4_char_sizeof_dummy
# ---------------------
# At each call return a new C++ identifier.
m4_define([_b4_char_sizeof_dummy],
[m4_define([_b4_char_sizeof_counter], m4_incr(_b4_char_sizeof_counter))dnl
dummy[]_b4_char_sizeof_counter])


# b4_char_sizeof_(SYMBOL-NUM)
# ---------------------------
# A comment describing this symbol.
m4_define([b4_char_sizeof_],
[      // b4_symbol([$1], [tag])
])

# b4_char_sizeof(SYMBOL-NUMS)
# ---------------------------
# To be mapped on the list of type names to produce:
#
#    char dummy1[sizeof(type_name_1)];
#    char dummy2[sizeof(type_name_2)];
#
# for defined type names.
m4_define([b4_char_sizeof],
[b4_symbol_if([$1], [has_type],
[
m4_map([b4_char_sizeof_], [$@])dnl
      char _b4_char_sizeof_dummy@{sizeof([b4_symbol([$1], [type])])@};
])])


# b4_variant_definition
# ---------------------
# Define "variant".
m4_define([b4_variant_definition],
[[
  /// A char[S] buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current state.
  template <size_t S>
  struct variant
  {]b4_assert_if([
    /// Whether something is contained.
    bool built;
])[
    /// Empty construction.
    inline
    variant ()]b4_assert_if([
      : built (false)])[
    {}

    /// Instantiate a \a T in here.
    template <typename T>
    inline T&
    build ()
    {]b4_assert_if([
      assert (!built);
      built = true;])[
      return *new (buffer.raw) T;
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    inline T&
    build (const T& t)
    {]b4_assert_if([
      assert(!built);
      built = true;])[
      return *new (buffer.raw) T(t);
    }

    /// Construct and fill.
    template <typename T>
    inline
    variant (const T& t)]b4_assert_if([
      : built (true)])[
    {
      new (buffer.raw) T(t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    inline T&
    as ()
    {]b4_assert_if([
      assert (built);])[
      return reinterpret_cast<T&>(buffer.raw);
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    inline const T&
    as () const
    {]b4_assert_if([
      assert(built);])[
      return reinterpret_cast<const T&>(buffer.raw);
    }

    /// Swap the content with \a other.
    template <typename T>
    inline void
    swap (variant<S>& other)
    {
      std::swap (as<T>(), other.as<T>());
    }

    /// Assign the content of \a other to this.
    /// Destroys \a other.
    template <typename T>
    inline void
    build (variant<S>& other)
    {
      build<T>();
      swap<T>(other);
      other.destroy<T>();
    }

    /// Destroy the stored \a T.
    template <typename T>
    inline void
    destroy ()
    {
      as<T>().~T();]b4_assert_if([
      built = false;])[
    }

    /// A buffer large enough to store any of the semantic values.
    /// Long double is chosen as it has the strongest alignment
    /// constraints.
    union
    {
      long double align_me;
      char raw[S];
    } buffer;
  };
]])


## -------------------------- ##
## Adjustments for variants.  ##
## -------------------------- ##


# How the semantic value is extracted when using variants.

# b4_symbol_value(VAL, [TYPE])
# ----------------------------
m4_define([b4_symbol_value],
[m4_ifval([$2],
          [$1.as< $2 >()],
          [$1])])

# b4_symbol_value_template(VAL, [TYPE])
# -------------------------------------
# Same as b4_symbol_value, but used in a template method.
m4_define([b4_symbol_value_template],
[m4_ifval([$2],
          [$1.template as< $2 >()],
          [$1])])