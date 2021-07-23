# C pre-processor magic, revisited

Macros are almost always discouraged, but they are far from rare in production code. Therefore, it is of interest to explore their pitfalls, limitations, and compliance with the C-standard and others. In this article, I will analyse the macros introduced by [C pre-processor magic][1][[1]], complementing the information their provided in a concise manner. Therefore, it is recommended to either read that article beforehand, or in conjunction with the following sections.

## What makes it interesting?

The content of [[1]] is similar to that of [C Preprocessor tricks, tips, and idioms][2][[2]] and [Is the C preprocessor Turing complete?][3][[2]], except for two aspects. First, macro expansion is illustrated in detail, step by step, rendering it extremely useful for understanding or reviewing the expansion of macros with recursion. Second, it introduces a macro construct for recursion that automatically counts the number of items to iterate onto. That is, the macro does not require a count parameter that could possibly not match the actual number of items. 

### CPP currying

The `IF_ELSE` macro shown in [[1]] is analogous to the `IFF` macro shown in [[2]] except for a subtle difference in their calls. Specifically, their calls below

```
IF_ELSE(cond)(true_action)(false_action)
IFF    (cond)(true_action, false_action)
```

show that the `IF_ELSE` macro is a curried version of the `IFF` macro (that is, a transformed version that takes single arguments multiple times instead of multiple arguments at once).

The macro `IF_ELSE` may seem more general because it allows `true_action` to expand to a comma-separate list, for example 

```
IF_ELSE(cond)(a, b)(false_action)
```

The same can be achived with `IFF` by using a `COMMA` macro (see page 272 of [[8]] and also [[9]]) as follows

```
#define COMMA ,
IFF(cond)(a COMMA b, false_action)
```

(but not by enclosing the arguments with parentheses, e.g. `IFF(cond)((a, b), false_action)`, because the expansion will preserve the parentheses). Nevertheless, `IF_ELSE` is most likely seen as cleaner (see [[9]]).



## Compliance

### Is it C-compliant?

Not entirely, contrary to what the article states. The section 6.10.3-12 of the [C99 standard][4] requires that the ellipsis (`...`) matches at least one argument of the variadic-macro call (also for C11 and C18). However, this is not fulfileed for the variadic macro `HAS_ARGS`and `SECOND`. The former is defined as `#define HAS_ARGS(...)` but intended to handle calls with no arguments (within `MAP`), thereby in contradiction with C99. The latter is defined as `#define SECOND(x, y, ...)` but intended to handle calls with only two arguments ( within `IS_PROBE`), thereby resulting in a situation analogous to that of `HAS_ARGS` (i.e. the `...` is associated with no argument). In both cases, the GNU CPP 9.3.0 emits the following warning:

```
warning: ISO C99 requires at least one argument for the "..." in a variadic macro
```

when called as `cpp -std=c99 -Wpedantic` (see also section 3.6 of the [GNU CPP manual][5]). Without those flags, `__VA_ARGS__` is left empty. As an extension, GNU CPP also allowed to write `##` before `__VA_ARGS__`, which will resolve issues with dangling commas. That is, given 

```
#define p(fmt, ...) printf(fmt, ## __VA_ARGS__)
```

the call `p("hello")` will expand to `printf("hello")` instead of `printf("hello", )`.


### Compliance with other standards

Empty `__VA_ARGS__` may be considered a non-standard feature which coding standards often discourage (e.g. recommendation MSC14-C in [SEI CERT C Coding Standard][6] and rule 1 in [JPL Institutional Coding Standard for the C Programming Language][7]).


## Inaccuracies and redundancies

### Argument expansion preserves the number of arguments

Both [[1]] and [[2]] implement the macro `IS_PROBE` as variadic, seemingly as if that is required, but that is actually unnecessary. Recall the following definitions from [[1]]

```
#define SECOND(a, b, ...) b
#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define PROBE() ~, 1
```

which are analogous to the definitions of `CHECK_N`, `CHECH` AND `PROBE` in [[2]]. These definitions are intended to produce `1` when calling `IS_PROBE` with `PROBE()`, and `0` when calling `IS_PROBE` with anything else, that is

```
IS_PROBE(PROBE()) // should produce 1
IS_PROBE(xxx)     // should produce 0
```

