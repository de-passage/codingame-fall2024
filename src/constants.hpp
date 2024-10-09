#pragma once

#include "types.hpp"
#include <cmath>

constexpr static inline double TUBE_COST{0.1};
constexpr static inline resource_t UPGRADE_COST{2}; // Initial cost * new capacity
constexpr static inline resource_t TELEPORTER_COST{500};
constexpr static inline resource_t POD_COST{1000};
constexpr static inline resource_t DESTROY_COST{-750};
constexpr static inline resource_t POD_RELOCATION_COST{POD_COST + DESTROY_COST};

inline resource_t compute_transport_cost(double distance) {
  return resource_t{std::floor(distance * TUBE_COST)};
}
