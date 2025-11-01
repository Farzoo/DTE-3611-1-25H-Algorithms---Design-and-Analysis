#include <lib3611/w2_subset_sum/subset_sum.h>

// google benchmark
#include <benchmark/benchmark.h>

// stl
#include <vector>
#include <random>
#include <numeric>

namespace alg = dte3611::np::algorithms;

struct SubsetSumBenchFixtureTemplate : benchmark::Fixture {

  using benchmark::Fixture::Fixture;
  ~SubsetSumBenchFixtureTemplate() override {}

  using Vector = std::vector<int>;

  Vector m_numbers;
  int m_target;

  void TearDown(const benchmark::State&) override { m_numbers.clear(); }
};

// Concrete fixtures
struct SubsetSumBenchF : SubsetSumBenchFixtureTemplate {

  using Base = SubsetSumBenchFixtureTemplate;
  using Base::Base;
  ~SubsetSumBenchF() override {}

  void SetUp(const benchmark::State& st) final
  {
    auto const n = st.range(0);

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 100);

    m_numbers.reserve(n);
    for (auto i = 0; i < n; ++i) {
      m_numbers.emplace_back(dist(rng));
    }

    // Target = ~40% of total sum
    int total = std::accumulate(m_numbers.begin(), m_numbers.end(), 0);
    m_target = total * 4 / 10;
  }
};

BENCHMARK_DEFINE_F(SubsetSumBenchF, backtracking)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    [[maybe_unused]] auto result
      = alg::subset_sum(m_numbers, m_target, std::identity{});
  }
}

BENCHMARK_DEFINE_F(SubsetSumBenchF, dp)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    [[maybe_unused]] auto result
      = alg::subset_sum_dp(m_numbers, m_target, std::identity{});
  }
}

// Backtracking: limited to n <= 24
BENCHMARK_REGISTER_F(SubsetSumBenchF, backtracking)
  ->Arg(10)
  ->Arg(12)
  ->Arg(14)
  ->Arg(16)
  ->Arg(18)
  ->Arg(20)
  ->Arg(22)
  ->Arg(24);

// DP: same range for comparison
BENCHMARK_REGISTER_F(SubsetSumBenchF, dp)
  ->Arg(10)
  ->Arg(12)
  ->Arg(14)
  ->Arg(16)
  ->Arg(18)
  ->Arg(20)
  ->Arg(22)
  ->Arg(24);

BENCHMARK_MAIN();