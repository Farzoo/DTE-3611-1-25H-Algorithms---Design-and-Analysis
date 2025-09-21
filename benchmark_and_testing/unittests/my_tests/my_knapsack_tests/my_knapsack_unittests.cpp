// Parameterized tests for knapsack algorithms
// Tests both DP and brute force versions with the same test cases

#include <lib3611/w2_knapsack/knapsack.h>

#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>
#include <functional>

namespace alg = dte3611::np::algorithms;

struct Item {
  std::string name;
  int value;
  int weight;
};

// Wrapper
struct DPAlgo {
  static constexpr const char* name = "DP";

  template <typename Range_T, typename Output_T, typename VP, typename WP>
  static int solve(Range_T&& range, Output_T result, int capacity, VP vp, WP wp) {
    return alg::knapsack_01(std::forward<Range_T>(range), result, capacity, vp, wp);
  }
};

struct BruteForceAlgo {
  static constexpr const char* name = "BruteForce";

  template <typename Range_T, typename Output_T, typename VP, typename WP>
  static int solve(Range_T&& range, Output_T result, int capacity, VP vp, WP wp) {
    return alg::knapsack_01_bruteforce(std::forward<Range_T>(range), result, capacity, vp, wp);
  }
};


// Parameterized Test Fixture
template <typename AlgoType>
class KnapsackTest : public ::testing::Test {
protected:
  using Algo = AlgoType;

  // Helper to run algorithm and get normalized result
  std::pair<int, std::vector<bool>> runKnapsack(
    const std::vector<Item>& items, int capacity)
  {
    std::vector<bool> selection;
    int value = Algo::solve(items, std::back_inserter(selection), capacity,
                            &Item::value, &Item::weight);
    std::reverse(selection.begin(), selection.end());
    return {value, selection};
  }
};

using AlgoTypes = ::testing::Types<DPAlgo, BruteForceAlgo>;
TYPED_TEST_SUITE(KnapsackTest, AlgoTypes);

TYPED_TEST(KnapsackTest, ClassicExample)
{
  // From course fixtures
  std::vector<Item> items = {
    {"A", 24, 12}, {"B", 13, 7}, {"C", 23, 11}, {"D", 15, 8}, {"E", 16, 9}
  };

  auto [value, selection] = this->runKnapsack(items, 26);

  EXPECT_EQ(value, 51);
  EXPECT_EQ(selection, (std::vector<bool>{false, true, true, true, false}));
}

TYPED_TEST(KnapsackTest, SecondExample)
{
  std::vector<Item> items = {
    {"R", 1, 1}, {"S", 6, 2}, {"T", 18, 5}, {"U", 22, 6}, {"V", 28, 7}
  };

  auto [value, selection] = this->runKnapsack(items, 11);

  EXPECT_EQ(value, 40);
  EXPECT_EQ(selection, (std::vector<bool>{false, false, true, true, false}));
}

TYPED_TEST(KnapsackTest, EmptyItems)
{
  std::vector<Item> items = {};
  std::vector<bool> selection;

  int result = TypeParam::solve(items, std::back_inserter(selection), 10,
                                &Item::value, &Item::weight);

  EXPECT_EQ(result, 0);
  EXPECT_TRUE(selection.empty());
}

TYPED_TEST(KnapsackTest, ZeroCapacity)
{
  std::vector<Item> items = {{"A", 100, 5}};

  auto [value, selection] = this->runKnapsack(items, 0);

  EXPECT_EQ(value, 0);
  EXPECT_EQ(selection, std::vector<bool>{false});
}

TYPED_TEST(KnapsackTest, SingleItemFits)
{
  std::vector<Item> items = {{"A", 50, 5}};

  auto [value, selection] = this->runKnapsack(items, 10);

  EXPECT_EQ(value, 50);
  EXPECT_EQ(selection, std::vector<bool>{true});
}

TYPED_TEST(KnapsackTest, SingleItemTooHeavy)
{
  std::vector<Item> items = {{"A", 50, 15}};

  auto [value, selection] = this->runKnapsack(items, 10);

  EXPECT_EQ(value, 0);
  EXPECT_EQ(selection, std::vector<bool>{false});
}

TYPED_TEST(KnapsackTest, AllItemsFit)
{
  std::vector<Item> items = {
    {"A", 10, 2}, {"B", 20, 3}, {"C", 30, 4}
  };

  auto [value, selection] = this->runKnapsack(items, 10);  // capacity >= 2+3+4=9

  EXPECT_EQ(value, 60);
  EXPECT_EQ(selection, (std::vector<bool>{true, true, true}));
}

