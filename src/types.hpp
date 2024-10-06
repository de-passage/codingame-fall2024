#pragma once

#include <strong_types.hpp>
#include <vector>

using building_id =
    dpsg::strong_types::strong_value<int, struct building_tag,
                                     dpsg::strong_types::comparable>;

struct transportation {
  building_id start_building;
  building_id end_building;
  int capacity;
};

struct pod {
  int identifier;
  std::vector<building_id> stops;
};
