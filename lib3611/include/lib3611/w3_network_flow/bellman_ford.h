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
    //    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    //    using EdgeDescriptor   = typename Graph_T::edge_descriptor;
    //    using DistanceType     = double;

    return {};

  }

} // namespace

#endif
