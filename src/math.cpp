#include "math.hpp"

int distance_squared(coordinates p1, coordinates p2) {
  return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}
int distance_squared(const segment &s1) {
  return distance_squared(s1.start, s1.end);
}

bool on_segment(const segment &s, coordinates q) {
  return distance_squared(s.start, q) + distance_squared(s.end, q) ==
         distance_squared(s);
}

int signed_area_doubled(coordinates p1, coordinates p2, coordinates p3) {
  return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

bool segments_intersect(const segment &s1, const segment &s2) {
  int a1 = signed_area_doubled(s1.start, s1.end, s2.start);
  int a2 = signed_area_doubled(s1.start, s1.end, s2.end);
  int a3 = signed_area_doubled(s2.start, s2.end, s1.start);
  int a4 = signed_area_doubled(s2.start, s2.end, s1.end);

  return a1 * a2 < 0 && a3 * a4 < 0;
}