(whether the latter expansion holds depends on the expansion of xxx; see [Effect of commas in arguments of is probe](#effect-of-commas-in-arguments-of-is-probe)).

The text and the substitution sequences in [[1]] seemingly suggests that `PROBE()` will expand in a way that `IS_PROBE` will be called with two arguments instead of one. That is, as if

```
IS_PROBE(PROBE())
```

expands into

```
IS_PROBE(~, 1)
```

instead of into something analogous to

```
IS_PROBE(~ COMMA 1)
```

with the macro `COMMA` defined as in the section [CPP currying](#cpp-currying). Although the latter may seem confusing because `COMMA` does not appear in the definition of `PROBE()`, it is more accurate in the sense that preserves the number of arguments passed to `IS_PROBE`.


To clarify, the section 3.10.6 of [[5]] starts by saying that, unless stringized or contatenated (recall `#` and `##`),  macro arguments are expanded before substituting them in the macro body. That is, it does not mention that, after expansion, they are substituted in the macro call and the macro invocation restarted with the expanded arguments. The difference is that, unlike the former, the latter may cause the number of arguments to increase. However, this is not explicitly mention there, one may still argue that it is not entirely clear. Furtunately, the last part of the section addresses this issue with a concrete example. 


Taking that into account, `IS_PROBE()` can be defined as

```
#define IS_PROBE(x) SECOND(x, 0)
```

with the call `IS_PROBE(PROBE())` expanding first to `SECOND(~, 1, 0)` and then to `1`. This also shows that, although variadic macros can expand to multiple arguments as mentioned in [[2]],  the property actually applies to any arguments. To conclude, the result is that defining the macro as variadic is redundant and could be removed to simplify the discusion. 



## Limitations

### Required number of arguments for `FIRST` and `SECOND`

For GNP cpp to work, `FIRST` and `SECOND` require at least one and two arguments, respectively, but for c99 compliance, they require one more. This can be reduced by defining

```
#define _FIRST(a, ...) a
#define FIRST(...) _FIRST(__VA_ARGS__, ~)

#define _SECOND(a, b, ...) b
#define SECOND(...) _SECOND(__VA_ARGS__, ~, ~)
```

With this modification, `FIRST` and `SECOND` require no argument for GNU CPP to work but at least one argument for C99 compliance. However, note that if `FIRST` is called with less than one argument, it will expand to `~`. According to [[1]], this character is likely to yield a syntax error. Hence, even if GNU CPP works, the compilation might not, effectively meaning that the macros `FIRST` and `SECOND` now require one and two arguments regardless of compliance level.

The restriction is not entirely enforced because character `~` is still valid in some situations. To ensure that it is, one may well choose to replace `~` with `\\` (two backslashes) which I think is even more likely to yield a syntax error. On the contrary, to remove the restriction, one may choose to remove the `~` character altogether and instead supply an empty argument.

### Empty arguments treated as missing by `HAS_ARGS`

The calls `HAS_ARGS()`, `HAS_ARGS( , )` and `HAS_ARGS( , other )` yield all the same result, namely `0`, because `HAS_ARGS` only takes into account the first argument (having no argument and having am emtpy argument is indistinguishable).

### The effect of commas in arguments to `IS_PROBE`

Although `IS_PROBE(xxx)` is intended in [[1]] and [[2]] to expand to `0` (see [Argument expansion preserves the number of arguments](#argument-expansion-preserves-the-number-of-arguments), this will not be the case if `xxx` expands to more than one argument with the second one not being `0`, as in

```
#define xxx a, b, c
IS_PROBE(xxx) // produces b
```


## References
[1]: http://jhnet.co.uk/articles/cpp_magic
[2]: https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
[3]: https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F
[4]: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
[5]: https://gcc.gnu.org/onlinedocs/cpp
[6]: https://wiki.sei.cmu.edu/confluence/display/c/
[7]: https://yurichev.com/mirrors/C/JPL_Coding_Standard_C.pdf
[8]: https://www.amazon.com/Beginning-Arduino-Second-Learn-Programming/dp/1484209419
[9]: https://stackoverflow.com/questions/13842468/comma-in-c-c-macro

1. [C pre-processor magic](http://jhnet.co.uk/articles/cpp_magic)
2. [C Preprocessor tricks, tips and idioms](https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms)
3. [Is the C preprocessor Turing complete?](https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F)
4. [C99 standard](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf)
5. [GNU CPP manual](https://gcc.gnu.org/onlinedocs/cpp)
6. [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/)
7. [JPL Institutional Coding Standard for the C Programming Language](https://yurichev.com/mirrors/C/JPL_Coding_Standard_C.pdf)
8. [Purdum J, Beginning C for Arduino, Second Edition: Learn C Programming for the Arduino,2015](https://www.amazon.com/Beginning-Arduino-Second-Learn-Programming/dp/1484209419)
9. [Response of non-a-user to "Comma in C/C++ macro"](https://stackoverflow.com/questions/13842468/comma-in-c-c-macro)
