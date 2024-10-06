#pragma once

#include <vector>
#include <strong_types.hpp>

struct transportation {
  int start_building;
  int end_building;
  int capacity;
};

struct pod {
  int identifier;
  std::vector<building_id> stops;
};
