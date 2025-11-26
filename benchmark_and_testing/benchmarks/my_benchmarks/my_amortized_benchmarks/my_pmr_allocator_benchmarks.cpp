#include <benchmark/benchmark.h>

#include <map>
#include <vector>
#include <string>
#include <memory_resource>
#include <random>
#include <array>

// ############################################################################
// PMR Allocator Impact: Map Insertions with Node Allocations
// ############################################################################
static std::vector<int> generateKeys(size_t n) {
  std::vector<int> keys;
  keys.reserve(n);
  std::mt19937 rng(1212312);
  std::uniform_int_distribution<int> dist(0, 10000000);
  for (size_t i = 0; i < n; ++i) {
    keys.push_back(dist(rng));
  }
  return keys;
}


struct MapAllocatorBenchBase : benchmark::Fixture {
  std::vector<int> m_keys;
  size_t m_n;

  void SetUp(const benchmark::State& st) override {
    m_n = static_cast<size_t>(st.range(0));
    m_keys = generateKeys(m_n);
  }

  void TearDown(const benchmark::State&) override {
    m_keys.clear();
  }
};

struct MapStdAllocatorF : MapAllocatorBenchBase {
  using MapAllocatorBenchBase::MapAllocatorBenchBase;
};

BENCHMARK_DEFINE_F(MapStdAllocatorF, insertions)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    // Create map, insert n elements, destroy map ALL in the benchmark loop
    std::map<int, int> map;
    for (size_t i = 0; i < m_n; ++i) {
      map[m_keys[i]] = static_cast<int>(i);
    }
    benchmark::DoNotOptimize(map.size());
    benchmark::ClobberMemory();
  }
}

struct MapPmrMonotonicF : MapAllocatorBenchBase {
  using MapAllocatorBenchBase::MapAllocatorBenchBase;
};

BENCHMARK_DEFINE_F(MapPmrMonotonicF, insertions)
(benchmark::State& st)
{
  const size_t buffer_size = m_n * 64;
  std::vector<std::byte> buffer(buffer_size);

  for ([[maybe_unused]] auto const& _ : st) {
    std::pmr::monotonic_buffer_resource resource(
      buffer.data(), buffer.size(), std::pmr::null_memory_resource());

    std::pmr::map<int, int> map(&resource);

    for (size_t i = 0; i < m_n; ++i) {
      map[m_keys[i]] = static_cast<int>(i);
    }
    benchmark::DoNotOptimize(map.size());
    benchmark::ClobberMemory();
  }
}

struct MapPmrPoolF : MapAllocatorBenchBase {
  using MapAllocatorBenchBase::MapAllocatorBenchBase;
};

BENCHMARK_DEFINE_F(MapPmrPoolF, insertions)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    // Create fresh pool resource for each iteration
    std::pmr::unsynchronized_pool_resource resource;

    std::pmr::map<int, int> map(&resource);

    for (size_t i = 0; i < m_n; ++i) {
      map[m_keys[i]] = static_cast<int>(i);
    }
    benchmark::DoNotOptimize(map.size());
    benchmark::ClobberMemory();
  }
}


// ############################################################################
// PMR Allocator Impact: Vector of Strings (many small string allocations)
// ############################################################################

static std::vector<std::string> generateStrings(size_t n) {
  std::vector<std::string> strings;
  strings.reserve(n);
  std::mt19937 rng(1212312);
  std::uniform_int_distribution<int> len_dist(20, 100);
  std::uniform_int_distribution<int> char_dist('a', 'z');

  for (size_t i = 0; i < n; ++i) {
    size_t len = len_dist(rng);
    std::string s;
    s.reserve(len);
    for (size_t j = 0; j < len; ++j) {
      s.push_back(static_cast<char>(char_dist(rng)));
    }
    strings.push_back(std::move(s));
  }
  return strings;
}

struct StringVectorBenchBase : benchmark::Fixture {
  std::vector<std::string> m_strings;
  size_t m_n;

  void SetUp(const benchmark::State& st) override {
    m_n = static_cast<size_t>(st.range(0));
    m_strings = generateStrings(m_n);
  }

  void TearDown(const benchmark::State&) override {
    m_strings.clear();
  }
};

struct StringVecStdAllocatorF : StringVectorBenchBase {
  using StringVectorBenchBase::StringVectorBenchBase;
};

BENCHMARK_DEFINE_F(StringVecStdAllocatorF, copyStrings)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::vector<std::string> vec;
    vec.reserve(m_n);
    for (const auto& s : m_strings) {
      vec.push_back(s);
    }
    benchmark::DoNotOptimize(vec.data());
    benchmark::ClobberMemory();
  }
}

struct StringVecPmrMonotonicF : StringVectorBenchBase {
  using StringVectorBenchBase::StringVectorBenchBase;
};

BENCHMARK_DEFINE_F(StringVecPmrMonotonicF, copyStrings)
(benchmark::State& st)
{
  const size_t buffer_size = m_n * 128;
  std::vector<std::byte> buffer(buffer_size);

  for ([[maybe_unused]] auto const& _ : st) {
    std::pmr::monotonic_buffer_resource resource(
      buffer.data(), buffer.size(), std::pmr::null_memory_resource());

    std::pmr::vector<std::pmr::string> vec(&resource);
    vec.reserve(m_n);

    for (const auto& s : m_strings) {
      vec.emplace_back(s);
    }
    benchmark::DoNotOptimize(vec.data());
    benchmark::ClobberMemory();
  }
}

struct StringVecPmrPoolF : StringVectorBenchBase {
  using StringVectorBenchBase::StringVectorBenchBase;
};

BENCHMARK_DEFINE_F(StringVecPmrPoolF, copyStrings)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::pmr::unsynchronized_pool_resource resource;

    std::pmr::vector<std::pmr::string> vec(&resource);
    vec.reserve(m_n);

    for (const auto& s : m_strings) {
      vec.emplace_back(s);
    }
    benchmark::DoNotOptimize(vec.data());
    benchmark::ClobberMemory();
  }
}


BENCHMARK_REGISTER_F(MapStdAllocatorF, insertions)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(MapPmrMonotonicF, insertions)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(MapPmrPoolF, insertions)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(StringVecStdAllocatorF, copyStrings)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(StringVecPmrMonotonicF, copyStrings)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(StringVecPmrPoolF, copyStrings)
  ->RangeMultiplier(10)
  ->Range(1000, 100000)
  ->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();