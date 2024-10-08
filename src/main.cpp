#include "actions.hpp"
#include "common.hpp"
#include "game.hpp"
#include "types.hpp"

#include <algorithm>
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
  std::unordered_map<building_id, building_id> direct_connections;
  std::unordered_map<building_type, worker_count> worker_destinations;
};

actions decide(const game &g) {
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

  // Compute the shortest path from landing_pad to required building
  for (auto &pad : g.landing_pads) {
    for (auto worker : pad.expected_workers) {
      c.worker_destinations[worker]++;
    }

    for (auto &[destination, _] : c.worker_destinations) {
      std::queue<building_id> to_visit;
      std::unordered_set<building_id> visited;
      for (auto &pad : g.landing_pads) {
      }
    }
  }

  return {};
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
