#include <predefined_utils/benchmark/fixtures/flow_bench_fixtures.h>

#include <lib3611/w3_network_flow/network_flow.h>

#include <benchmark/benchmark.h>

using namespace dte3611::predef::benchmarking::flow::fixtures;

namespace alg = dte3611::np::algorithms;

BENCHMARK_DEFINE_F(RandomFlowNetworkF, EdmondsKarp)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    // Reset flows before each iteration
    auto [e_begin, e_end] = boost::edges(m_graph);
    for (auto it = e_begin; it != e_end; ++it) {
      m_graph[*it].flow = 0;
    }

    auto max_flow = alg::maxFlow(m_graph, m_source, m_sink);
    benchmark::DoNotOptimize(max_flow);
  }
}

BENCHMARK_DEFINE_F(RandomFlowNetworkF, CycleCanceling)
(benchmark::State& st)
{
  for ([[maybe_unused]] auto const& _ : st) {
    // Reset flows before each iteration
    auto [e_begin, e_end] = boost::edges(m_graph);
    for (auto it = e_begin; it != e_end; ++it) {
      m_graph[*it].flow = 0;
    }

    alg::cycleCanceling(m_graph);
    benchmark::DoNotOptimize(m_graph);
  }
}


BENCHMARK_REGISTER_F(RandomFlowNetworkF, EdmondsKarp)
  ->Arg(10)
  ->Arg(20)
  ->Arg(50)
  ->Arg(100)
  ->Arg(200)
  ->Arg(500)
  ->Arg(1000)
  ->Arg(2000);

BENCHMARK_REGISTER_F(RandomFlowNetworkF, CycleCanceling)
  ->Arg(10)
  ->Arg(20)
  ->Arg(50)
  ->Arg(100)
  ->Arg(200)
  ->Arg(500)
  ->Arg(1000)
  ->Arg(2000);


BENCHMARK_MAIN();