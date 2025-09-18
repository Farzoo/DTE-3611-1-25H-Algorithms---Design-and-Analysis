#ifndef DTE3611_WEEK2_SUBSET_SUM_H
#define DTE3611_WEEK2_SUBSET_SUM_H


// concepts
// ...

// stl
#include <vector>
#include <ranges>

#include <numeric>
#include <functional>

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
      operator()(Iterator_T begin, Sentinel_T end, Value_T target,
                 Projection_T projection = {}) const
      {
        std::vector<Value_T> values;
        for (auto it = begin; it != end; ++it) {
          values.push_back(std::invoke(projection, *it));
        }

        SubsetSum_Output<Value_T> result;
        std::vector<Value_T> current;

        std::function<void(size_t, Value_T)> backtrack =
          [&](size_t idx, Value_T currentSum) {
            if (currentSum == target) {
              result.push_back(current);
            }
            if (currentSum >= target || idx >= values.size()) {
              return;
            }
            for (size_t i = idx; i < values.size(); ++i) {
              current.push_back(values[i]);
              backtrack(i + 1, currentSum + values[i]);
              current.pop_back();
            }
          };

        backtrack(0, Value_T{0});
        return result;
      }

      template <std::ranges::forward_range Range_T,
                typename Projection_T = std::identity, typename Value_T>
      SubsetSum_Output<Value_T>
      operator()(Range_T&& range, Value_T target, Projection_T projection = {}) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       target, std::ref(projection));
      }
    };

    struct subset_sum_dp_fn {

      /******************
       *  Ranges Operator
       */
      template <typename Value_T>
      using SubsetSum_Output = std::vector<std::vector<Value_T>>;

      // Type Generics
      template <std::forward_iterator         Iterator_T,
                std::sentinel_for<Iterator_T> Sentinel_T,
                typename Projection_T = std::identity, typename Value_T>

      // Return value
      SubsetSum_Output<Value_T>

      // Call-operator signature
      operator()(Iterator_T begin, Sentinel_T end, Value_T target,
                 Projection_T projection = {}) const
      {
        if (target < 0) return {};

        std::vector<Value_T> values;
        for (auto it = begin; it != end; ++it) {
          values.push_back(std::invoke(projection, *it));
        }

        int n = static_cast<int>(values.size());
        int W = static_cast<int>(target);

        if (n == 0) {
          if (target == 0) return {{}};
          return {};
        }

        // DP table
        std::vector<std::vector<bool>> dp(n + 1, std::vector<bool>(W + 1, false));

        // Base case: sum 0 is always achievable
        for (int i = 0; i <= n; ++i) {
          dp[i][0] = true;
        }

        // Fill table: dp[i][s] = dp[i-1][s] OR dp[i-1][s - x_i]
        for (int i = 1; i <= n; ++i) {
          int xi = static_cast<int>(values[i - 1]);
          for (int s = 0; s <= W; ++s) {
            dp[i][s] = dp[i - 1][s];
            if (xi <= s && dp[i - 1][s - xi]) {
              dp[i][s] = true;
            }
          }
        }

        if (!dp[n][W]) return {};

        // Backtrack to find ALL solutions
        SubsetSum_Output<Value_T> result;
        std::vector<Value_T> current;

        std::function<void(int, int)> reconstruct = [&](int i, int s) {
          if (s == 0) {
            std::vector<Value_T> solution(current.rbegin(), current.rend());
            result.push_back(solution);
            return;
          }
          if (i == 0 || s < 0) return;

          int xi = static_cast<int>(values[i - 1]);

          // Don't take element i
          if (dp[i - 1][s]) {
            reconstruct(i - 1, s);
          }

          // Take element i
          if (xi <= s && dp[i - 1][s - xi]) {
            current.push_back(values[i - 1]);
            reconstruct(i - 1, s - xi);
            current.pop_back();
          }
        };

        reconstruct(n, W);
        return result;
      }

      template <std::ranges::forward_range Range_T,
                typename Projection_T = std::identity, typename Value_T>
      SubsetSum_Output<Value_T>
      operator()(Range_T&& range, Value_T target, Projection_T projection = {}) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       target, std::ref(projection));
        //      static_assert( false, "Complete the code - find the appropriate call signature in the cpp reference documentation." );
      }
    };

  }   // namespace detail

  inline constexpr detail::subset_sum_fn subset_sum{};

  inline constexpr detail::subset_sum_dp_fn subset_sum_dp{};
}   // namespace dte3611::np::algorithms


#endif   // DTE3611_WEEK2_SUBSET_SUM_H
