#ifndef COMMON_HPP_HEADER_GUARD
#define COMMON_HPP_HEADER_GUARD

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif // COMMON_HPP_HEADER_GUARD
