#include "types.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace std;

struct game {
  resource_t resources;
  std::vector<transportation> travel_routes;
  std::vector<pod> pods;
  std::vector<landing_pad> landing_pads;
  std::vector<building> buildings;
};


template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
std::istream &operator>>(std::istream &in, game &g) {
    in >> g.resources;
    in.ignore();
    g.travel_routes.clear();
    in >> g.travel_routes;
    in.ignore();
    in >> g.pods;
    in.ignore();

    std::vector<any_building> buildings;
    in >> buildings;
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
    return in;
}

int main() {
  game g;

  // game loop
  while (1) {

    cout << "TUBE 0 1;TUBE 0 2;POD 42 0 1 0 2 0 1 0 2"
         << endl; // TUBE | UPGRADE | TELEPORT | POD | DESTROY | WAIT
  }
}
