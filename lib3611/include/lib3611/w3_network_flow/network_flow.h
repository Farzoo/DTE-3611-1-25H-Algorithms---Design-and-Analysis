#ifndef DTE3611_WEEK3_NETWORK_FLOW_H
#define DTE3611_WEEK3_NETWORK_FLOW_H

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <vector>

// boost
#include <boost/graph/edge_list.hpp>

#include "operators.h"

namespace dte3611::np::algorithms
{

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
    // Implement Ford-Fulkerson (method) for computing the maximum flow in a flow
    // network by augmenting path.
    // Note: Edmonds-Karp is a fully defined implementation of the Ford-Fulkerson
    // method.
    return 0;
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
