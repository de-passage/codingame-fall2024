#pragma once

#include "strong_types.hpp"
#include "strong_types/iostream.hpp"
#include "strong_types/hash.hpp"
#include <istream>
#include <variant>
#include <vector>

template <class Tag>
using strong_id =
    dpsg::strong_types::strong_value<int, Tag, dpsg::strong_types::comparable,
                                     dpsg::strong_types::streamable,
                                     dpsg::strong_types::hashable>;

#define DEFINE_ID_TYPE(identifier, suffix)                                     \
  using identifier = strong_id<struct identifier##_tag>;                       \
  consteval identifier operator""_##suffix(unsigned long long value) {         \
    return identifier{static_cast<identifier::value_type>(value)};             \
  }

DEFINE_ID_TYPE(building_id, bid);
DEFINE_ID_TYPE(pod_id, pid);

using capacity_t = dpsg::strong_types::number<int, struct capacity_tag,
                                              dpsg::strong_types::streamable>;
using building_type = dpsg::strong_types::strong_value<
    unsigned int, struct building_type_tag, dpsg::strong_types::streamable,
    dpsg::strong_types::comparable_with<unsigned int>,
    dpsg::strong_types::comparable, dpsg::strong_types::hashable>;

using resource_t = dpsg::strong_types::number<int, struct resource_tag,
                                              dpsg::strong_types::streamable>;

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
