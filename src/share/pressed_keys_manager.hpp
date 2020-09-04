#pragma once

// `krbn::pressed_keys_manager` can be used safely in a multi-threaded environment.

#include "types.hpp"
#include <unordered_set>

namespace krbn {
class pressed_keys_manager {
public:
  template <typename T>
  void insert(T value) {
    std::lock_guard<std::mutex> lock(mutex_);

    entries_.insert(key_down_up_valued_event(value));
  }

  template <typename T>
  void erase(T value) {
    std::lock_guard<std::mutex> lock(mutex_);

    entries_.erase(key_down_up_valued_event(value));
  }

  bool empty(void) const {
    std::lock_guard<std::mutex> lock(mutex_);

    return entries_.empty();
  }

private:
  std::unordered_set<key_down_up_valued_event> entries_;
  mutable std::mutex mutex_;
};
} // namespace krbn
