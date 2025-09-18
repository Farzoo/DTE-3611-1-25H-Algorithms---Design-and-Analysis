// Parameterized tests for subset_sum algorithms
// Tests both backtracking and DP versions with the same test cases

#include <lib3611/w2_subset_sum/subset_sum.h>

#include <gtest/gtest.h>

#include <vector>
#include <ranges>
#include <algorithm>
#include <string>
#include <functional>

namespace alg = dte3611::np::algorithms;

// Wrapper
template <typename Value_T>
using SubsetSumResult = std::vector<std::vector<Value_T>>;

struct BacktrackingAlgo {
  static constexpr const char* name = "Backtracking";

  template <typename Range_T, typename Value_T, typename Proj_T = std::identity>
  static SubsetSumResult<Value_T> solve(Range_T&& range, Value_T target, Proj_T proj = {}) {
    return alg::subset_sum(std::forward<Range_T>(range), target, proj);
  }
};

struct DPAlgo {
  static constexpr const char* name = "DP";

  template <typename Range_T, typename Value_T, typename Proj_T = std::identity>
  static SubsetSumResult<Value_T> solve(Range_T&& range, Value_T target, Proj_T proj = {}) {
    return alg::subset_sum_dp(std::forward<Range_T>(range), target, proj);
  }
};

// Parameterized Test Fixture
template <typename AlgoType>
class SubsetSumTest : public ::testing::Test {
protected:
  using Algo = AlgoType;
};

using AlgoTypes = ::testing::Types<BacktrackingAlgo, DPAlgo>;
TYPED_TEST_SUITE(SubsetSumTest, AlgoTypes);

TYPED_TEST(SubsetSumTest, BasicCase)
{
  std::vector<int> numbers = {1, 2, 3, 4};
  auto result = TypeParam::solve(numbers, 7, std::identity{});
  std::ranges::sort(result);

  std::vector<std::vector<int>> expected = {{1, 2, 4}, {3, 4}};
  EXPECT_EQ(result, expected);
}

TYPED_TEST(SubsetSumTest, NoSolution)
{
  std::vector<int> numbers = {1, 2, 3, 4};
  auto result = TypeParam::solve(numbers, 11, std::identity{});

  EXPECT_TRUE(result.empty());
}

TYPED_TEST(SubsetSumTest, EmptySet)
{
  std::vector<int> numbers = {};
  auto result = TypeParam::solve(numbers, 5, std::identity{});

  EXPECT_TRUE(result.empty());
}

TYPED_TEST(SubsetSumTest, TargetZero)
{
  std::vector<int> numbers = {1, 2, 3};
  auto result = TypeParam::solve(numbers, 0, std::identity{});

  // Empty subset sums to 0
  ASSERT_EQ(result.size(), 1u);
  EXPECT_TRUE(result[0].empty());
}

TYPED_TEST(SubsetSumTest, SingleElementMatch)
{
  std::vector<int> numbers = {5};
  auto result = TypeParam::solve(numbers, 5, std::identity{});

  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], std::vector<int>{5});
}

TYPED_TEST(SubsetSumTest, SingleElementNoMatch)
{
  std::vector<int> numbers = {5};
  auto result = TypeParam::solve(numbers, 3, std::identity{});

  EXPECT_TRUE(result.empty());
}

TYPED_TEST(SubsetSumTest, TargetLargerThanSum)
{
  std::vector<int> numbers = {1, 2, 3}; // sum = 6
  auto result = TypeParam::solve(numbers, 100, std::identity{});

  EXPECT_TRUE(result.empty());
}

TYPED_TEST(SubsetSumTest, TargetEqualsFullSum)
{
  std::vector<int> numbers = {1, 2, 3, 4}; // sum = 10
  auto result = TypeParam::solve(numbers, 10, std::identity{});

  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], (std::vector<int>{1, 2, 3, 4}));
}

TYPED_TEST(SubsetSumTest, MultipleSolutions)
{
  std::vector<int> numbers = {1, 2, 3, 4, 5};
  auto result = TypeParam::solve(numbers, 6, std::identity{});
  std::ranges::sort(result);

  // {1,2,3}, {1,5}, {2,4}
  ASSERT_EQ(result.size(), 3u);
  std::vector<std::vector<int>> expected = {{1, 2, 3}, {1, 5}, {2, 4}};
  EXPECT_EQ(result, expected);
}

TYPED_TEST(SubsetSumTest, DuplicateValues)
{
  std::vector<int> numbers = {2, 2, 3};
  auto result = TypeParam::solve(numbers, 4, std::identity{});
  std::ranges::sort(result);

  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], (std::vector<int>{2, 2}));
}

// Projection Test
struct ValueWrapper {
  int value;
  std::string label;
};

TYPED_TEST(SubsetSumTest, WithProjection)
{
  std::vector<ValueWrapper> items = {
    {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}
  };

  auto result = TypeParam::solve(items, 7, &ValueWrapper::value);
  std::ranges::sort(result);

  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0], (std::vector<int>{1, 2, 4}));
  EXPECT_EQ(result[1], (std::vector<int>{3, 4}));
}

// Consistency Test - Both algorithms should give same results
TEST(SubsetSumConsistency, BothAlgorithmsGiveSameResults)
{
  std::vector<int> numbers = {3, 7, 1, 8, 4, 12, 5};
  int target = 15;

  auto result_bt = alg::subset_sum(numbers, target, std::identity{});
  auto result_dp = alg::subset_sum_dp(numbers, target, std::identity{});

  std::ranges::sort(result_bt);
  std::ranges::sort(result_dp);

  EXPECT_EQ(result_bt.size(), result_dp.size());
  EXPECT_EQ(result_bt, result_dp);
}