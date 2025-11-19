#ifndef DTE3611_WEEK3_NETWORK_FLOW_H
#define DTE3611_WEEK3_NETWORK_FLOW_H

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <optional>

// boost
#include <boost/graph/edge_list.hpp>

#include "operators.h"

namespace dte3611::np::algorithms
{

    namespace detail {

      template <typename EdgeDescriptor>
      struct ResidualEdge {
        EdgeDescriptor original_edge;
        bool is_forward;
        int residual_capacity;
      };

      template <typename Graph_T, typename VertexDescriptor, typename EdgeDescriptor>
      std::optional<std::pair<std::vector<ResidualEdge<EdgeDescriptor>>, int>>
      findAugmentingPathBFS(
          Graph_T const& graph,
          VertexDescriptor const& source,
          VertexDescriptor const& sink)
      {
        using ResEdge = ResidualEdge<EdgeDescriptor>;

        auto const num_vertices = boost::num_vertices(graph);

        // for each vertex, store the residual edge that leads to it
        std::vector<std::optional<ResEdge>> parent(num_vertices, std::nullopt);
        std::vector<bool> visited(num_vertices, false);

        std::queue<VertexDescriptor> queue;
        queue.push(source);
        visited[source] = true;

        bool found_sink = false;

        while (!queue.empty() && !found_sink) {
          auto u = queue.front();
          queue.pop();

          // Explore forward edges
          auto [out_begin, out_end] = boost::out_edges(u, graph);
          for (auto it = out_begin; it != out_end; ++it) {
            auto e = *it;
            auto v = boost::target(e, graph);

            int residual_cap = graph[e].capacity - graph[e].flow;

            if (!visited[v] && residual_cap > 0) {
              visited[v] = true;
              parent[v] = ResEdge{e, true, residual_cap};
              queue.push(v);

              if (v == sink) {
                found_sink = true;
                break;
              }
            }
          }

          if (found_sink) break;

          // Explore backward edges so that allows "undoing" flow
          auto [in_begin, in_end] = boost::in_edges(u, graph);
          for (auto it = in_begin; it != in_end; ++it) {
            auto e = *it;
            auto v = boost::source(e, graph);

            int residual_cap = graph[e].flow;

            if (!visited[v] && residual_cap > 0) {
              visited[v] = true;
              parent[v] = ResEdge{e, false, residual_cap};
              queue.push(v);

              if (v == sink) {
                found_sink = true;
                break;
              }
            }
          }
        }

        if (!found_sink) {
          return std::nullopt;
        }

        // We reconstruct path and find bottleneck
        std::vector<ResEdge> path;
        int bottleneck = std::numeric_limits<int>::max();

        VertexDescriptor current = sink;
        while (current != source) {
          auto const& edge_info = parent[current].value();
          path.push_back(edge_info);
          bottleneck = std::min(bottleneck, edge_info.residual_capacity);

          // Move to previous vertex
          if (edge_info.is_forward) {
            current = boost::source(edge_info.original_edge, graph);
          } else {
            current = boost::target(edge_info.original_edge, graph);
          }
        }

        // We reverse to get path from source to sink
        std::reverse(path.begin(), path.end());

        return std::make_pair(path, bottleneck);
      }
    }

    template <predef::concepts::graph::BidirectionalGraph Graph_T,
            predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCOp_T =
                graph::operators::DefaultEdgeCapacityOperator<>,
            predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdFOp_T =
                graph::operators::DefaultEdgeFlowOperator<>>
    int maxFlow([[maybe_unused]] Graph_T& graph,
              [[maybe_unused]] typename Graph_T::vertex_descriptor const& source,
              [[maybe_unused]] typename Graph_T::vertex_descriptor const& sink,
              [[maybe_unused]] EdCOp_T capacity_op = EdCOp_T(),
              [[maybe_unused]] EdFOp_T flow_op = EdFOp_T()) {
      using VertexDescriptor = typename Graph_T::vertex_descriptor;
      using EdgeDescriptor = typename Graph_T::edge_descriptor;

      // Init all flows to 0
      auto [edge_begin, edge_end] = boost::edges(graph);
      for (auto it = edge_begin; it != edge_end; ++it) {
        graph[*it].flow = 0;
      }

      int total_flow = 0;

      // We repeatedly find augmenting paths and push flow
      while (true) {
        auto result = detail::findAugmentingPathBFS<Graph_T, VertexDescriptor, EdgeDescriptor>(
            graph, source, sink);

        if (!result.has_value()) {
          // No more augmenting paths, we are done
          break;
        }

        auto const& [path, bottleneck] = result.value();

        // Augment flow along the path
        for (auto const& res_edge : path) {
          if (res_edge.is_forward) {
            // Forward edge = increase flow
            graph[res_edge.original_edge].flow += bottleneck;
          } else {
            // Backward edge = decrease flow (undo previous flow)
            graph[res_edge.original_edge].flow -= bottleneck;
          }
        }

        total_flow += bottleneck;
      }

      return total_flow;
    }

    template <typename Graph_T>
    void cycleCanceling([[maybe_unused]] Graph_T& graph)
    {
    // 1. Find a feasible flow f   (Ford-Fulkerson maxflow in a related graph)
    // 2. REPEAT (until no augmenting cycles)
    //   2.1 Find an augmenting cycle C (Bellman-Ford in residual graph to detect negative cost cycles)
    //   2.2 Augment flow along C
    }

}   // namespace dte3611::graph::algorithms


#endif   // DTE3611_WEEK3_NETWORK_FLOW_H
