#ifndef DTE3611_WEEK1_STRING_MATCH_KNUTH_MORRIS_PRATT_SEARCH_H
#define DTE3611_WEEK1_STRING_MATCH_KNUTH_MORRIS_PRATT_SEARCH_H

// stl
#include <iterator>
#include <algorithm>
#include <functional>

namespace dte3611::string_match::algorithms
{
  namespace detail
  {

    struct kmp_search_fn {

      /**************************
       *  Iterator Range Operator
       */

      // Type Generics
      template <std::forward_iterator           Iterator_T,
                std::sentinel_for<Iterator_T>   Sentinel_T,
                std::forward_iterator           S_Iterator_T,
                std::sentinel_for<S_Iterator_T> S_Sentinel_T,
                typename BinaryPredicate_T = std::ranges::equal_to,
                typename Projection_T      = std::identity,
                typename S_Projection_T    = std::identity>

      // Algorithm type requirements
      requires std::indirectly_comparable<Iterator_T, S_Iterator_T,
                                          BinaryPredicate_T, Projection_T,
                                          S_Projection_T>

      // Return value
      constexpr Iterator_T

      // Call-operator signature
      operator()(Iterator_T first, Sentinel_T last,
                 S_Iterator_T s_first, S_Sentinel_T s_last,
                 BinaryPredicate_T pred = {}, Projection_T proj = {},
                 S_Projection_T s_proj = {}) const
      {
        // Empty pattern matches at first
        if (s_first == s_last) return first;

        // Materialize projected pattern
        using PatKey = std::remove_cvref_t<std::invoke_result_t<S_Projection_T, decltype(*s_first)>>;
        std::vector<PatKey> pat;
        for (auto it = s_first; it != s_last; ++it) pat.push_back(std::invoke(s_proj, *it));
        const std::size_t m = pat.size();
        if (m == 0) return first;

        // Build LPS
        std::vector<std::size_t> lps(m, 0);
        for (std::size_t i = 1, len = 0; i < m; ) {
          if (std::invoke(pred, pat[i], pat[len])) {
            lps[i++] = ++len;
          } else if (len) {
            len = lps[len - 1];
          } else {
            lps[i++] = 0;
          }
        }

        // KMP search start tracking
        Iterator_T start = first;
        std::size_t j = 0;

        for (Iterator_T it = first; it != last; ++it) {
          auto hv = std::invoke(proj, *it);

          while (j > 0 && !std::invoke(pred, hv, pat[j])) {
            std::size_t old = j;
            j = lps[j - 1];
            for (std::size_t k = 0; k < old - j; ++k) ++start;
          }

          if (std::invoke(pred, hv, pat[j])) {
            ++j;
            if (j == m) return start;
          } else {
            // j == 0 mismatch
            ++start;
          }
        }
        return std::ranges::next(first, last); // last
      }

      /******************
       *  Ranges Operator
       */

      // Type Generics
      template <std::ranges::forward_range Range_T,
                std::ranges::forward_range S_Range_T,
                typename BinaryPredicate_T = std::ranges::equal_to,
                typename Projection_T      = std::identity,
                typename S_Projection_T    = std::identity>

      // Algorithm type requirements
      requires std::indirectly_comparable<
        std::ranges::iterator_t<Range_T>, std::ranges::iterator_t<S_Range_T>,
        BinaryPredicate_T, Projection_T, S_Projection_T>

      // Return value
      constexpr std::ranges::iterator_t<Range_T>

      // Call-operator signature
      operator()(Range_T&& range, S_Range_T&& s_range,
                 BinaryPredicate_T pred = {}, Projection_T proj = {},
                 S_Projection_T s_proj = {}) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::ranges::begin(s_range), std::ranges::end(s_range),
                       std::move(pred), std::move(proj), std::move(s_proj));
//        static_assert(false, "Complete the code"
//                             "- find the appropriate call signature in the "
//                             "cpp reference documentation.");
      }

    };

  }   // namespace detail

  // Niebloid API Instantiation
  inline constexpr detail::kmp_search_fn kmp_search{};

}   // namespace dte3611::string_match::algorithms

#endif   // DTE3611_WEEK1_STRING_MATCH_KNUTH_MORRIS_PRATT_SEARCH_H
