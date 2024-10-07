#pragma once

#include "types.hpp"

struct segment {
  coordinates start, end;
};

int distance_squared(coordinates p1, coordinates p2);
int distance_squared(const segment &s1);

/// Returns true if q is on the segment pr
bool on_segment(const segment &s, coordinates q);


int signed_area_doubled(coordinates p1, coordinates p2, coordinates p3);
bool segments_intersect(const segment& s1, const segment& s2);
