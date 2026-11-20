#pragma once
#include <cstdlib>
#include <iostream>
#include <print>
#include <source_location>
#include <sstream>

struct dummy_t { };
template <>
struct std::formatter<dummy_t, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        return it;
    }
    template <class FmtContext>
    FmtContext::iterator format(dummy_t, FmtContext& ctx) const
    {
        std::ostringstream out;
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

#define cum_eval(...) ([&]() { __VA_ARGS__; return dummy_t{}; }())

#define cum_assert(EXPR, FMT, ...)                                                      \
    if (!(EXPR))                                                                        \
        do {                                                                            \
            std::println("assertion: `{}' failed!\n\"" FMT "\"", #EXPR, ##__VA_ARGS__); \
            constexpr const auto l = std::source_location::current();                   \
            std::println("file: {}({}:{}) `{}`",                                        \
                l.file_name(), l.line(), l.column(), l.function_name());                \
            std::flush(std::cout);                                                      \
            exit(1);                                                                    \
        } while (0);
