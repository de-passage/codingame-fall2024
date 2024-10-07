#ifndef COMMON_HPP_HEADER_GUARD
#define COMMON_HPP_HEADER_GUARD

#include <utility>
namespace std {
template <class Key> struct hash;
template <class A, class B> struct hash<std::pair<A, B>> {
  std::size_t operator()(const std::pair<A, B> &p) const {
    return std::hash<A>{}(p.first) ^ std::hash<B>{}(p.second);
  }
};
} // namespace std

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#ifndef NDEBUG
#define DEBUG_ONLY(x)                                                          \
  do {                                                                         \
  } while (0)

#define LOG_DEBUG(...)
#else
#define DEBUG_ONLY(x)                                                          \
  do {                                                                         \
    x;                                                                         \
  } while (0)

#include <iostream>
namespace detail {
template <class... Ts> void log_stderr(Ts &&...ts) {
  (std::cerr << ... << ts) << std::endl;
}
} // namespace detail

#define LOG_DEBUG(...) DEBUG_ONLY(detail::log_stderr(__VA_ARGS__));
#endif

#endif // COMMON_HPP_HEADER_GUARD
