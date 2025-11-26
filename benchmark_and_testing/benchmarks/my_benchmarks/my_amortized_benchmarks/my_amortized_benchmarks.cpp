#include <benchmark/benchmark.h>

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory_resource>
#include <random>
#include <algorithm>

// ############################################################################
// Amortized Analysis: push_back with/without reserve
// ############################################################################

struct PushBackBenchBase : benchmark::Fixture {
  size_t m_n;

  void SetUp(const benchmark::State& st) override {
    m_n = static_cast<size_t>(st.range(0));
  }

  void TearDown(const benchmark::State&) override {}
};

struct PushBackReservedF : PushBackBenchBase {
  using PushBackBenchBase::PushBackBenchBase;
};

BENCHMARK_DEFINE_F(PushBackReservedF, withReserve)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::vector<int> vec;
    vec.reserve(m_n);  // Pre-allocate

    for (size_t i = 0; i < m_n; ++i) {
      vec.push_back(static_cast<int>(i));
    }

    benchmark::DoNotOptimize(vec.data());
    benchmark::ClobberMemory();
  }
}

struct PushBackNoReserveF : PushBackBenchBase {
  using PushBackBenchBase::PushBackBenchBase;
};

BENCHMARK_DEFINE_F(PushBackNoReserveF, withoutReserve)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::vector<int> vec;
    // No reserve - triggers reallocations

    for (size_t i = 0; i < m_n; ++i) {
      vec.push_back(static_cast<int>(i));
    }

    benchmark::DoNotOptimize(vec.data());
    benchmark::ClobberMemory();
  }
}


// ############################################################################
// Data Structures: Map vs Unordered_map
// ############################################################################

static std::vector<int> generateKeys(size_t n, unsigned seed = 42) {
  std::vector<int> keys;
  keys.reserve(n);
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> dist(0, 10000000);
  for (size_t i = 0; i < n; ++i) {
    keys.push_back(dist(rng));
  }
  return keys;
}

struct MapInsertF : benchmark::Fixture {
  std::vector<int> m_keys;
  size_t m_n;

  void SetUp(const benchmark::State& st) override {
    m_n = static_cast<size_t>(st.range(0));
    m_keys = generateKeys(m_n, 42);
  }

  void TearDown(const benchmark::State&) override {
    m_keys.clear();
  }
};

BENCHMARK_DEFINE_F(MapInsertF, stdMap)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::map<int, int> map;

    for (size_t i = 0; i < m_n; ++i) {
      map[m_keys[i]] = static_cast<int>(i);
    }

    benchmark::DoNotOptimize(map.size());
    benchmark::ClobberMemory();
  }
}

BENCHMARK_DEFINE_F(MapInsertF, stdUnorderedMap)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::unordered_map<int, int> map;

    for (size_t i = 0; i < m_n; ++i) {
      map[m_keys[i]] = static_cast<int>(i);
    }

    benchmark::DoNotOptimize(map.size());
    benchmark::ClobberMemory();
  }
}

struct MapLookupF : benchmark::Fixture {
  std::map<int, int> m_map;
  std::unordered_map<int, int> m_unordered_map;
  std::vector<int> m_lookup_keys;
  size_t m_n;

  void SetUp(const benchmark::State& st) override {
    m_n = static_cast<size_t>(st.range(0));

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 10000000);

    for (size_t i = 0; i < m_n; ++i) {
      int key = dist(rng);
      m_map[key] = static_cast<int>(i);
      m_unordered_map[key] = static_cast<int>(i);
      m_lookup_keys.push_back(key);
    }

    std::shuffle(m_lookup_keys.begin(), m_lookup_keys.end(), rng);
  }

  void TearDown(const benchmark::State&) override {
    m_map.clear();
    m_unordered_map.clear();
    m_lookup_keys.clear();
  }
};

BENCHMARK_DEFINE_F(MapLookupF, stdMapLookup)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    int sum = 0;
    for (int key : m_lookup_keys) {
      sum += m_map[key];
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK_DEFINE_F(MapLookupF, stdUnorderedMapLookup)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    int sum = 0;
    for (int key : m_lookup_keys) {
      sum += m_unordered_map[key];
    }
    benchmark::DoNotOptimize(sum);
  }
}

// ############################################################################
// Register Benchmarks
// ############################################################################

BENCHMARK_REGISTER_F(PushBackReservedF, withReserve)
  ->RangeMultiplier(10)
  ->Range(1000, 1000000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(PushBackNoReserveF, withoutReserve)
  ->RangeMultiplier(10)
  ->Range(1000, 1000000)
  ->Unit(benchmark::kMicrosecond);

// Data structures: map vs unordered_map
BENCHMARK_REGISTER_F(MapInsertF, stdMap)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(MapInsertF, stdUnorderedMap)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(MapLookupF, stdMapLookup)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(MapLookupF, stdUnorderedMapLookup)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();