#ifndef DTE3611_DAY5_DIJKSTRA_SHORTEST_PATHS_H
#define DTE3611_DAY5_DIJKSTRA_SHORTEST_PATHS_H

#include "operators.h"

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <queue>
#include <vector>

namespace dte3611::graph::algorithms
{

    template <predef::concepts::graph::BidirectionalGraph            Graph_T,
              predef::concepts::graph::EdgeDistanceOperator<Graph_T> EdOp_T
              = operators::DefaultEdgeDistanceOperator<>>
    std::vector<std::vector<typename Graph_T::vertex_descriptor>>
    dijkstraShortestPaths([[maybe_unused]] Graph_T const& graph,
                          [[maybe_unused]]
                          typename Graph_T::vertex_descriptor const& start,
                          [[maybe_unused]]
                          typename Graph_T::vertex_descriptor const& goal,
                          [[maybe_unused]] EdOp_T distance_op = EdOp_T())
    {
        using VertexDescriptor = typename Graph_T::vertex_descriptor;
        using DistanceType = double;
        using VertexDistance = std::pair<DistanceType, VertexDescriptor>;

        std::priority_queue<
            VertexDistance,
            std::vector<VertexDistance>,
            std::greater<VertexDistance> // Smallest distance first
        > pq; // Min heap

        std::map<VertexDescriptor, DistanceType> dist;
        std::map<VertexDescriptor, std::vector<VertexDescriptor>> parents;
        std::set<VertexDescriptor> visited;

        dist[start] = 0.0;
        pq.push({0.0, start});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (visited.contains(u)) {
                continue;
            }

            visited.insert(u);

            for (auto e : boost::make_iterator_range(boost::out_edges(u, graph))) {
                auto v = boost::target(e, graph);
                auto weight = distance_op(e, graph);
                auto new_dist = dist[u] + weight;

                bool v_not_seen = (!dist.contains(v)); // Treat unseen as +inf

                if (v_not_seen || new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parents[v] = {u}; // Reset parents
                    pq.push({new_dist, v});
                }
                else if (new_dist == dist[v]) {
                    parents[v].push_back(u); // Add alternative parent
                }
            }
        }

        // Path reconstruction via backtracking
        std::vector<std::vector<VertexDescriptor>> result;

        std::function<void(VertexDescriptor, std::vector<VertexDescriptor>&)>
        buildPaths = [&](VertexDescriptor v, std::vector<VertexDescriptor>& path) {
            if (v == start) {
                result.push_back(path); // Path complete (excludes start)
                return;
            }

            if (!parents.contains(v)) {
                return; // Unreachable
            }

            for (auto& p : parents[v]) {
                path.insert(path.begin(), v);
                buildPaths(p, path);
                path.erase(path.begin()); // Backtrack
            }
        };

        std::vector<VertexDescriptor> path;
        buildPaths(goal, path);

        return result;
    }


}   // namespace dte3611::graph::algorithms

#endif   // DTE3611_DAY5_DIJKSTRA_SHORTEST_PATHS_H
