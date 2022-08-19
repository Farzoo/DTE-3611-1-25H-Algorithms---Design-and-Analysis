#ifndef DTE3611_WEEK3_NETWORK_FLOW_H
#define DTE3611_WEEK3_NETWORK_FLOW_H

// stl
#include <vector>

namespace dte3611::np::algorithms
{

  template <typename Graph_T>
  int maxFlow([[maybe_unused]] Graph_T& graph,
              [[maybe_unused]]
              typename Graph_T::vertex_descriptor const& source,
              [[maybe_unused]] typename Graph_T::vertex_descriptor const& sink)
  {
    return {0};
  }

  template <typename Graph_T>
  void cycleCanceling([[maybe_unused]] Graph_T& graph)
  {
  }

}   // namespace dte3611::graph::algorithms


#endif   // DTE3611_WEEK3_NETWORK_FLOW_H
