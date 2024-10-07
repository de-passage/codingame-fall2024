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
} // namespace custom_modifiers

#define DEFINE_ID_TYPE(identifier)                                             \
  using identifier =                                                           \
      dpsg::strong_types::strong_value<int, struct identifier##_tag,           \
                                       dpsg::strong_types::comparable,         \
                                       custom_modifiers::streamable>;

DEFINE_ID_TYPE(building_id);
consteval building_id operator""_bid(unsigned long long value) {
  return building_id{static_cast<int>(value)};
}
DEFINE_ID_TYPE(pod_id)
consteval pod_id operator""_pid(unsigned long long value) {
  return pod_id{static_cast<int>(value)};
}

using capacity_t = dpsg::strong_types::number<int, struct capacity_tag,
                                              custom_modifiers::streamable>;
using building_type =
    dpsg::strong_types::strong_value<unsigned int, struct building_type_tag,
                                     custom_modifiers::streamable,
                                     dpsg::strong_types::comparable_with<unsigned int>>;

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

template<class T>
inline std::istream& operator>>(std::istream& in, std::vector<T>& vec) {
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
