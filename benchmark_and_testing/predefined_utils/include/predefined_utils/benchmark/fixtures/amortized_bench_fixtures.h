#ifndef DTE3611_AMORTIZED_BENCH_FIXTURES_H
#define DTE3611_AMORTIZED_BENCH_FIXTURES_H

// google benchmark
#include <benchmark/benchmark.h>

// stl
#include <vector>
#include <memory_resource>
#include <random>
#include <algorithm>
#include <memory>

namespace dte3611::benchmarking::amortized::fixtures
{


template <typename Vector_T>
struct SortAllocatorBenchF : benchmark::Fixture {

  using benchmark::Fixture::Fixture;
  ~SortAllocatorBenchF() override = default;

  Vector_T m_data;
  std::vector<int> m_original;

  void TearDown(const benchmark::State&) override {
    m_data.clear();
    m_original.clear();
  }
};

// Standard allocator fixture
struct SortStdAllocatorF : SortAllocatorBenchF<std::vector<int>> {
  using Base = SortAllocatorBenchF<std::vector<int>>;
  using Base::Base;
  ~SortStdAllocatorF() override = default;

  void SetUp(const benchmark::State& st) final
  {
    auto const n = static_cast<size_t>(st.range(0));

    std::mt19937 rng(1231231231);
    std::uniform_int_distribution<int> dist(0, 1000000);

    m_original.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      m_original.push_back(dist(rng));
    }

    m_data = m_original;
  }
};


struct SortPmrMonotonicF : benchmark::Fixture {
  using benchmark::Fixture::Fixture;
  ~SortPmrMonotonicF() override = default;

  std::unique_ptr<std::byte[]> m_buffer;
  std::unique_ptr<std::pmr::monotonic_buffer_resource> m_resource;
  std::pmr::vector<int> m_data;
  std::vector<int> m_original;

  void SetUp(const benchmark::State& st) final
  {
    auto const n = static_cast<size_t>(st.range(0));

    size_t buffer_size = n * sizeof(int) * 2;
    m_buffer = std::make_unique<std::byte[]>(buffer_size);
    m_resource = std::make_unique<std::pmr::monotonic_buffer_resource>(
      m_buffer.get(), buffer_size, std::pmr::null_memory_resource());

    std::mt19937 rng(1231231231);
    std::uniform_int_distribution<int> dist(0, 1000000);

    m_original.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      m_original.push_back(dist(rng));
    }

    // Copy to PMR vector
    m_data = std::pmr::vector<int>(m_resource.get());
    m_data.reserve(n);
    for (int val : m_original) {
      m_data.push_back(val);
    }
  }

  void TearDown(const benchmark::State&) final
  {
    m_data.clear();
    m_original.clear();
    m_resource.reset();
    m_buffer.reset();
  }
};

// PMR pool allocator fixture
struct SortPmrPoolF : benchmark::Fixture {
  using benchmark::Fixture::Fixture;
  ~SortPmrPoolF() override = default;

  std::unique_ptr<std::pmr::unsynchronized_pool_resource> m_resource;
  std::pmr::vector<int> m_data;
  std::vector<int> m_original;

  void SetUp(const benchmark::State& st) final
  {
    auto const n = static_cast<size_t>(st.range(0));

    m_resource = std::make_unique<std::pmr::unsynchronized_pool_resource>();

    std::mt19937 rng(1231231231);
    std::uniform_int_distribution<int> dist(0, 1000000);

    m_original.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      m_original.push_back(dist(rng));
    }

    // Copy to PMR vector
    m_data = std::pmr::vector<int>(m_resource.get());
    m_data.reserve(n);
    for (int val : m_original) {
      m_data.push_back(val);
    }
  }

  void TearDown(const benchmark::State&) final
  {
    m_data.clear();
    m_original.clear();
    m_resource.reset();
  }
};


//=============================================================================
// Benchmark 2: Push_back amortized cost
//=============================================================================

// Measure push_back with pre-reserved capacity (no reallocations)
struct PushBackReservedF : benchmark::Fixture {
  using benchmark::Fixture::Fixture;
  ~PushBackReservedF() override = default;

  size_t m_n;

  void SetUp(const benchmark::State& st) final
  {
    m_n = static_cast<size_t>(st.range(0));
  }

  void TearDown(const benchmark::State&) final {}
};

// Measure push_back without reserve (with reallocations)
struct PushBackNoReserveF : benchmark::Fixture {
  using benchmark::Fixture::Fixture;
  ~PushBackNoReserveF() override = default;

  size_t m_n;

  void SetUp(const benchmark::State& st) final
  {
    m_n = static_cast<size_t>(st.range(0));
  }

  void TearDown(const benchmark::State&) final {}
};


//=============================================================================
// Benchmark 3: Map vs Unordered_map (for data structures comparison)
//=============================================================================

struct MapInsertF : benchmark::Fixture {
  using benchmark::Fixture::Fixture;
  ~MapInsertF() override = default;

  std::vector<int> m_keys;
  size_t m_n;

  void SetUp(const benchmark::State& st) final
  {
    m_n = static_cast<size_t>(st.range(0));

    std::mt19937 rng(1231231231);
    std::uniform_int_distribution<int> dist(0, 10000000);

    m_keys.reserve(m_n);
    for (size_t i = 0; i < m_n; ++i) {
      m_keys.push_back(dist(rng));
    }
  }

  void TearDown(const benchmark::State&) final
  {
    m_keys.clear();
  }
};

}   // namespace dte3611::benchmarking::amortized::fixtures

#endif   // DTE3611_AMORTIZED_BENCH_FIXTURES_H