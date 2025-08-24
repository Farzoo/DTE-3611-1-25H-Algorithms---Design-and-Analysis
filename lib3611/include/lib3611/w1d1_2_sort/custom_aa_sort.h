#ifndef DTE3611_WEEK1_CUSTOM_AA_SORT_H
#define DTE3611_WEEK1_CUSTOM_AA_SORT_H

// stl
#include <iterator>
#include <algorithm>

namespace dte3611::sort::algorithms
{

  namespace detail
  {

    struct custom_aa_sort_fn {

      /**************************
       *  Iterator Range Operator
       */

      // Type Generics
      template <std::random_access_iterator   Iterator_T,
                std::sentinel_for<Iterator_T> Sentinel_T,
                typename Compare_T    = std::ranges::less,
                typename Projection_T = std::identity>
      // Algorithm type requirements
      requires std::sortable<Iterator_T, Compare_T, Projection_T>
      // Return value
      constexpr Iterator_T
      // Call-operator signature
      operator()(Iterator_T first, Sentinel_T last,
                 Compare_T comp = {}, Projection_T proj = {}) const
      {
        Iterator_T last_it = std::ranges::next(first, last);
        if (first == last_it) return last_it;

        auto less = [&](const auto& a, const auto& b) {
          return std::invoke(comp, std::invoke(proj, a), std::invoke(proj, b));
        };

        // Insertion sort for small ranges (stable).
        auto insertion_sort = [&](Iterator_T f, Iterator_T l) {
          for (Iterator_T it = f + (f == l ? 0 : 1); it != l; ++it) {
            auto val = std::move(*it);
            Iterator_T j = it;
            while (j != f && less(val, *(j - 1))) {
              *j = std::move(*(j - 1));
              --j;
            }
            *j = std::move(val);
          }
        };

        constexpr std::size_t SMALL = 24;

        // Non-recursive quicksort with Hoare style partition median of three pivot.
        std::vector<std::pair<Iterator_T, Iterator_T>> stack;
        stack.emplace_back(first, last_it);

        while (!stack.empty()) {
          auto [lo, hi] = stack.back();
          stack.pop_back();

          while (true) {
            auto n = hi - lo;
            if (n <= 1) break;
            if (n <= static_cast<std::ptrdiff_t>(SMALL)) {
              insertion_sort(lo, hi);
              break;
            }

            Iterator_T mid = lo + n / 2;

            // Median of three : (lo, mid, hi-1)
            Iterator_T a = lo, b = mid, c = hi - 1;
            if (less(*b, *a)) std::iter_swap(a, b);
            if (less(*c, *b)) std::iter_swap(b, c);
            if (less(*b, *a)) std::iter_swap(a, b);
            auto pivot_key = std::invoke(proj, *b);

            // Hoare partition
            Iterator_T i = lo;
            Iterator_T j = hi - 1;
            while (true) {
              while (std::invoke(comp, std::invoke(proj, *i), pivot_key)) ++i;
              while (std::invoke(comp, pivot_key, std::invoke(proj, *j))) --j;
              if (i >= j) break;
              std::iter_swap(i, j);
              ++i; --j;
            }

            // Tail call elimination sort smaller part first, loop on larger.
            Iterator_T left_lo = lo, left_hi = j + 1;
            Iterator_T right_lo = j + 1, right_hi = hi;
            auto left_n  = left_hi - left_lo;
            auto right_n = right_hi - right_lo;

            if (left_n < right_n) {
              if (right_n > 1) stack.emplace_back(right_lo, right_hi);
              lo = left_lo; hi = left_hi; // continue on smaller
            } else {
              if (left_n > 1) stack.emplace_back(left_lo, left_hi);
              lo = right_lo; hi = right_hi; // continue on smaller
            }
          }
        }

        return last_it;
      }

      /******************
       *  Ranges Operator
       */

      // Type Generics
      template <std::ranges::random_access_range Range_T,
                typename Compare_T    = std::ranges::less,
                typename Projection_T = std::identity>
      // Algorithm type requirements
      requires std::sortable<std::ranges::iterator_t<Range_T>, Compare_T,
                             Projection_T>
      // Return value
      constexpr std::ranges::borrowed_iterator_t<Range_T>
      // Call-operator signature
      operator()(Range_T&& range, Compare_T comp = {},
                 Projection_T proj = {}) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::move(comp), std::move(proj));
//        static_assert(false, "Complete the code"
//                             "- find the appropriate call signature in the "
//                             "cpp reference documentation.");
      }
    };

  }   // namespace detail

  // Niebloid API Instantiation
  inline constexpr detail::custom_aa_sort_fn custom_aa_sort{};

}   // namespace dte3611::sort::algorithms

#endif   // DTE3611_WEEK1_CUSTOM_AA_SORT_H
