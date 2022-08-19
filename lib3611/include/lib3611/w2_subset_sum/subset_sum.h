#ifndef DTE3611_WEEK2_SUBSET_SUM_H
#define DTE3611_WEEK2_SUBSET_SUM_H


// concepts
// ...

// stl
#include <vector>
#include <ranges>

#include <numeric>

namespace dte3611::np::algorithms
{

  namespace detail
  {

    struct subset_sum_fn {

      /************************
       *  Return type -- helper
       */
      template <typename Value_T>
      using SubsetSum_Output = std::vector<std::vector<Value_T>>;



      /**************************
       *  Iterator Range Operator
       */
      // Type Generics
      template <std::forward_iterator         Iterator_T,
                std::sentinel_for<Iterator_T> Sentinel_T,
                typename Projection_T = std::identity, typename Value_T>

      // Return value
      SubsetSum_Output<Value_T>

      // Call-operator signature
      operator()(Iterator_T /*begin*/, Sentinel_T /*end*/, Value_T /*target*/,
                 Projection_T /* projection */ = {}) const
      {

        /* function content */
        return {};
      }


      /******************
       *  Ranges Operator
       */

      // Type Generics
      template <std::ranges::forward_range Range_T,
                typename Projection_T = std::identity, typename Value_T>

      // Return value
      SubsetSum_Output<Value_T>

      // Call-operator signature
      operator()(Range_T&& range, Value_T target,
                 Projection_T projection = {}) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       target, std::ref(projection));
        //      static_assert( false, "Complete the code - find the appropriate call signature in the cpp reference documentation." );
      }
    };

  }   // namespace detail

  inline constexpr detail::subset_sum_fn subset_sum{};

}   // namespace dte3611::np::algorithms


#endif   // DTE3611_WEEK2_SUBSET_SUM_H
