# The Tlön programming language.

Tlön is a programming language transpiled into Modern C++. These are the key goals:
* Add features which C++ is missing.
* Throw out all the legacy crap that's been dragged into C++ over the years.
* Also, just simplify things.
* Provide a sensible API over Standard Library/STL.
* Other lofty goals :)

For a more lively demonstration, watch [this video](https://www.youtube.com/watch?v=LmRTPweKGmQ).

The following is an approximate featureset with explanations *why* those features are in there.

## Front End
* Two-step front-end implementation
  * Make use of non-keyboard symbols (e.g., subscript/superscript).
  * Later: a flexible editor.
* No semicolons (they can be used for something else).
* No inappropriate symbols: `-` is not subtraction, `*` is not multiplication, etc. This prevents the subversion of well-defined mathematical symbols.
* Bitwise operators strictly textual (`xor`), no operator symbols. Avoids the confusion between `&&` and `&` etc.
* `%` means `%`, i.e. `5%` means `0.05`. Operators `mod` and `rem` used instead.
* Minute/second marks `"'` cannot be used for string or characters.
* `<` or `>` cannot be used for anything other than less-that/greater-than.

## Basic Operations and Control Flow
* Definitions via `:=` for non-constant and `≝` (or `≡`) for constants. (WIP)
* `=` used as comparison operator, also valid in mutual comparisons `if (a = b = c)`
* Assignment via `->` and `<-` (both ways). This helps readability in some cases.
* Assignment can cluster (`x,y <- 0`, `x -> y,z`) and daisy-chain (`x -> y -> z`)
* JAI's `delay` for executing code at the end of scope.

## Identifiers/Variables
* Extended symbol range allows identifiers with printable non-numeric, non-syntax letters.
* Access modifier prefix? `-private, #protected, +public`.
* Forced initial values (zeros for numerics, `0` for bits). Principle of least surprise.

## Namespaces

* Namespace declarations do not 'cause' a scope, content not wrapped in `{}`. This avoids a 1-tab indentation of *everything* you write and in 99% is a meaningless idea.
* Multipart declarations are permitted: `namespace Foo.Bar`. Avoids insanity like `namespace Foo { namespace Bar { ...`.
* Fused namespace-and-class declaration. Writing `class Foo.Bar` creates namespace `Foo` and treats all subsequence input as pertaining to class `Bar`, unless...
* Fused namespace-and-inner-class declaration. If you must. Writing `class Foo.Bar.Baz` where a class `Bar` already exists in namespace `Foo` does, you guessed it, an inner class `Baz` that's declared inside `Bar`.

## Types

* All types inherit from `tlön::object` whose interface contains `equals()`, `hash_code()` and `str()` (just like C#).
* Interfaces (*a la* C#/Java).
  * Can feature property definitions (like C#).
* Virtual destructor added by default.
* Virtual dispatch-centric (visitor) member?
* Global members not allowed

## Reflection

* All types support reflection by including a `static` table of reflection-based info.
* Reflection is intended to work as it does in C#, i.e., an ability to inspect a type's properties & methods.
* Reflection is intended to provide `invoke()` functionality.

## Functions
* `add := (a,b:i32 := 0) => a+b` -- argument clustering, default value, no `return` statement and return type inferencing.
* Capture modes indicated by subscript? E.g., `add_five := x => twoᵥ + threeᵣ`.
* Things are passed and stored by reference by default;

## Fields
* The concept of a 'field' does not exist in Tlön. Period.

## Properties

* Expandable from an existing field declaration, e.g. `x,y := 0` becomes `x,y := 0 with get := xx and set := it -> xx`.
* Auto-properties `x := 0 with get and set`.
* Bindable properties, e.g., `x =:= y` ensures `x` is always set to `y` via an atomic change. Implies construction of critical sections.

## Events (tentative)

* TODO; most likely based on `boost::signal<>`

## Embedded Features

These are some features that will be baked into the system:

* Database system: most likely a direct-to-metal interface for some commercial database system (e.g., MongoDB). Think Slang/SecDB.
* Dependency injection: given that we have reflection, DI is going to be baked in and used everywhere.
