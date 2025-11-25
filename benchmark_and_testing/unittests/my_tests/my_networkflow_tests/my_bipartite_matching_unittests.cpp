#include <predefined_utils/testing/fixtures/flow_testing_fixtures.h>
#include <lib3611/w3_network_flow/network_flow.h>

// gtest
#include <gtest/gtest.h>

// stl
#include <vector>
#include <iostream>

using namespace dte3611::predef::testing::flow::detail;

namespace alg = dte3611::np::algorithms;

namespace {


TEST(BipartiteMatching, PerfectMatchingExists)
{
  types::BidirectionalGraph graph;

  using VP = types::properties::NodeProperties;
  using EP = types::properties::EdgeProperties;

  auto S = boost::add_vertex(VP{.name = "S"}, graph);
  auto T = boost::add_vertex(VP{.name = "T"}, graph);

  auto L1 = boost::add_vertex(VP{.name = "L1"}, graph);
  auto L2 = boost::add_vertex(VP{.name = "L2"}, graph);
  auto L3 = boost::add_vertex(VP{.name = "L3"}, graph);

  auto R1 = boost::add_vertex(VP{.name = "R1"}, graph);
  auto R2 = boost::add_vertex(VP{.name = "R2"}, graph);
  auto R3 = boost::add_vertex(VP{.name = "R3"}, graph);


  boost::add_edge(S, L1, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, L2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, L3, EP{.flow = 0, .capacity = 1}, graph);

  boost::add_edge(L1, R1, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L1, R2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L2, R2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L2, R3, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L3, R3, EP{.flow = 0, .capacity = 1}, graph);

  boost::add_edge(R1, T, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(R2, T, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(R3, T, EP{.flow = 0, .capacity = 1}, graph);

  int max_matching = alg::maxFlow(graph, S, T);

  std::cout << "Bipartite Matching Test 1 (Perfect Matching):" << std::endl;
  std::cout << "  |L| = 3, |R| = 3" << std::endl;
  std::cout << "  Max matching size = " << max_matching << std::endl;

  EXPECT_EQ(max_matching, 3);
}


TEST(BipartiteMatching, ImperfectMatching)
{
  types::BidirectionalGraph graph;

  using VP = types::properties::NodeProperties;
  using EP = types::properties::EdgeProperties;

  auto S = boost::add_vertex(VP{.name = "S"}, graph);
  auto T = boost::add_vertex(VP{.name = "T"}, graph);

  auto L1 = boost::add_vertex(VP{.name = "L1"}, graph);
  auto L2 = boost::add_vertex(VP{.name = "L2"}, graph);
  auto L3 = boost::add_vertex(VP{.name = "L3"}, graph);
  auto L4 = boost::add_vertex(VP{.name = "L4"}, graph);

  auto R1 = boost::add_vertex(VP{.name = "R1"}, graph);
  auto R2 = boost::add_vertex(VP{.name = "R2"}, graph);


  boost::add_edge(S, L1, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, L2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, L3, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, L4, EP{.flow = 0, .capacity = 1}, graph);

  boost::add_edge(L1, R1, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L2, R1, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L2, R2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L3, R2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(L4, R2, EP{.flow = 0, .capacity = 1}, graph);

  boost::add_edge(R1, T, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(R2, T, EP{.flow = 0, .capacity = 1}, graph);

  int max_matching = alg::maxFlow(graph, S, T);

  std::cout << "Bipartite Matching Test 2 (Imperfect Matching):" << std::endl;
  std::cout << "  |L| = 4, |R| = 2" << std::endl;
  std::cout << "  Max matching size = " << max_matching << std::endl;

  EXPECT_EQ(max_matching, 2);
}


/**
 * @brief Job assignment problem modeled as bipartite matching
 *
 * Workers = {Alice, Bob, Charlie}
 * Jobs = {Job1, Job2, Job3}
 *
 * Qualifications:
 *   Alice can do Job1, Job2
 *   Bob can do Job2, Job3
 *   Charlie can do Job1
 *
 * Question: Can we assign each worker to exactly one job?
 * Answer: Yes, max matching = 3 (Alice-Job2, Bob-Job3, Charlie-Job1)
 */
TEST(BipartiteMatching, JobAssignmentProblem)
{
  types::BidirectionalGraph graph;

  using VP = types::properties::NodeProperties;
  using EP = types::properties::EdgeProperties;

  auto S = boost::add_vertex(VP{.name = "S"}, graph);
  auto T = boost::add_vertex(VP{.name = "T"}, graph);
  auto Alice= boost::add_vertex(VP{.name = "Alice"}, graph);
  auto Bob = boost::add_vertex(VP{.name = "Bob"}, graph);
  auto Charlie = boost::add_vertex(VP{.name = "Charlie"}, graph);
  auto Job1 = boost::add_vertex(VP{.name = "Job1"}, graph);
  auto Job2 = boost::add_vertex(VP{.name = "Job2"}, graph);
  auto Job3 = boost::add_vertex(VP{.name = "Job3"}, graph);

  // Source to Workers
  boost::add_edge(S, Alice, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, Bob, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(S, Charlie, EP{.flow = 0, .capacity = 1}, graph);

  // Worker-Job qualifications
  boost::add_edge(Alice, Job1, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(Alice, Job2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(Bob, Job2, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(Bob, Job3, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(Charlie, Job1, EP{.flow = 0, .capacity = 1}, graph);

  // Jobs to Sink
  boost::add_edge(Job1, T, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(Job2, T, EP{.flow = 0, .capacity = 1}, graph);
  boost::add_edge(Job3, T, EP{.flow = 0, .capacity = 1}, graph);

  int max_matching = alg::maxFlow(graph, S, T);

  std::cout << "Bipartite Matching Test 3 (Job Assignment):" << std::endl;
  std::cout << "  Workers = {Alice, Bob, Charlie}" << std::endl;
  std::cout << "  Jobs = {Job1, Job2, Job3}" << std::endl;
  std::cout << "  Max assignment = " << max_matching << std::endl;


  EXPECT_EQ(max_matching, 3);

  // Verify the matching by checking flows
  std::cout << "  Assigned jobs:" << std::endl;
  auto [e_begin, e_end] = boost::edges(graph);
  for (auto it = e_begin; it != e_end; ++it) {
    if (graph[*it].flow == 1) {
      auto src = boost::source(*it, graph);
      auto tgt = boost::target(*it, graph);

      if (src != S && tgt != T) {
        std::cout << "    " << graph[src].name << " -> " << graph[tgt].name << std::endl;
      }
    }
  }
}

}