TYPED_TEST(KnapsackTest, NoItemsFit)
{
  std::vector<Item> items = {
    {"A", 10, 5}, {"B", 20, 6}, {"C", 30, 7}
  };

  auto [value, selection] = this->runKnapsack(items, 3);  // all items too heavy

  EXPECT_EQ(value, 0);
  EXPECT_EQ(selection, (std::vector<bool>{false, false, false}));
}

TYPED_TEST(KnapsackTest, GreedyFails_ClassicCounterExample)
{
  // Classic example where greedy by value/weight ratio fails
  // Ratios: A=6, B=5, C=4
  // Greedy takes A+B = 160, weight 30
  // Optimal is B+C = 220, weight 50
  // Classic example where greedy by value/weight ratio fails
  // Ratios: A=6, B=5, C=4
  // Greedy takes A+B = 160, weight 30
  // Optimal is B+C = 220, weight 50
  std::vector<Item> items = {
    {"A", 60, 10}, {"B", 100, 20}, {"C", 120, 30}
  };

  auto [value, selection] = this->runKnapsack(items, 50);

  EXPECT_EQ(value, 220);
  EXPECT_EQ(selection, (std::vector<bool>{false, true, true}));
}

TYPED_TEST(KnapsackTest, PerfectFit)
{
  std::vector<Item> items = {
    {"A", 10, 3}, {"B", 20, 4}, {"C", 15, 3}
  };

  auto [value, selection] = this->runKnapsack(items, 6);  // A+C = weight 6 exactly

  // A(10) + C(15) = 25 vs B(20)
  EXPECT_EQ(value, 25);
  EXPECT_EQ(selection, (std::vector<bool>{true, false, true}));
}

TYPED_TEST(KnapsackTest, AsSubsetSumReduction)
{
  // Subset sum : {3, 7, 1, 8, 4}, target = 11
  // As knapsack: value = weight, capacity = target
  std::vector<Item> items = {
    {"", 3, 3}, {"", 7, 7}, {"", 1, 1}, {"", 8, 8}, {"", 4, 4}
  };

  auto [value, selection] = this->runKnapsack(items, 11);

  // If value == 11, subset sum exists
  EXPECT_EQ(value, 11);

  // Verify selection sums to 11
  int sum = 0;
  for (size_t i = 0; i < items.size(); ++i) {
    if (selection[i]) sum += items[i].weight;
  }
  EXPECT_EQ(sum, 11);
}

// Consistency Test - Both algorithms should give same VALUE
// (selection might differ if multiple optimal solutions exist)
TEST(KnapsackConsistency, BothAlgorithmsGiveSameValue)
{
  std::vector<Item> items = {
    {"A", 24, 12}, {"B", 13, 7}, {"C", 23, 11},
    {"D", 15, 8}, {"E", 16, 9}, {"F", 10, 5}
  };
  int capacity = 30;

  std::vector<bool> sel_dp, sel_bf;
  int val_dp = alg::knapsack_01(items, std::back_inserter(sel_dp), capacity,
                                 &Item::value, &Item::weight);
  int val_bf = alg::knapsack_01_bruteforce(items, std::back_inserter(sel_bf), capacity,
                                            &Item::value, &Item::weight);

  EXPECT_EQ(val_dp, val_bf);
}

TEST(KnapsackConsistency, MultipleRandomTests)
{
  // Test with several random configurations
  std::vector<std::vector<Item>> test_cases = {
    {{"a", 5, 3}, {"b", 8, 4}, {"c", 12, 6}},
    {{"a", 1, 1}, {"b", 2, 2}, {"c", 3, 3}, {"d", 4, 4}},
    {{"a", 10, 5}, {"b", 15, 7}, {"c", 7, 3}, {"d", 8, 4}, {"e", 9, 5}},
  };

  std::vector<int> capacities = {10, 7, 15};

  for (size_t i = 0; i < test_cases.size(); ++i) {
    std::vector<bool> sel_dp, sel_bf;
    int val_dp = alg::knapsack_01(test_cases[i], std::back_inserter(sel_dp),
                                   capacities[i], &Item::value, &Item::weight);
    int val_bf = alg::knapsack_01_bruteforce(test_cases[i], std::back_inserter(sel_bf),
                                              capacities[i], &Item::value, &Item::weight);

    EXPECT_EQ(val_dp, val_bf) << "Mismatch at test case " << i;
  }
}