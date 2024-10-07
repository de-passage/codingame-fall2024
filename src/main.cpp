#include "actions.hpp"
#include "common.hpp"
#include "types.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

struct game {
  resource_t resources;
  std::vector<transportation> travel_routes;
  std::vector<pod> pods;
  std::vector<landing_pad> landing_pads;
  std::vector<building> buildings;
};

std::istream &operator>>(std::istream &in, game &g) {
  in >> g.resources;
  std::cerr << "resources: " << g.resources << std::endl;
  in.ignore();
  in >> g.travel_routes;

  std::cerr << "travel_routes: " << g.travel_routes.size() << std::endl;
  for (const auto &route : g.travel_routes) {
    std::cerr << "route: " << route.start_building << " -> "
              << route.end_building << " (" << route.capacity << ")"
              << std::endl;
  }
  in.ignore();
  in >> g.pods;
  std::cerr << "pods: " << g.pods.size() << std::endl;
  for (const auto &pod : g.pods) {
    std::cerr << "pod: " << pod.identifier << " -> ";
    for (const auto &stop : pod.stops) {
      std::cerr << stop << " ";
    }
    std::cerr << std::endl;
  }
  in.ignore();

  std::vector<any_building> buildings;
  in >> buildings;
  for (auto &building : buildings) {
    std::visit(
        overloaded{
            [&](landing_pad &&b) { g.landing_pads.push_back(std::move(b)); },
            [&](struct building &&b) { g.buildings.push_back(std::move(b)); }},
        std::move(building));
  }
  in.ignore();

  std::cerr << "landing_pads: " << g.landing_pads.size() << std::endl;
  for (const auto &pad : g.landing_pads) {
    std::cerr << "pad: " << pad.identifier << " (" << pad.coords.x << ", "
              << pad.coords.y << ") ";
    for (const auto &worker : pad.expected_workers) {
      std::cerr << worker << " ";
    }
    std::cerr << std::endl;
  }

  std::cerr << "buildings: " << g.buildings.size() << std::endl;
  for (const auto &building : g.buildings) {
    std::cerr << "building: " << building.identifier << " ("
              << building.coords.x << ", " << building.coords.y << ") "
              << building.type << std::endl;
  }
  return in;
}

int main() {
  game g;

  // game loop
  while (1) {
    std::cin >> g;
    actions a;
    a.push_back(build_tube{0_bid, 1_bid});
    a.push_back(build_tube{0_bid, 2_bid});
    a.push_back(build_pod{42_pid, {0_bid, 1_bid, 2_bid}});
    std::cout << a << std::endl;
  }
}
