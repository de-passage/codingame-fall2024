#pragma once

#include "actions.hpp"
#include "common.hpp"
#include "math.hpp"
#include "types.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_set>

using building_link = std::pair<building_id, building_id>;
struct game {
  // Game data
  resource_t resources;
  std::vector<transportation> travel_routes;
  std::vector<pod> pods;
  std::vector<landing_pad> landing_pads;
  std::vector<building> buildings;

  const building &get_building(building_id id) const {
    auto it =
        std::find_if(buildings.begin(), buildings.end(),
                     [id](const building &b) { return b.identifier == id; });
    if (it != buildings.end()) {
      return *it;
    }
    auto it2 =
        std::find_if(landing_pads.begin(), landing_pads.end(),
                     [id](const landing_pad &b) { return b.identifier == id; });
    if (it2 != landing_pads.end()) {
      return *it2;
    }
    std::stringstream ss;
    ss << "Building with id " << id << " not found";
    throw std::runtime_error(ss.str());
  }

  segment segment_between(building_id start, building_id end) const {
    return {get_building(start).coords, get_building(end).coords};
  }

  segment segment_between(const transportation& transportation) const {
    return segment_between(transportation.start_building, transportation.end_building);
  }

  pod_id next_pod_id() {
    return pod_id{current_pod_id_--};
  }

  pod_id next_priority_pod_id() {
    return pod_id{current_priority_pod_id++};
  }

  // Persistent data
  std::vector<desired_action> missing_pods;

private:
  int current_pod_id_{500};
  int current_priority_pod_id{1};
};

template <class T, class F>
requires std::is_same_v<T, building> ||
    std::is_same_v<T, landing_pad> && std::is_invocable_v<F, T>
void over_building_vector(const std::vector<T> &buildings, F &&func) {
  for (const auto &building : buildings) {
    func(building);
  }
}

template <class F> void over_buildings(const game &g, F &&func) {
  over_building_vector(g.buildings, std::forward<F>(func));
  over_building_vector(g.landing_pads, std::forward<F>(func));
}

inline std::istream &operator>>(std::istream &in, game &g) {
  in >> g.resources;
  in.ignore();
  LOG_DEBUG("resources: ", g.resources);

  in >> g.travel_routes;
  in.ignore();
  LOG_DEBUG("travel_routes: ", g.travel_routes.size());
  DEBUG_ONLY({
    for (const auto &route : g.travel_routes) {
      LOG_DEBUG("route: ", route.start_building, " -> ", route.end_building,
                " (", route.capacity, ")")
    }
  });

  in >> g.pods;
  in.ignore();
  LOG_DEBUG("pods: ", g.pods.size());
  DEBUG_ONLY({
    for (const auto &pod : g.pods) {
      std::cerr << "  pod: " << pod.identifier << " -> ";
      for (const auto &stop : pod.stops) {
        std::cerr << stop << " ";
      }
      std::cerr << std::endl;
    }
  });

  std::vector<any_building> buildings;
  in >> buildings;
  LOG_DEBUG("new buildings: ", buildings.size());
  for (auto &building : buildings) {
    std::visit(overloaded{[&](landing_pad &&b) {
                            g.landing_pads.push_back(std::move(b));
                          },
                          [&](struct building &&b) {
                            g.buildings.push_back(std::move(b));
                          }},
               std::move(building));
  }
  in.ignore();
  LOG_DEBUG("landing_pads: ", g.landing_pads.size());
  DEBUG_ONLY({
    for (const auto &pad : g.landing_pads) {
      std::cerr << "  pad: " << pad.identifier << " (" << pad.coords.x << ", "
                << pad.coords.y << ") ";
      for (const auto &worker : pad.expected_workers) {
        std::cerr << worker << " ";
      }
      std::cerr << std::endl;
    }
  });
  LOG_DEBUG("buildings: ", g.buildings.size());
  DEBUG_ONLY({
    for (const auto &building : g.buildings) {
      std::cerr << "  building: " << building.identifier << " ("
                << building.coords.x << ", " << building.coords.y << ") "
                << building.type << std::endl;
    }
  });
  return in;
}
