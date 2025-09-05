#ifndef DTE3611_WEEK1_COUNTING_SORT_H
#define DTE3611_WEEK1_COUNTING_SORT_H

// stl
#include <iterator>
#include <algorithm>
#include <functional>

namespace dte3611::sort::algorithms
{
  namespace detail
  {

    struct counting_sort_fn {

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
        (void)comp; // comparator not used

        Iterator_T last_it = std::ranges::next(first, last);
        if (first == last_it) return last_it;

        using Elem = std::iter_value_t<Iterator_T>;
        using Key  = std::remove_cvref_t<std::invoke_result_t<Projection_T, decltype(*first)>>;

        static_assert(std::is_integral_v<Key>,
                      "counting_sort requires an integral key after projection");

        // Find key range [min_k, max_k]
        Key min_k = std::invoke(proj, *first);
        Key max_k = min_k;
        for (Iterator_T it = first + 1; it != last_it; ++it) {
          Key k = std::invoke(proj, *it);
          if (k < min_k) min_k = k;
          if (k > max_k) max_k = k;
        }

        // Allocate counts
        const auto span = static_cast<std::make_unsigned_t<Key>>(max_k - min_k) + 1u;
        std::vector<std::size_t> counts(static_cast<std::size_t>(span), 0);

        // Histogram
        for (Iterator_T it = first; it != last_it; ++it) {
          Key k = std::invoke(proj, *it);
          counts[static_cast<std::size_t>(static_cast<std::make_unsigned_t<Key>>(k - min_k))] += 1;
        }

        // Prefix sums -> end positions
        for (std::size_t i = 1; i < counts.size(); ++i) counts[i] += counts[i - 1];

        // Stable placement to buffer
        std::vector<Elem> buffer(static_cast<std::size_t>(last_it - first));
        for (Iterator_T it = last_it; it != first; ) {
          --it;
          Key k = std::invoke(proj, *it);
          const auto idx = static_cast<std::size_t>(static_cast<std::make_unsigned_t<Key>>(k - min_k));
          std::size_t pos = --counts[idx];
          buffer[pos] = *it;
        }

        std::move(buffer.begin(), buffer.end(), first);
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
  inline constexpr detail::counting_sort_fn counting_sort{};


}   // namespace dte3611::sort::algorithms

#endif   // DTE3611_WEEK1_COUNTING_SORT_H
