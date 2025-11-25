#ifndef DTE3611_PREDEF_BENCHMARKING_FLOW_FIXTURES_H
#define DTE3611_PREDEF_BENCHMARKING_FLOW_FIXTURES_H

// google benchmark
#include <benchmark/benchmark.h>

// boost
#include <boost/graph/adjacency_list.hpp>

// stl
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <memory>

namespace dte3611::predef::benchmarking::flow::fixtures
{
  namespace detail
  {

    namespace types
    {

      namespace properties
      {

        struct NodeProperties {
          std::string name;
        };

        struct EdgeProperties {
          int flow;
          int capacity;
          int cost;
        };

        using GraphProperties = boost::no_property;

      }   // namespace properties

      using BidirectionalGraph
        = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                properties::NodeProperties,
                                properties::EdgeProperties,
                                properties::GraphProperties>;

    }   // namespace types


    /**
     * @brief Template for flow network benchmark fixtures
     */
    template <typename GraphType_T>
    struct FlowBenchmarkFixtureTemplate : benchmark::Fixture {

      using benchmark::Fixture::Fixture;
      ~FlowBenchmarkFixtureTemplate() override {}

      using Graph = GraphType_T;
      using VP    = typename Graph::vertex_property_type;
      using EP    = typename Graph::edge_property_type;
      using VD    = typename Graph::vertex_descriptor;

      Graph m_graph;
      VD m_source;
      VD m_sink;

      void TearDown(const benchmark::State&) override { m_graph.clear(); }
    };


    namespace networks
    {

      class RandomFlowNetworkF
        : public detail::FlowBenchmarkFixtureTemplate<detail::types::BidirectionalGraph> {

        using Base = detail::FlowBenchmarkFixtureTemplate<detail::types::BidirectionalGraph>;

      public:
        using Base::Base;
        ~RandomFlowNetworkF() override {}

        void SetUp(const benchmark::State& st) final
        {
          auto num_vertices = static_cast<std::size_t>(st.range(0));
          auto num_edges = num_vertices * 3; // Sparse: E = 3V

          // Seed varies with V to avoid "lucky/unlucky" fixed instances
          std::mt19937 rng(1 + num_vertices);
          std::uniform_int_distribution<int> cap_dist(1, 100);
          std::uniform_int_distribution<int> cost_dist(1, 20);

          if (num_vertices < 4) num_vertices = 4;

          std::vector<VD> vertices;
          vertices.reserve(num_vertices);
          for (std::size_t i = 0; i < num_vertices; ++i) {
            vertices.push_back(
              boost::add_vertex(VP{.name = "V" + std::to_string(i)}, m_graph));
          }

          m_source = vertices.front();
          m_sink = vertices.back();

          // Fixed 4 layers: source | layer1 | layer2 | sink
          // This creates WIDE layers where width grows with V
          constexpr std::size_t num_layers = 4;
          std::vector<std::vector<VD>> layers(num_layers);

          layers[0].push_back(m_source);
          layers[num_layers - 1].push_back(m_sink);

          // Distribute intermediate vertices evenly across layers 1 and 2
          // Each middle layer gets ~(V-2)/2 vertices
          for (std::size_t i = 1; i < num_vertices - 1; ++i) {
            std::size_t layer_idx = 1 + (i % 2); // Alternates between layer 1 and 2
            layers[layer_idx].push_back(vertices[i]);
          }

          std::size_t edges_added = 0;

          // Connect source to ALL vertices in layer 1 (degree grows with V)
          for (auto v : layers[1]) {
            if (edges_added >= num_edges) break;
            boost::add_edge(m_source, v,
              EP{.flow = 0, .capacity = cap_dist(rng), .cost = cost_dist(rng)}, m_graph);
            ++edges_added;
          }

          // Connect layer 1 to layer 2
          std::uniform_int_distribution<std::size_t> l2_dist(0, layers[2].size() - 1);
          for (auto u : layers[1]) {
            if (edges_added >= num_edges) break;
            // Each vertex in layer 1 connects to 2 random vertices in layer 2
            for (int k = 0; k < 2 && edges_added < num_edges; ++k) {
              auto v = layers[2][l2_dist(rng)];
              auto [existing, exists] = boost::edge(u, v, m_graph);
              if (!exists) {
                boost::add_edge(u, v,
                  EP{.flow = 0, .capacity = cap_dist(rng), .cost = cost_dist(rng)}, m_graph);
                ++edges_added;
              }
            }
          }

          // Connect ALL vertices in layer 2 to sink (degree grows with V)
          for (auto u : layers[2]) {
            if (edges_added >= num_edges) break;
            boost::add_edge(u, m_sink,
              EP{.flow = 0, .capacity = cap_dist(rng), .cost = cost_dist(rng)}, m_graph);
            ++edges_added;
          }

          // Add remaining random edges between layers 1 and 2
          std::uniform_int_distribution<std::size_t> l1_dist(0, layers[1].size() - 1);
          std::size_t attempts = 0;

          while (edges_added < num_edges && attempts < num_edges * 10) {
            ++attempts;
            auto u = layers[1][l1_dist(rng)];
            auto v = layers[2][l2_dist(rng)];

            auto [existing, exists] = boost::edge(u, v, m_graph);
            if (!exists) {
              boost::add_edge(u, v,
                EP{.flow = 0, .capacity = cap_dist(rng), .cost = cost_dist(rng)}, m_graph);
              ++edges_added;
            }
          }
        }
      };

    }   // namespace networks

  }   // namespace detail

  using RandomFlowNetworkF = detail::networks::RandomFlowNetworkF;

}   // namespace dte3611::predef::benchmarking::flow::fixtures

#endif   // DTE3611_PREDEF_BENCHMARKING_FLOW_FIXTURES_H