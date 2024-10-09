#include "actions.hpp"
#include "common.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "math.hpp"
#include "types.hpp"
#include <cmath>

#include <algorithm>
#include <bits/ranges_algo.h>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <chrono>

constexpr static inline std::hash<std::pair<building_id, building_id>>
    hash_pair;

using transfer_time =
    dpsg::strong_types::number<int, struct transfer_time_tag,
                               dpsg::strong_types::streamable>;
using worker_count = dpsg::strong_types::number<int, struct worker_count_tag,
                                                dpsg::strong_types::streamable>;

struct turn_data {
  std::unordered_map<building_link, transfer_time> transfer_times;
  std::unordered_set<building_link> direct_connections;
  std::unordered_map<building_type, worker_count> worker_destinations;
};

struct path_search_result {
  std::vector<building_id> target_buildings;
  int current_best_path{-1};
};

path_search_result find_path_to_destination(const landing_pad &source,
                                            building_type destination,
                                            const game &g, const turn_data &c) {
  struct path {
    building_id id;
    int distance;
  };
  path_search_result result;
  std::queue<path> to_visit;
  std::unordered_set<building_id> visited;

  to_visit.push({source.identifier, 0});

  for (auto &pad : g.landing_pads) {
    auto current = to_visit.front();
    to_visit.pop();

    visited.insert(current.id);

    for (auto building : g.buildings) {
      if (building.type == destination) {
        result.target_buildings.push_back(building.identifier);
        if (result.current_best_path > 0) {
          // We already found a better path, we only keep this as a reference in
          // case we want to build a new connection.
          continue;
        }

        if (c.direct_connections.count(
                building_link{current.id, building.identifier})) {
          // We found a direct connection to the building,
          result.current_best_path = current.distance + 1;
          continue;
        }
      } else {
        if (result.current_best_path > 0) {
          // We already found a better path
          continue;
        }

        // If there's a direct connection to this building, we'll inspect it
        if (c.direct_connections.count(
                building_link{current.id, building.identifier})) {
          if (visited.count(building.identifier) > 0) {
            continue;
          }
          to_visit.push({building.identifier, current.distance + 1});
        }
      }
    }
  }

  return result;
}

bool can_build_tube(const building_id &start_id, const building_id &end_id,
                    const game &g) {
  auto &start = g.get_building(start_id);
  auto &end = g.get_building(end_id);
  segment wanted_tube{start.coords, end.coords};
  // Check if the tube would intersect with any building
  for (auto &building : g.buildings) {
    if (building.identifier == start.identifier ||
        building.identifier == end.identifier) {
      continue;
    }

    if (on_segment(wanted_tube, building.coords)) {
      return false;
    }
  }

  for (auto &building : g.landing_pads) {
    if (building.identifier == start.identifier ||
        building.identifier == end.identifier) {
      continue;
    }

    if (on_segment(wanted_tube, building.coords)) {
      return false;
    }
  }

  for (auto &transport : g.travel_routes) {
    if (transport.start_building == start.identifier) {
      if (transport.end_building == end.identifier) {
        return false;
      } else {
        continue; // segments can't intersect if they have a common point
      }
    } else if (transport.end_building == start.identifier) {
      continue; // segments can't intersect if they have a common point
    }
    if (segments_intersect(wanted_tube, g.segment_between(transport))) {
      return false;
    }
  }

  return true;
}

