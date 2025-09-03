#ifndef DTE3611_DAY4_BREADTH_FIRST_SEARCH_H
#define DTE3611_DAY4_BREADTH_FIRST_SEARCH_H

// concepts
#include "../utils/concepts/graphs.h"

// boost
#include <boost/graph/adjacency_list.hpp>
#include <queue>

namespace dte3611::graph::algorithms
{
  template <predef::concepts::graph::BidirectionalGraph Graph_T>
  std::vector<typename Graph_T::vertex_descriptor>
  breadthFirstSearch([[maybe_unused]] Graph_T const& graph,
                     [[maybe_unused]]
                     typename Graph_T::vertex_descriptor const& start)
  {

    using VertexDescriptor = typename Graph_T::vertex_descriptor;

    std::queue<VertexDescriptor> q;

    std::set<VertexDescriptor> visited;

    std::vector<VertexDescriptor> result_traversal;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
      VertexDescriptor u = q.front();
      q.pop();

      result_traversal.push_back(u);

      for (auto v : boost::make_iterator_range(boost::adjacent_vertices(u, graph))) {

        if (!visited.contains(v)) {
          visited.insert(v);

          q.push(v);
        }
      }
    }

    if(!result_traversal.empty())
      result_traversal.erase(result_traversal.begin());

    return result_traversal;
  }

}   // namespace dte3611::graph::algorithms


#endif   // DTE3611_DAY4_BREADTH_FIRST_SEARCH_H
