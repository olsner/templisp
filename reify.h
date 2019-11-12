#pragma once

#include <array>

namespace {

template<typename T> struct reify;
template<typename T> ob reified = reify<T>::value();

template<char... sym> struct reify<symbol<sym...>> {
    static constexpr auto name = std::array { sym..., '\0' };
    static inline auto val = ob_(otsymbol, name.data());
    static ob value() { return regsym(&val); }
};
template<char... s> struct reify<string<s...>> {
    static constexpr auto name = std::array { s..., '\0' };
    static constexpr auto val = ob_(otstring, name.data());
    static constexpr ob value() { return (ob)&val; }
};
template<typename CAR, typename CDR> struct reify<cons<CAR,CDR>> {
    static inline auto val = ob_(otcons, reify<CAR>::value(), reify<CDR>::value());
    static ob value() { return (ob)&val; }
};
template<> struct reify<nil> {
    static ob value() { return nullptr; }
};
template<typename T, T valu> struct reify<value_type<T, valu>> {
    static constexpr auto val = ob_(valu);
    static ob value() { return (ob)&val; }
};

}
