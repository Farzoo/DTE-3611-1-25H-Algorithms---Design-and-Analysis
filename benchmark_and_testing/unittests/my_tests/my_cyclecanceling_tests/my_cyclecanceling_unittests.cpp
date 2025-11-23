#include <predefined_utils/testing/fixtures/flow_testing_fixtures.h>

#include <lib3611/w3_network_flow/network_flow.h>

// gtest
#include <gtest/gtest.h>

// stl
#include <vector>
#include <numeric>

using namespace dte3611::predef::testing::flow::fixtures;
using namespace dte3611::predef::testing::flow::detail;


namespace alg = dte3611::np::algorithms;

namespace {


// Helper function to calculate total flow cost
template <typename Graph_T>
int calculateFlowCost(Graph_T const& graph)
{
  int total_cost = 0;
  auto [edge_begin, edge_end] = boost::edges(graph);
  for (auto it = edge_begin; it != edge_end; ++it) {
    total_cost += graph[*it].flow * graph[*it].cost;
  }
  return total_cost;
}

// Helper function to calculate total flow value from source
template <typename Graph_T>
int calculateFlowValue(Graph_T const& graph,
                       typename Graph_T::vertex_descriptor const& source)
{
  int total_flow = 0;
  auto [out_begin, out_end] = boost::out_edges(source, graph);
  for (auto it = out_begin; it != out_end; ++it) {
    total_flow += graph[*it].flow;
  }
  return total_flow;
}

}


TEST_F(MinCostMaxFlowDAG, CycleCancelingPreservesMaxFlow)
{
  // First get the max flow using standard algorithm
  auto& graph = gold->graph();
  int max_flow_before = alg::maxFlow(graph, gold->s(), gold->t());

  // Reset and run cycleCanceling
  auto [edge_begin, edge_end] = boost::edges(graph);
  for (auto it = edge_begin; it != edge_end; ++it) {
    graph[*it].flow = 0;
  }

  alg::cycleCanceling(graph);

  int flow_after = calculateFlowValue(graph, gold->s());

  // Flow should still be maximum
  EXPECT_EQ(flow_after, max_flow_before);
  EXPECT_EQ(flow_after, gold->maxFlowSTGold());
}


TEST_F(MinCostMaxFlowDAG, CycleCancelingProducesValidFlow)
{
  auto& graph = gold->graph();
  alg::cycleCanceling(graph);

  auto [edge_begin, edge_end] = boost::edges(graph);
  for (auto it = edge_begin; it != edge_end; ++it) {
    auto flow = graph[*it].flow;
    auto capacity = graph[*it].capacity;
    EXPECT_GE(flow, 0) << "Flow should be non-negative";
    EXPECT_LE(flow, capacity) << "Flow should not exceed capacity";
  }

  // Check flow conservation at intermediate nodes
  auto [v_begin, v_end] = boost::vertices(graph);
  for (auto it = v_begin; it != v_end; ++it) {
    auto v = *it;

    // Skip source and sink
    if (v == gold->s() || v == gold->t()) continue;

    int flow_in = 0;
    int flow_out = 0;

    auto [in_begin, in_end] = boost::in_edges(v, graph);
    for (auto e_it = in_begin; e_it != in_end; ++e_it) {
      flow_in += graph[*e_it].flow;
    }

    auto [out_begin, out_end] = boost::out_edges(v, graph);
    for (auto e_it = out_begin; e_it != out_end; ++e_it) {
      flow_out += graph[*e_it].flow;
    }

    EXPECT_EQ(flow_in, flow_out) << "Flow conservation violated at node " << v;
  }
}


TEST_F(MinCostMaxFlowDAG, CycleCancelingReducesCost)
{
  auto& graph = gold->graph();

  alg::maxFlow(graph, gold->s(), gold->t());
  int cost_maxflow = calculateFlowCost(graph);

  // Reset and run cycleCanceling
  auto [edge_begin, edge_end] = boost::edges(graph);
  for (auto it = edge_begin; it != edge_end; ++it) {
    graph[*it].flow = 0;
  }

  alg::cycleCanceling(graph);
  int cost_mincost = calculateFlowCost(graph);

  EXPECT_LE(cost_mincost, cost_maxflow);

  std::cout << "MaxFlow cost: " << cost_maxflow << std::endl;
  std::cout << "MinCost flow cost: " << cost_mincost << std::endl;
}


// Simple test case with known optimal solution
TEST(CycleCancelingSimple, DiamondGraph)
{

  types::BidirectionalGraph graph;

  auto S = boost::add_vertex(types::properties::NodeProperties{.name = "S"}, graph);
  auto A = boost::add_vertex(types::properties::NodeProperties{.name = "A"}, graph);
  auto B = boost::add_vertex(types::properties::NodeProperties{.name = "B"}, graph);
  auto T = boost::add_vertex(types::properties::NodeProperties{.name = "T"}, graph);

  using EP = types::properties::EdgeProperties;

  boost::add_edge(S, A, EP{.flow = 0, .capacity = 10, .cost = 1}, graph);
  boost::add_edge(S, B, EP{.flow = 0, .capacity = 10, .cost = 10}, graph);
  boost::add_edge(A, T, EP{.flow = 0, .capacity = 10, .cost = 0}, graph);
  boost::add_edge(B, T, EP{.flow = 0, .capacity = 10, .cost = 0}, graph);

  alg::cycleCanceling(graph);

  int flow_through_A = 0;
  int flow_through_B = 0;

  auto [e_begin, e_end] = boost::edges(graph);
  for (auto it = e_begin; it != e_end; ++it) {
    auto src = boost::source(*it, graph);
    auto tgt = boost::target(*it, graph);
    if (src == S && tgt == A) flow_through_A = graph[*it].flow;
    if (src == S && tgt == B) flow_through_B = graph[*it].flow;
  }

  int total_cost = calculateFlowCost(graph);

  std::cout << "Diamond graph - Flow through A: " << flow_through_A
            << ", Flow through B: " << flow_through_B
            << ", Total cost: " << total_cost << std::endl;

  EXPECT_EQ(flow_through_A + flow_through_B, 20);
  EXPECT_EQ(total_cost, 110);
}