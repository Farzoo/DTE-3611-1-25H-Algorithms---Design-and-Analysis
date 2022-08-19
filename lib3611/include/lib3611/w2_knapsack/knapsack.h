#ifndef DTE3611_WEEK2_KNAPSACK_H
#define DTE3611_WEEK2_KNAPSACK_H

// stl
//#include <iterator>
#include <ranges>
#include <algorithm>

namespace dte3611::np::algorithms
{

  namespace detail
  {

    struct knapsack_01_fn {

      /**************************
       *  Iterator Range Operator
       */

      // Type Generics
      template <std::input_iterator           Iterator_T,
                std::sentinel_for<Iterator_T> Sentinel_T,
                std::weakly_incrementable     Output_T,
                typename ValueProjection_T  = std::identity,
                typename WeightProjection_T = std::identity>
      // Return value
      int
      // Call-operator signature
      operator()(Iterator_T /* begin */, Sentinel_T /* end */,
                 Output_T /* result */, int /* capacity */,
                 ValueProjection_T /* value_projection */,
                 WeightProjection_T /* weight_projection */) const
      {
        return {};
      }

      /******************
       *  Ranges Operator
       */

      // Type Generics
      template <std::ranges::input_range  Range_T,
                std::weakly_incrementable Output_T,
                typename ValueProjection_T  = std::identity,
                typename WeightProjection_T = std::identity>
      // Return value
      auto
      // Call-operator signature
      operator()(Range_T&& range, Output_T result, int capacity,
                 ValueProjection_T  value_projection,
                 WeightProjection_T weight_projection) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::move(result), capacity, std::ref(value_projection),
                       std::ref(weight_projection));
        //      static_assert( false, "Complete the code - find the appropriate call signature in the cpp reference documentation." );
      }

    };

  }   // namespace detail

  inline constexpr detail::knapsack_01_fn knapsack_01{};


}   // namespace dte3611::np::algorithms


#endif   // DTE3611_WEEK2_KNAPSACK_H
