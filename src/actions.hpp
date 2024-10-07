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
  for (size_t i = 0; i < a.size(); ++i) {
    if (i > 0) out << ";";
    out << a[i];
  }
  return out;
}
