# C pre-processor magic, revisited

Although macros are almost always discouraged, they are not rare in production code. Therefore, it is of interest to explore their pitfalls, limitations, and compliance with the C-standard and others. In this article, I will analyse the macros introduced by [C pre-processor magic][1], complementing the information their provided in a concise manner. Therefore, it is recommended to either read that article beforehand, or in conjunction with the following sections.

## What makes it interesting?

The content of that article is similar to that of [C Preprocessor tricks, tips, and idioms][2] and [Is the C preprocessor Turing complete?][3], except for two aspects. First, macro expansion is illustrated in detail, step by step, rendering it extremely useful for understanding or reviewing the expansion of macros with recursion. Second, it introduces a macro construct for recursion that automatically counts the number of items to iterate onto. That is, the macro does not require a count parameter that could possibly not match the actual number of items. 

## Is it C-compliant?

Not entirely, contrary to what the article states. The section 6.10.3-12 of the C99 standard [4] requires that the ellipsis (`...`) matches at least one argument of the variadic-macro call (also for C11 and C18). However, this is not fulfileed for the variadic macro `HAS_ARGS`and `SECOND`. The former is defined as `#define HAS_ARGS(...)` but intended to handle calls with no arguments (within `MAP`), thereby in contradiction with C99. The latter is defined as `#define SECOND(x, y, ...)` but intended to handle calls with only two arguments ( within `IS_PROBE`), thereby resulting in a situation analogous to that of `HAS_ARGS` (i.e. the `...` is associated with no argument). In both cases, the GNU CPP 9.3.0 emits the following warning:

```warning: ISO C99 requires at least one argument for the "..." in a variadic macro```

when called as `cpp -std=c99 -Wpedantic` (see also section 3.6 of the [GNU CPP manual][5]). Without those flags, `__VA_ARGS__` is left empty. As an extension, GNU CPP also allowed to write `##` before `__VA_ARGS__`, which will resolve issues with dangling commas. That is, given 

```#define p(fmt, ...) printf(fmt, ## __VA_ARGS__)```,

the call `p("hello")` will expand to `printf("hello")` instead of `printf("hello", )`.


## Compliance with other standards

Empty `__VA_ARGS__` may be considered a non-standard feature which coding standards often discourage (e.g. recommendation MSC14-C in [SEI CERT C Coding Standard][6] and rule 1 in [JPL Institutional Coding Standard for the C Programming Language][7]).


## CPP currying

The `IF_ELSE` macro presented in [1] differs from that of 


## References
[1]: http://jhnet.co.uk/articles/cpp_magic
[2]: https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
[3]: https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F
[4]: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
[5]: https://gcc.gnu.org/onlinedocs/cpp
[6]: https://wiki.sei.cmu.edu/confluence/display/c/
[7]: https://yurichev.com/mirrors/C/JPL_Coding_Standard_C.pdf
