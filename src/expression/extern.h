/*
 * Copyright (C) 2017 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file extern.h
/// The MEF facilities to call external functions in expressions.

#ifndef SCRAM_SRC_EXPRESSION_EXTERN_H_
#define SCRAM_SRC_EXPRESSION_EXTERN_H_

#include <string>
#include <type_traits>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include "src/element.h"
#include "src/error.h"
#include "src/expression.h"

namespace scram {
namespace mef {

/// The MEF construct to extend expressions with external libraries.
/// This class dynamically loads and manages libraries.
/// It supports only very basic interface for C function lookup with its symbol.
class ExternLibrary : public Element, private boost::noncopyable {
 public:
  /// @copydoc Element::Element
  ///
  /// @param[in] lib_path  The library path with its name.
  /// @param[in] reference_dir  The reference directory for relative paths.
  /// @param[in] system  Search for the library in system paths.
  /// @param[in] decorate  Decorate the library name with prefix and suffix.
  ///
  /// @throws InvalidArgument  The library path is invalid.
  /// @throws IOError  The library cannot be found.
  ExternLibrary(std::string name, std::string lib_path,
                const boost::filesystem::path& reference_dir, bool system,
                bool decorate);

  ~ExternLibrary();  ///< Closes the loaded library.

  /// @tparam F  The C free function pointer type.
  ///
  /// @param[in] symbol  The function symbol in the library.
  ///
  /// @returns The function pointer resolved from the symbol.
  ///
  /// @throws UndefinedElement  The symbol is not in the library.
  ///
  /// @note The functionality should fail at compile time (UB in C)
  ///       if the platform doesn't support
  ///       object pointer to function pointer casts.
  template <typename F>
  std::enable_if_t<std::is_pointer<F>::value &&
                       std::is_function<std::remove_pointer_t<F>>::value,
                   F>
  get(const std::string& symbol) const {
    return reinterpret_cast<F>(get(symbol.c_str()));
  }

 private:
  /// Hides all the cross-platform shared library faculties.
  /// @todo Remove/refactor after switching to Boost 1.61 on Linux.
  class Pimpl;

  /// @param[in] symbol  The function symbol in the library.
  ///
  /// @returns The function loaded from the library symbol.
  ///
  /// @throws UndefinedElement  The symbol is not in the library.
  void* get(const char* symbol) const;

  Pimpl* pimpl_;  ///< Provides basic implementation for function discovery.
};

/// Extern function abstraction to be referenced by expressions.
///
/// @tparam R  Numeric return type.
/// @tparam Args  Numeric argument types.
///
/// @pre The source dynamic library is loaded as long as this function lives.
template <typename R, typename... Args>
class ExternFunction : public Element, private boost::noncopyable {
 public:
  using Pointer = R (*)(Args...);  ///< The function pointer type.

  /// Loads a function from a library for further usage.
  ///
  /// @copydoc Element::Element
  ///
  /// @param[in] symbol  The symbol name for the function in the library.
  /// @param[in] library  The dynamic library to lookup the function.
  ///
  /// @throws UndefinedElement  There is no such symbol in the library.
  ExternFunction(std::string name, const std::string& symbol,
                 const ExternLibrary& library)
      : Element(name), fptr_(library.get<Pointer>(symbol)) {}

  /// Calls the library function with the given numeric arguments.
  R operator()(Args... args) const noexcept { return fptr_(args...); }

 private:
  Pointer fptr_;  ///< The pointer to the extern function in a library.
};

namespace detail {  // Helpers for extern function call with Expression values.

/// Marshaller of expressions to extern function calls.
///
/// @tparam N  The number of arguments.
///
/// @pre The number of arguments is exactly the same at runtime.
template <int N>
struct Marshaller {
  /// Evaluates the argument expressions and marshals the result to function.
  template <typename F, typename R, typename... Ts, typename... Args>
  R operator()(const ExternFunction<R, Args...>& self,
               const std::vector<Expression*>& args, F&& eval,
               Ts&&... values) const noexcept {
    double value = eval(args[N - 1]);
    return Marshaller<N - 1>()(self, args, std::forward<F>(eval), value,
                               std::forward<Ts>(values)...);
  }
};

/// Specialization to call the extern function with argument values.
template <>
struct Marshaller<0> {
  /// Calls the extern function with the argument values.
  template <typename F, typename R, typename... Ts, typename... Args>
  R operator()(const ExternFunction<R, Args...>& self,
               const std::vector<Expression*>&, F&&, Ts&&... values) const
      noexcept {
    return self(std::forward<Ts>(values)...);
  }
};

}  // namespace detail

/// Expression evaluating an extern function with expression arguments.
///
/// @tparam R  Numeric return type.
/// @tparam Args  Numeric argument types.
template <typename R, typename... Args>
class ExternExpression
    : public ExpressionFormula<ExternExpression<R, Args...>> {
 public:
  /// @param[in] extern_function  The library function.
  /// @param[in] args  The argument expression for the function.
  ///
  /// @throws InvalidArgument  The number of arguments is invalid.
  explicit ExternExpression(const ExternFunction<R, Args...>* extern_function,
                            std::vector<Expression*> args)
      : ExpressionFormula<ExternExpression>(std::move(args)),
        extern_function_(*extern_function) {
    if (Expression::args().size() != sizeof...(Args))
      throw InvalidArgument("The number of function arguments does not match.");
  }

  /// Computes the extern function with the given evaluator for arguments.
  template <typename F>
  double Compute(F&& eval) noexcept {
    return detail::Marshaller<sizeof...(Args)>()(
        extern_function_, Expression::args(), std::forward<F>(eval));
  }

 private:
  const ExternFunction<R, Args...>& extern_function_;  ///< The source function.
};

}  // namespace mef
}  // namespace scram

#endif  // SCRAM_SRC_EXPRESSION_EXTERN_H_