actions decide(game &g) {
  turn_data c;

  // Build an indexable table of connections and initialize transfer times
  for (auto &transport : g.travel_routes) {
    c.direct_connections.emplace(transport.start_building,
                                 transport.end_building);
    c.transfer_times.emplace(
        std::pair{transport.start_building, transport.end_building}, 1);
    if (transport.capacity > 0) {
      c.direct_connections.emplace(transport.end_building,
                                   transport.end_building);
      c.transfer_times.emplace(
          std::pair{transport.end_building, transport.start_building}, 1);
    }
  }

  std::vector<desired_action> actions{std::move(g.missing_pods)};
  g.missing_pods.clear();
  for (auto &pod : actions) {
    pod.priority *= 3;
  }

  std::unordered_set<building_link> already_requested;

  // Compute the shortest path from landing_pad to required building
  for (auto &pad : g.landing_pads) {
    for (auto worker : pad.expected_workers) {
      c.worker_destinations[worker]++;
    }

    for (auto &[destination, _] : c.worker_destinations) {
      auto results = find_path_to_destination(pad, destination, g, c);

      if (results.current_best_path >= 0) {
        // there is already a path to the destination, we don't need to add one,
        // unless we have too many workers on the way For now let's do nothing
      } else {
        // there isn't a path to the destination, we need to build one. Select
        // the closest building to the landing pad, ignoring those for which the
        // connection would be broken by another building or a transport.
        std::ranges::sort(
            results.target_buildings, [&g, &pad](building_id a, building_id b) {
              return distance_squared(pad.coords, g.get_building(a).coords) <
                     distance_squared(pad.coords, g.get_building(b).coords);
              ;
            });

        struct tube_by_priority {
          building_id start, end;
          int priority;
          resource_t cost;
        };
        std::vector<tube_by_priority> desired_tubes;
        ;
        for (auto &building : results.target_buildings) {
          if (already_requested.count(building_link{pad.identifier, building}) >
              0) {
            continue;
          }
          already_requested.emplace(building_link{pad.identifier, building});
          // The priority depends on the distance and the number of workers.
          // Number of workers increase the priority by a lot. Distance
          // decreases it a little. Let's go with workers squared divided by
          // distance.
          double distance = std::sqrt(
              distance_squared(pad.coords, g.get_building(building).coords));
          auto worker_count = c.worker_destinations[destination];
          int priority = (worker_count * worker_count).value / (int)distance;

          desired_tubes.push_back({pad.identifier, building, priority,
                                   compute_transport_cost(distance)});
        }

        constexpr static auto MAX_REQUEST = 1;
        int request_count = 0;
        for (auto &tube : desired_tubes) {
          // check if the transportation route is blocked
          if (can_build_tube(tube.start, tube.end, g)) {
            LOG_DEBUG("Want tube from ", tube.start, " to ", tube.end,
                      " with priority ", tube.priority, " and cost ",
                      tube.cost);
            actions.push_back(
                {build_tube{tube.start, tube.end}, tube.priority, tube.cost});
            std::vector<building_id> stops{tube.start, tube.end, tube.start};
            actions.push_back({build_pod{g.next_pod_id(), std::move(stops)},
                               tube.priority / 2, POD_COST});
          }
          if (request_count >= MAX_REQUEST) {
            goto next_pad;
          }
        }
      }
    }
  next_pad:
    if (actions.size() > 100) {
      break;
    }
  }

  std::ranges::sort(actions,
                    [](const desired_action &a, const desired_action &b) {
                      return a.priority > b.priority;
                    });
  std::vector<action> result;
  result.reserve(actions.size());
  resource_t available_resources = g.resources;
  for (auto &desired : actions) {
    if (desired.cost > available_resources) {
      if (desired.todo.index() == index_of<build_pod, action>) {
        g.missing_pods.push_back(desired);
      }
    } else {
      result.push_back(desired.todo);
      available_resources -= desired.cost;
    }
  }

  LOG_DEBUG("Expected resources after actions: ", available_resources);

  DEBUG_ONLY({
    std::cerr << "Desired actions: " << std::endl;
    for (auto &action : actions) {
      std::cerr << "\t(" << action.priority << ", " << action.cost << ") ";
      std::cerr << action.todo << std::endl;
    }
  });

  return result;
}

int main() {
  game g;

  // game loop
  while (1) {
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    std::cin >> g;
    std::cout << decide(g) << std::endl;
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::cerr << "Elapsed time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << "ms" << std::endl;
  }
}
