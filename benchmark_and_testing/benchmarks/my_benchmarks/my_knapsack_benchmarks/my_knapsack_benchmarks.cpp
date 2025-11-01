#include <lib3611/w2_knapsack/knapsack.h>

// google benchmark
#include <benchmark/benchmark.h>

// stl
#include <vector>
#include <random>

namespace alg = dte3611::np::algorithms;

namespace detail {
  struct BenchItem {
    int value;
    int weight;
  };
}

using namespace detail;

struct KnapsackBenchFixtureTemplate : benchmark::Fixture {

  using benchmark::Fixture::Fixture;
  ~KnapsackBenchFixtureTemplate() override {}

  using Items = std::vector<BenchItem>;

  Items m_items;
  int m_capacity;

  void TearDown(const benchmark::State&) override { m_items.clear(); }
};

// Concrete fixtures
struct KnapsackBenchF : KnapsackBenchFixtureTemplate {

  using Base = KnapsackBenchFixtureTemplate;
  using Base::Base;
  ~KnapsackBenchF() override {}

  void SetUp(const benchmark::State& st) final
  {
    auto const n = st.range(0);

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> val_dist(1, 100);
    std::uniform_int_distribution<int> wt_dist(1, 50);

    m_items.reserve(n);
    int total_weight = 0;
    for (auto i = 0; i < n; ++i) {
      m_items.emplace_back(BenchItem{val_dist(rng), wt_dist(rng)});
      total_weight += m_items.back().weight;
    }

    // Capacity = ~50% of total weight
    m_capacity = total_weight / 2;
  }
};

BENCHMARK_DEFINE_F(KnapsackBenchF, dp)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::vector<bool> selection;
    [[maybe_unused]] auto result = alg::knapsack_01(
      m_items, std::back_inserter(selection), m_capacity,
      &BenchItem::value, &BenchItem::weight);
  }
}

BENCHMARK_DEFINE_F(KnapsackBenchF, bruteForce)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    std::vector<bool> selection;
    [[maybe_unused]] auto result = alg::knapsack_01_bruteforce(
      m_items, std::back_inserter(selection), m_capacity,
      &BenchItem::value, &BenchItem::weight);
  }
}

// Brute force: limited to n <= 25
BENCHMARK_REGISTER_F(KnapsackBenchF, bruteForce)
  ->Arg(10)
  ->Arg(12)
  ->Arg(14)
  ->Arg(16)
  ->Arg(18)
  ->Arg(20)
  ->Arg(22)
  ->Arg(24)
  ->Arg(25);

// DP: same range for comparison
BENCHMARK_REGISTER_F(KnapsackBenchF, dp)
  ->Arg(10)
  ->Arg(12)
  ->Arg(14)
  ->Arg(16)
  ->Arg(18)
  ->Arg(20)
  ->Arg(22)
  ->Arg(24)
  ->Arg(25);

// DP only: larger sizes
BENCHMARK_REGISTER_F(KnapsackBenchF, dp)
  ->Arg(50)
  ->Arg(100)
  ->Arg(500)
  ->Arg(1000);

BENCHMARK_MAIN();