# C pre-processor magic, revisited

Although macros are almost always discouraged, they are not rare in production code. Therefore, it is of interest to explore their pitfalls, limitations, and compliance with the C-standard and others. In this article, I will analyse the macros introduced by [C pre-processor magic][1], complementing the information their provided in a concise manner. Therefore, it is recommended to either read that article beforehand, or in conjunction with the following sections.

## What makes it interesting?

The content of that article is similar to that of [C Preprocessor tricks, tips, and idioms][2] and [Is the C preprocessor Turing complete?][3], except for two aspects. First, macro expansion is illustrated in detail, step by step, rendering it extremely useful for understanding or reviewing the expansion of macros with recursion. Second, it introduces a macro construct for recursion that automatically counts the number of items to iterate onto. That is, the macro does not require a count parameter that could possibly not match the actual number of items. 

## Is it C-compliant?

Not entirely, contrary to what the article states. The section 6.10.3-12 of the [C99 standard][4] requires that the ellipsis (`...`) matches at least one argument of the variadic-macro call (also for C11 and C18). However, this is not fulfileed for the variadic macro `HAS_ARGS`and `SECOND`. The former is defined as `#define HAS_ARGS(...)` but intended to handle calls with no arguments (within `MAP`), thereby in contradiction with C99. The latter is defined as `#define SECOND(x, y, ...)` but intended to handle calls with only two arguments ( within `IS_PROBE`), thereby resulting in a situation analogous to that of `HAS_ARGS` (i.e. the `...` is associated with no argument). In both cases, the GNU CPP 9.3.0 emits the following warning:

```warning: ISO C99 requires at least one argument for the "..." in a variadic macro```

when called as `cpp -std=c99 -Wpedantic` (see also section 3.6 of the [GNU CPP manual][5]). Without those flags, `__VA_ARGS__` is left empty. As an extension, GNU CPP also allowed to write `##` before `__VA_ARGS__`, which will resolve issues with dangling commas. That is, given 

```#define p(fmt, ...) printf(fmt, ## __VA_ARGS__)```,

the call `p("hello")` will expand to `printf("hello")` instead of `printf("hello", )`.


## Compliance with other standards

Empty `__VA_ARGS__` may be considered a non-standard feature which coding standards often discourage (e.g. recommendation MSC14-C in [SEI CERT C Coding Standard][6] and rule 1 in [JPL Institutional Coding Standard for the C Programming Language][7]).


## CPP currying

The `IF_ELSE` macro shown in [[1]] is analogous to the `IFF` macro shown in [[2]] except for a subtle difference in their calls. Specifically, their calls below

```
IF_ELSE(cond)(true_action)(false_action)
IFF    (cond)(true_action, false_action)
```

show that the `IF_ELSE` macro is a curried version of the `IFF` macro (that is, a transformed version that takes single arguments multiple times instead of multiple arguments at once).

The macro `IF_ELSE` may seem more general because it allows `true_action` to expand to a comma-separate list, for example 

```IF_ELSE(cond)(a, b)(false_action)```. 

The same can be achived with `IFF` by using a `COMMA` macro (see page 272 of [[8]] and also [[9]]) as follows

```
#define COMMA ,
IFF(cond)(a COMMA b, false_action)
```

(but not by enclosing the arguments with parentheses, e.g. `IFF(cond)((a, b), false_action)`, because the expansion will preserve the parentheses). Nevertheless, `IF_ELSE` is most likely seen as cleaner (see [[9]]).


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
