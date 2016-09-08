# The Tlön programming language.

Tlön is a programming language transpiled into Modern C++. These are the key goals:
* Add features which C++ is missing.
* Throw out all the legacy crap that's been dragged into C++ over the years.
* Also, just simplify things.
* Provide a sensible API over Standard Library/STL.
* Other lofty goals :)

The following is an approximate featureset with explanations *why* those features are in there.

## Front End
* Two-step front-end implementation
  * Make use of non-keyboard symbols (e.g., subscript/superscript).
  * Later: a flexible editor.
* No semicolons (they can be used for something else).
* No inappropriate symbols: `-` is not subtraction, `*` is not multiplication, etc.
* Bitwise operators strictly textual (`xor`), no operator symbols. Avoids the confusion between `&&` and `&`.
* `%` means `%`, i.e. `5%` means `0.05`
* Minute/second marks `"'` cannot be used for string or characters.
* `<` or `>` cannot be used for anything other than less-that/greater-than.

## Basic Control Flow
* Definitions via `:=` for non-constant and `≝` (or `≡`) for constants.
* Assignment via `->` and `<-` (both ways). This helps readability in some cases
* Assignment can cluster (`x,y <- 0`, `x -> y,z`) and daisy-chain (`x -> y -> z`).
* JAI's `delay` for executing code at the end of scope.

## Identifiers/Variables
* Extended symbol range allows identifiers with printable non-numeric, non-syntax letters.
* Access modifier prefix? `-private, #protected, +public`.
* Forced initial values (zeros for numerics, `0` for bits).

## Namespaces

* Namespace declarations do not cause a scope, content not wrapped in `{}`. This avoids a 1-tab indentation of *everything* you write and in 99% is a meaningless idea.
* Multipart declarations are permitted: `namespace Foo.Bar`. Avoids insanity like `namespace Foo { namespace Bar { ...`.
* Fused namespace-and-class declaration. Writing `class Foo.Bar` creates namespace `Foo` and treats all subsequence input as pertaining to class `Bar`, unless...
* Fused namespace-and-inner-class declaration. If you must. Writing `class Foo.Bar.Baz` where a class `Bar` already exists in namespace `Foo` does, you guessed it, an inner class `Baz` that's declared inside `Bar`.
* Imports go *into* the namespace. FxCop was right.


## Types

* Interfaces (*a la* C#/Java).
  * Can feature property definitions (like C#).
* Union/intersection types?
* Virtual destructor added by default.
* Virtual dispatch-centric (visitor) member?
* Friend classes are nonsensical. We should either throw them out completely or make them bidirectional (meaning if `A` is a friend of `B` then the reverse applies). If we keep friend classes, it should also be possible to specify entire categories, e.g., `friend MyNamespace.*`.

## Variables

* Globals not allowed. All globals get synthesized into an injected `Globals` class. Global variables and functions ruin discoverability and testability and cause weird behaviors when included in multiple translation units.
* Should be very little difference between temporary and member variable declarations.

## Functions
* `add := (a,b:i32 := 0) => a+b` -- argument clustering, default value, no `return` statement and return type inferencing.
* Capture modes indicated by subscript? E.g., `add_five := x => twoᵥ + threeᵣ`.

## Properties

* Expandable from an existing field declaration, e.g. `x,y := 0` becomes `x,y := 0 with get := xx and set := it -> xx`.
* Auto-properties `x := 0 with get and set`.
* Bindable properties, e.g., `x =:= y` ensures `x` is always set to `y` via an atomic change. Implies construction of critical sections.
