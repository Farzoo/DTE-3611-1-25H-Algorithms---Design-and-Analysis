#ifndef DTE3611_WEEK1_RADIX_SORT_H
#define DTE3611_WEEK1_RADIX_SORT_H

// stl
#include <iterator>
#include <ranges>
#include <algorithm>


namespace dte3611::sort::algorithms
{
  namespace detail
  {

    struct radix_sort_fn {

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

        using Elem = std::iter_value_t<Iterator_T>;
        using ProjRes = std::invoke_result_t<Projection_T, decltype(*first)>;
        using Key = std::remove_cvref_t<ProjRes>;

        // Fallback: non-integral key -> stable_sort with comp+proj
        if constexpr (!std::is_integral_v<Key>) {
          std::ranges::stable_sort(std::ranges::subrange(first, last_it), comp, proj);
          return last_it;
        } else {
          using UKey = std::make_unsigned_t<Key>;
          constexpr std::size_t BYTES = sizeof(UKey);
          constexpr UKey SIGN_MASK = std::is_signed_v<Key> ? (UKey(1) << (BYTES * 8 - 1)) : UKey(0);

          Key min_k = std::invoke(proj, *first);
          Key max_k = min_k;
          for (Iterator_T it = first + 1; it != last_it; ++it) {
            Key k = std::invoke(proj, *it);
            if (k < min_k) min_k = k;
            if (k > max_k) max_k = k;
          }
          const bool descending = std::invoke(comp, max_k, min_k);

          const std::size_t n = static_cast<std::size_t>(last_it - first);
          std::vector<Elem> buffer(n);

          auto get_ukey = [&](const auto& e) -> UKey {
            // Bias signed domain so ascending unsigned order == ascending signed order
            return static_cast<UKey>(std::invoke(proj, e)) ^ SIGN_MASK;
          };

          bool read_src = true; // read from [first] first, write to buffer
          for (std::size_t pass = 0; pass < BYTES; ++pass) {
            std::array<std::size_t, 256> count{}; // histogram

            if (read_src) {
              for (std::size_t i = 0; i < n; ++i) {
                UKey u = get_ukey(*(first + static_cast<std::ptrdiff_t>(i)));
                unsigned byte = static_cast<unsigned>((u >> (8 * pass)) & 0xFFu);
                ++count[byte];
              }
            } else {
              for (std::size_t i = 0; i < n; ++i) {
                UKey u = get_ukey(buffer[i]);
                unsigned byte = static_cast<unsigned>((u >> (8 * pass)) & 0xFFu);
                ++count[byte];
              }
            }

            // Prefix sums -> end positions (stable LSD, fill from right using --count)
            for (std::size_t i = 1; i < 256; ++i) count[i] += count[i - 1];

            if (read_src) {
              for (std::size_t i = n; i-- > 0; ) {
                const Elem& e = *(first + static_cast<std::ptrdiff_t>(i));
                UKey u = get_ukey(e);
                unsigned byte = static_cast<unsigned>((u >> (8 * pass)) & 0xFFu);
                std::size_t pos = --count[byte];
                buffer[pos] = e;
              }
            } else {
              for (std::size_t i = n; i-- > 0; ) {
                const Elem& e = buffer[i];
                UKey u = get_ukey(e);
                unsigned byte = static_cast<unsigned>((u >> (8 * pass)) & 0xFFu);
                std::size_t pos = --count[byte];
                *(first + static_cast<std::ptrdiff_t>(pos)) = e;
              }
            }

            read_src = !read_src;
          }

          // If last write ended in buffer (odd number of passes), move back
          if (!read_src) {
            std::move(buffer.begin(), buffer.end(), first);
          }

          // Descending comparator reverse the ascending result (works for integral keys)
          if (descending) {
            std::reverse(first, last_it);
          }

          return last_it;
        }
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
      }
    };

  }   // namespace detail

  // Niebloid API Instantiation
  inline constexpr detail::radix_sort_fn radix_sort{};

}   // namespace dte3611::sort::algorithms

#endif   // DTE3611_WEEK1_RADIX_SORT_H
