#pragma once

#include "common.hpp"
#include "types.hpp"
#include <ostream>

struct wait {};

inline std::ostream &operator<<(std::ostream &out, const wait &) {
  return out << "WAIT";
}

struct build_tube {
  building_id start_building;
  building_id end_building;
};

inline std::ostream &operator<<(std::ostream &out, const build_tube &tube) {
  return out << "TUBE " << tube.start_building << " " << tube.end_building;
}

struct upgrade_building {
  building_id start_building;
  building_id end_building;
};

inline std::ostream &operator<<(std::ostream &out,
                                const upgrade_building &upgrade) {
  return out << "UPGRADE " << upgrade.start_building << " "
             << upgrade.end_building;
}

struct build_teleporter {
  building_id start_building;
  building_id end_building;
};

inline std::ostream &operator<<(std::ostream &out,
                                const build_teleporter &teleporter) {
  return out << "TELEPORT " << teleporter.start_building << " "
             << teleporter.end_building;
}

struct build_pod {
  pod_id identifier;
  std::vector<building_id> stops;
};

inline std::ostream &operator<<(std::ostream &out, const build_pod &pod) {
  out << "POD " << pod.identifier;
  for (const auto &stop : pod.stops) {
    out << " " << stop;
  }
  return out;
}

struct destroy_pod {
  pod_id pod;
};

inline std::ostream &operator<<(std::ostream &out, const destroy_pod &destroy) {
  return out << "DESTROY " << destroy.pod;
}

using action = std::variant<wait, build_tube, upgrade_building,
                            build_teleporter, build_pod, destroy_pod>;

struct writer {
  std::ostream &out;
  std::ostream& operator()(auto &&a) const { return out << a; }
};

inline std::ostream &operator<<(std::ostream &out, const action &a) {
  return std::visit(writer{out}, a);
}

using actions = std::vector<action>;

inline std::ostream& operator<<(std::ostream& out, const actions& a) {
  if (a.empty()) return out << "WAIT";
  for (size_t i = 0; i < a.size(); ++i) {
    if (i > 0) out << ";";
    out << a[i];
  }
  return out;
}

namespace detail {
template <typename T, typename Variant>
struct index_of_impl;
template<typename T, typename... Ts>
  struct index_of_impl<T, std::variant<Ts...>> {
    constexpr static size_t value = []<size_t... Is>(std::index_sequence<Is...>) {
      size_t result = 0;
      ((std::is_same_v<T, std::variant_alternative_t<Is, std::variant<Ts...>>>
        ? result = Is
        : result), ...);
      return result;
    }(std::make_index_sequence<sizeof...(Ts)>{});
  };
}
template<class T, class Variant>
constexpr size_t index_of = detail::index_of_impl<T, Variant>::value;
static_assert(index_of<wait, action> == 0);
static_assert(index_of<build_tube, action> == 1);
static_assert(index_of<upgrade_building, action> == 2);
static_assert(index_of<build_teleporter, action> == 3);
static_assert(index_of<build_pod, action> == 4);
static_assert(index_of<destroy_pod, action> == 5);

  struct desired_action {
    action todo;
    int priority;
    resource_t cost;
  };

