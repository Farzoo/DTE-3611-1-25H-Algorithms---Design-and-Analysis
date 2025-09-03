#ifndef DTE3611_DAY4_DEPTH_FIRST_SEARCH_H
#define DTE3611_DAY4_DEPTH_FIRST_SEARCH_H

// concepts
#include "../utils/concepts/graphs.h"

#include <boost/graph/adjacency_list.hpp>

#include <stack>

namespace dte3611::graph::algorithms
{

  template <predef::concepts::graph::BidirectionalGraph Graph_T>
  std::vector<typename Graph_T::vertex_descriptor>
  depthFirstSearch([[maybe_unused]] Graph_T const& graph,
                   [[maybe_unused]]
                   typename Graph_T::vertex_descriptor const& start)
  {
      using VertexDescriptor = typename Graph_T::vertex_descriptor;

      std::vector<VertexDescriptor> result_traversal;
      std::set<VertexDescriptor> visited;
      std::stack<VertexDescriptor> s;

      s.push(start);

      while (!s.empty()) {
          VertexDescriptor u = s.top();
          s.pop();

          if (!visited.contains(u)) {
              visited.insert(u);
              result_traversal.push_back(u);

              std::vector<VertexDescriptor> neighbors;
              for (auto v : boost::make_iterator_range(boost::adjacent_vertices(u, graph))) {
                  if (!visited.contains(v)) {
                      neighbors.push_back(v);
                  }
              }
              for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                  s.push(*it);
              }
          }
      }

      if (!result_traversal.empty())
          result_traversal.erase(result_traversal.begin());

      return result_traversal;
  }

}   // namespace dte3611::graph::algorithms


#endif   // DTE3611_DAY4_DEPTH_FIRST_SEARCH_H
