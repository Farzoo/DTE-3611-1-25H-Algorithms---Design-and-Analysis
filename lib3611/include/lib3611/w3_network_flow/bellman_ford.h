#ifndef DTE3611_WEEK2_BELLMAN_FORD_H
#define DTE3611_WEEK2_BELLMAN_FORD_H

#include "../w1d5_graph_path_finding/operators.h"

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <vector>

// utils
#include "../utils/graph_utils.h"

// Boost
#include <boost/property_map/function_property_map.hpp>

namespace dte3611::graph::algorithms
{

  template <predef::concepts::graph::BidirectionalGraph            Graph_T,
            predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdOp_T
            = operators::DefaultEdgeDistanceOperator<>>
  std::vector<std::vector<typename Graph_T::vertex_descriptor>>
  bellmanFordShortestPaths([[maybe_unused]] Graph_T const& graph,
                           [[maybe_unused]]
                           typename Graph_T::vertex_descriptor const& start,
                           [[maybe_unused]]
                           typename Graph_T::vertex_descriptor const& goal,
                           [[maybe_unused]] EdOp_T distance_op = EdOp_T())
  {
    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    using DistanceType = double;

    constexpr DistanceType INF = std::numeric_limits<DistanceType>::infinity();

    auto const num_vertices = boost::num_vertices(graph);

    // Distance from start to each vertex
    std::vector<DistanceType> dist(num_vertices, INF);

    // Parent tracking for path reconstruction
    std::map<VertexDescriptor, std::vector<VertexDescriptor>> parents;

    dist[start] = 0.0;

    // Relax all edges
    for (std::size_t i = 0; i < num_vertices - 1; ++i) {
      bool any_relaxation = false;

      auto [edge_begin, edge_end] = boost::edges(graph);
      for (auto it = edge_begin; it != edge_end; ++it) {
        auto e = *it;
        auto u = boost::source(e, graph);
        auto v = boost::target(e, graph);
        auto weight = distance_op(e, graph);

        // we skip if source is unreachable
        if (dist[u] == INF) {
          continue;
        }

        DistanceType new_dist = dist[u] + weight;

        constexpr double EPSILON = 1e-9;

        if (new_dist < dist[v] - EPSILON) {
          // We found a shorter path
          dist[v] = new_dist;
          parents[v] = {u}; // Reset parents
          any_relaxation = true;
        }
        else if (std::abs(new_dist - dist[v]) < EPSILON) {
          // We found an alternative path of equal length
          // Check if u is already a parent to avoid duplicates
          auto& parent_list = parents[v];
          if (std::find(parent_list.begin(), parent_list.end(), u) == parent_list.end()) {
            parent_list.push_back(u);
          }
        }
      }

      if (!any_relaxation) {
        break;
      }
    }

    // Path reconstruction via backtracking
    std::vector<std::vector<VertexDescriptor>> result;

    // If the goal is unreachable, return empty
    if (dist[goal] == INF) {
      return result;
    }

    std::function<void(VertexDescriptor, std::vector<VertexDescriptor>&)>
    buildPaths = [&](VertexDescriptor v, std::vector<VertexDescriptor>& path) {
      if (v == start) {
        result.push_back(path);
        return;
      }

      if (!parents.contains(v)) {
        return; // Should never happen since we checked reachability earlier
      }

      for (auto const& p : parents[v]) {
        path.insert(path.begin(), v);
        buildPaths(p, path);
        path.erase(path.begin());  // Backtrack
      }
    };

    std::vector<VertexDescriptor> path;
    buildPaths(goal, path);

    return result;

  }

} // namespace

#endif
