
add_subdirectory(predefined_utils)

include(../cmake/testing_utils.cmake)

option(DTE3611_ENABLE_UNITTESTS "Enable : Unittests" OFF)
if(DTE3611_ENABLE_UNITTESTS)

  ## Setup unittest environment variables used by the individual unittests
  SETUP_UNITTEST_ENV()

  ## Guard for GTEST
  if(TARGET GTest::GTest)

    # Unittest template
    add_subdirectory(unittests/templates/unittest_template)

    # Predefined unittests
    add_subdirectory(unittests/predefined/predefined_sort_unittests)
    add_subdirectory(unittests/predefined/predefined_string_match_unittests)
    add_subdirectory(unittests/predefined/predefined_graph_traversal_unittests)
    add_subdirectory(unittests/predefined/predefined_graph_path_finding_unittests)
    add_subdirectory(unittests/predefined/predefined_subset_sum_unittests)
    add_subdirectory(unittests/predefined/predefined_knapsack_unittests)
    add_subdirectory(unittests/predefined/predefined_network_flow_unittests)

    # My Unittests
    add_subdirectory(unittests/my_tests/my_sort_tests)
    add_subdirectory(unittests/my_tests/my_subset_sum_tests)
    add_subdirectory(unittests/my_tests/my_knapsack_tests)

  endif(TARGET GTest::GTest)
endif(DTE3611_ENABLE_UNITTESTS)

## Guard for BENCHMARK
option(DTE3611_ENABLE_BENCHMARKS "Enable : Benchmarks" OFF)
if(DTE3611_ENABLE_BENCHMARKS)
    SETUP_BENCHMARK_ENV()

    if(TARGET benchmark::benchmark)
        add_subdirectory(benchmarks/templates/benchmark_template template)
        add_subdirectory(benchmarks/predefined/predefined_sort_benchmarks sort)
        add_subdirectory(benchmarks/predefined/predefined_string_match_benchmarks string_match)
        add_subdirectory(benchmarks/predefined/predefined_graph_traversal_benchmarks graph_traversal)
        add_subdirectory(benchmarks/predefined/predefined_graph_path_finding_benchmarks graph_path_finding)
        add_subdirectory(benchmarks/my_benchmarks/my_first_benchmarks my_first)
    endif()

endif(DTE3611_ENABLE_BENCHMARKS)
