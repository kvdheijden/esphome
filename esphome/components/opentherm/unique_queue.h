#pragma once

#include <queue>
#include <unordered_set>
#include <functional>

namespace esphome {
namespace opentherm {

template<typename T, typename Hash = std::hash<T>, typename Pred = std::equal_to<T>> class unique_queue {
 public:
  template<typename... Args> bool emplace(Args &&...args) {
    auto ret = lookup.emplace(args...);
    if (ret.second) {
      store.push(*ret.first);
      return true;
    }

    return false;
  }

  [[nodiscard]] bool empty() const { return store.empty(); }

  [[nodiscard]] size_t size() const { return store.size(); }

  T pop_front() {
    T ret = store.front();
    lookup.erase(ret);
    store.pop();
    return ret;
  }

  void clear() {
    std::queue<T> empty;
    lookup.clear();
    std::swap(store, empty);
  }

 private:
  std::unordered_set<T, Hash, Pred> lookup;
  std::queue<T> store;
};

}  // namespace opentherm
}  // namespace esphome