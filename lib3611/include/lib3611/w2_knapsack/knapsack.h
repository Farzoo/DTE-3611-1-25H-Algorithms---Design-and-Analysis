#ifndef DTE3611_WEEK2_KNAPSACK_H
#define DTE3611_WEEK2_KNAPSACK_H

// stl
//#include <iterator>
#include <ranges>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <functional>

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
      operator()(Iterator_T begin, Sentinel_T end,
                     Output_T result, int capacity,
                     ValueProjection_T value_projection,
                     WeightProjection_T weight_projection) const
      {
        std::vector<int> values;
        std::vector<int> weights;

        /******************
         *  Ranges Operator
         */
        for (auto it = begin; it != end; ++it) {
          values.push_back(std::invoke(value_projection, *it));
          weights.push_back(std::invoke(weight_projection, *it));
        }

        int n = static_cast<int>(values.size());
        int W = capacity;

        // DP table: M[i][w] = max value using items 0..i-1 with capacity w
        std::vector<std::vector<int>> M(n + 1, std::vector<int>(W + 1, 0));

        // Fill table (Bellman equation)
        for (int i = 1; i <= n; ++i) {
          int wi = weights[i - 1];
          int vi = values[i - 1];

          for (int w = 0; w <= W; ++w) {
            if (wi > w) {
              M[i][w] = M[i - 1][w];
            } else {
              M[i][w] = std::max(M[i - 1][w], vi + M[i - 1][w - wi]);
            }
          }
        }

        // Backtrack to find selection
        int w = W;
        for (int i = n; i >= 1; --i) {
          if (M[i][w] != M[i - 1][w]) {
            *result++ = true;
            w -= weights[i - 1];
          } else {
            *result++ = false;
          }
        }

        return M[n][W];
      }

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

    struct knapsack_01_bruteforce_fn {

      template <std::input_iterator           Iterator_T,
                std::sentinel_for<Iterator_T> Sentinel_T,
                std::weakly_incrementable     Output_T,
                typename ValueProjection_T  = std::identity,
                typename WeightProjection_T = std::identity>
      int operator()(Iterator_T begin, Sentinel_T end,
                     Output_T result, int capacity,
                     ValueProjection_T value_projection,
                     WeightProjection_T weight_projection) const
      {
        std::vector<int> values;
        std::vector<int> weights;

        for (auto it = begin; it != end; ++it) {
          values.push_back(std::invoke(value_projection, *it));
          weights.push_back(std::invoke(weight_projection, *it));
        }

        int n = static_cast<int>(values.size());

        if (n == 0) return 0;
        if (n > 30) {
          throw std::runtime_error("Brute force not feasible for n > 30");
        }

        int bestValue = 0;
        uint64_t bestMask = 0;
        uint64_t totalSubsets = 1ULL << n; // 2^n

        // Enumerate all 2^n subsets
        for (uint64_t mask = 0; mask < totalSubsets; ++mask) {
          int totalWeight = 0;
          int totalValue = 0;

          for (int i = 0; i < n; ++i) {
            if (mask & (1ULL << i)) {
              totalWeight += weights[i];
              totalValue += values[i];
            }
          }

          if (totalWeight <= capacity && totalValue > bestValue) {
            bestValue = totalValue;
            bestMask = mask;
          }
        }

        // Output selection
        for (int i = n - 1; i >= 0; --i) {
          bool selected = (bestMask & (1ULL << i)) != 0;
          *result++ = selected;
        }

        return bestValue;
      }

      template <std::ranges::input_range  Range_T,
                std::weakly_incrementable Output_T,
                typename ValueProjection_T  = std::identity,
                typename WeightProjection_T = std::identity>
      auto operator()(Range_T&& range, Output_T result, int capacity,
                      ValueProjection_T  value_projection,
                      WeightProjection_T weight_projection) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::move(result), capacity, std::ref(value_projection),
                       std::ref(weight_projection));
      }
    };

  }   // namespace detail

  inline constexpr detail::knapsack_01_fn knapsack_01{};

  inline constexpr detail::knapsack_01_bruteforce_fn knapsack_01_bruteforce{};

}   // namespace dte3611::np::algorithms


#endif   // DTE3611_WEEK2_KNAPSACK_H
