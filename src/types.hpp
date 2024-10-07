#pragma once

#include "strong_types.hpp"
#include <istream>
#include <variant>
#include <vector>

namespace custom_modifiers {
namespace st = dpsg::strong_types;
namespace meta = st::black_magic;

struct streamable {
  template <class T>
  struct type
      : meta::for_each<
            meta::tuple<meta::tuple<st::shift_left, std::basic_istream<char>>,
                        meta::tuple<st::shift_right, std::basic_ostream<char>>>,
            meta::apply<st::implement_binary_operation, T>> {};
};

namespace detail {
template <class T> struct get_first;
template <template <class...> class C, class T, class... Ts>
struct get_first<C<T, Ts...>> {
  using type = T;
};
} // namespace detail
struct hashable {
  template <class T> struct type {
    using hashable = typename detail::get_first<T>::type;
  };
};

template <class T>
concept MadeHashable = requires(T obj) {
  typename T::hashable;
};
} // namespace custom_modifiers

namespace std {
template <custom_modifiers::MadeHashable T> struct hash<T> {
  std::size_t operator()(const T &value) const {
    return std::hash<typename T::hashable>{}(
        custom_modifiers::st::get_value_t{}(value));
  }
};
} // namespace std

template <class Tag>
using strong_id =
    dpsg::strong_types::strong_value<int, Tag, dpsg::strong_types::comparable,
                                     custom_modifiers::streamable,
                                     custom_modifiers::hashable>;

#define DEFINE_ID_TYPE(identifier, suffix)                                     \
  using identifier = strong_id<struct identifier##_tag>;                       \
  consteval identifier operator""_##suffix(unsigned long long value) {         \
    return identifier{static_cast<identifier::value_type>(value)};             \
  }

DEFINE_ID_TYPE(building_id, bid);
DEFINE_ID_TYPE(pod_id, pid);

using capacity_t = dpsg::strong_types::number<int, struct capacity_tag,
                                              custom_modifiers::streamable>;
using building_type = dpsg::strong_types::strong_value<
    unsigned int, struct building_type_tag, custom_modifiers::streamable,
    dpsg::strong_types::comparable_with<unsigned int>,
    dpsg::strong_types::comparable, custom_modifiers::hashable>;

using resource_t = dpsg::strong_types::number<int, struct resource_tag,
                                              custom_modifiers::streamable>;

constexpr static inline building_type landing_pad_type{0};

struct transportation {
  building_id start_building;
  building_id end_building;
  capacity_t capacity;
};

inline std::istream &operator>>(std::istream &in, transportation &t) {
  return in >> t.start_building >> t.end_building >> t.capacity;
}

struct pod {
  pod_id identifier;
  std::vector<building_id> stops;
};

template <class T>
inline std::istream &operator>>(std::istream &in, std::vector<T> &vec) {
  int size;
  in >> size;
  vec.resize(size);
  for (auto &stop : vec) {
    in >> stop;
  }
  return in;
}

inline std::istream &operator>>(std::istream &in, pod &p) {
  return in >> p.identifier >> p.stops;
}

struct coordinates {
  int x;
  int y;
};

inline std::istream &operator>>(std::istream &in, coordinates &c) {
  return in >> c.x >> c.y;
}

struct building {
  building_id identifier;
  building_type type;
  coordinates coords;
};

struct landing_pad : building {
  std::vector<building_type> expected_workers;
};

using any_building = std::variant<building, landing_pad>;

inline std::istream &operator>>(std::istream &in, any_building &b) {
  building_type type;
  in >> type;
  if (type == 0) {
    landing_pad land;
    land.type = type;
    in >> land.identifier >> land.coords >> land.expected_workers;
    b = land;
  } else {
    building mod;
    mod.type = type;
    in >> mod.identifier >> mod.coords;
    b = mod;
  }
  return in;
}
