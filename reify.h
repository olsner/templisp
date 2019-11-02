#pragma once

#include <array>

namespace {

template<typename T> struct reify;
template<typename T> ob reified = reify<T>::value();

template<char... sym> struct reify<symbol<sym...>> {
    static constexpr auto name = std::array { sym..., '\0' };
    // TODO Have some compile-time registration of symbols, as well as a obnew
    // variant with static memory allocation.
    // Isn't constexpr new a thing somewhere?
    // if regsym/obnew are constexpr, this can be `static constexpr ob value = foo;`
    static ob value() { return regsym(obnew(otsymbol, 1, name.data())); }
};
template<char... s> struct reify<string<s...>> {
    static constexpr auto name = std::array { s..., '\0' };
    static ob value() { return obnew(otstring, 1, name.data()); }
};
template<typename CAR, typename CDR> struct reify<cons<CAR,CDR>> {
    static ob value() { return obnew(otcons, 2, reify<CAR>::value(), reify<CDR>::value()); }
};
template<> struct reify<nil> {
    static ob value() { return nullptr; }
};
template<typename T, T val> struct reify<value_type<T, val>> {
    static ob value() { return obnew(otint, 1, (uintptr_t)val); }
};

}
