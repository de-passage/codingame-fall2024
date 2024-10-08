#pragma once

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

  segment get_segment(building_id start, building_id end) const {
    return {get_building(start).coords, get_building(end).coords};
  }

  // Cache
  std::unordered_set<building_link> possible_connection;
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

inline void compute_possible_connections(const building &b, game &g) {
  // We add all new possible connections
  // Possible connections are:
  // - All pairs with the new building.
  over_buildings(g, [&](const building &other) {
    // Always keep the nodes ordered!!!
    auto pair = std::minmax(b.identifier, other.identifier);
    g.possible_connection.emplace(pair.first, pair.second);
  });

  /*
  // We trim the connections made invalid by the new node
  // We need to remove:
  // - the connections that have the new building on the segment
  // - the connections that have the new building as a start or end and:
  //    - have another building on the segment
  for (const auto &connection : g.possible_connection) {
    auto start_id = connection.first;
    auto end_id = connection.second;

    if (b.identifier == start_id || b.identifier == end_id) {
      // The new building part of the segment, we remove the connection iff
      // another building is on the segment
      over_buildings(g, [&](const building &other) {
        if (other.identifier != start_id && other.identifier != end_id) {
          const auto &starter = b.identifier == start_id ? b : g.get_building(start_id);
          const auto &ender = b.identifier == end_id ? b : g.get_building(end_id);
          if (on_segment({starter.coords, ender.coords}, other.coords)) {
            g.possible_connection.erase({start_id, end_id});
          }
        }
      });

    } else {
      const auto &starter = g.get_building(start_id);
      const auto &ender = g.get_building(end_id);

      // If the new building is on the segment, we remove the connection
      if (on_segment({starter.coords, ender.coords}, b.coords)) {
        g.possible_connection.erase({starter.identifier, ender.identifier});
      }
    }
  }
  */
}

inline void remove_travel_routes(game &g) {
  for (const auto &route : g.travel_routes) {
    auto [start, end] = std::minmax(route.start_building, route.end_building);
    for (const auto &connection : g.possible_connection) {
      if (connection.first == start && connection.second == end) {
        g.possible_connection.erase(connection);
      } else if (segments_intersect(
                     g.get_segment(connection.first, connection.second),
                     g.get_segment(route.start_building, route.end_building))) {
        g.possible_connection.erase(connection);
      }
    }
  }
}

inline std::istream &operator>>(std::istream &in, game &g) {
  in >> g.resources;
  in.ignore();
  LOG_DEBUG("resources: ", g.resources);

  in >> g.travel_routes;
  remove_travel_routes(g);
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
                            compute_possible_connections(b, g);
                            g.landing_pads.push_back(std::move(b));
                          },
                          [&](struct building &&b) {
                            compute_possible_connections(b, g);
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

  DEBUG_ONLY({
    std::cerr << "possible connections: \n";
    for (const auto &connection : g.possible_connection) {
      std::cerr << "  " << connection.first << " -> " << connection.second << '\n';
    }
    std::cerr << std::endl;
  });
  return in;
}
