#include "utils.h"

uint64_t get_timestamps() {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  return now.time_since_epoch().count();
}