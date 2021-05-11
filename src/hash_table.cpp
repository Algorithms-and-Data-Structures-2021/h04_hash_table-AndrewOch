#include "hash_table.hpp"

#include <stdexcept>

namespace itis {

  int HashTable::hash(int key) const {
    return utils::hash(key, static_cast<int>(buckets_.size()));
  }

  HashTable::HashTable(int capacity, double load_factor) : load_factor_{load_factor} {
    if (capacity <= 0) {
      throw std::logic_error("hash table capacity must be greater than zero");
    }

    if (load_factor <= 0.0 || load_factor > 1.0) {
      throw std::logic_error("hash table load factor must be in range [0...1]");
    }
    for (int i = 0; i < capacity; ++i) {
      buckets_.emplace_back();
    }
  }

  std::optional<std::string> HashTable::Search(int key) const {
    int index = hash(key);
    auto bucket = buckets_[index];
    if (!bucket.empty()) {
      for (auto pair : bucket) {
        if (pair.first == key) {
          return pair.second;
        }
      }
    }
    return std::nullopt;
  }

  void HashTable::Put(int key, const std::string &value) {
    int index = hash(key);
    bool alreadyHasKey = false;

    if (!buckets_[index].empty()) {
      for (auto &pair : buckets_[index]) {
        if (pair.first == key) {
          pair.second = value;
          alreadyHasKey = true;
          break;
        }
      }
    }
    if (!alreadyHasKey) {
      buckets_[index].emplace_back(key, value);
    }
    num_keys_++;
    if (static_cast<double>(num_keys_) / capacity() >= load_factor_) {
      std::vector<Bucket> newBuckets(capacity() * kGrowthCoefficient);
      std::list<std::pair<int, std::string>> empty_list;
      for (auto list : buckets_) {
        if (!list.empty()) {
          int newIndex = utils::hash(list.front().first, newBuckets.size());
          newBuckets[newIndex] = list;
        }
      }
      buckets_ = newBuckets;
    }
  }

  std::optional<std::string> HashTable::Remove(int key) {
    int index = hash(key);
    if (!buckets_[index].empty()) {
      std::pair<int, std::string> removingPair;
      bool hasKey = false;
      for (auto pair : buckets_[index]) {
        if (pair.first == key) {
          removingPair = pair;
          hasKey = true;
          break;
        }
      }
      if (hasKey) {
        buckets_[index].remove(removingPair);
        return removingPair.second;
      }
    }
    return std::nullopt;
  }

  bool HashTable::ContainsKey(int key) const {
    return Search(key).has_value();
  }

  bool HashTable::empty() const {
    return size() == 0;
  }

  int HashTable::size() const {
    return num_keys_;
  }

  int HashTable::capacity() const {
    return static_cast<int>(buckets_.size());
  }

  double HashTable::load_factor() const {
    return load_factor_;
  }

  std::unordered_set<int> HashTable::keys() const {
    std::unordered_set<int> keys(num_keys_);
    for (const auto &bucket : buckets_) {
      for (const auto &[key, _] : bucket) {
        keys.insert(key);
      }
    }
    return keys;
  }

  std::vector<std::string> HashTable::values() const {
    std::vector<std::string> values;
    for (const auto &bucket : buckets_) {
      for (const auto &[_, value] : bucket) {
        values.push_back(value);
      }
    }
    return values;
  }

}  // namespace itis