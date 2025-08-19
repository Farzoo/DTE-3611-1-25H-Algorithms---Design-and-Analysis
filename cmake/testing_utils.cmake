macro(SETUP_UNITTEST_ENV)

  include(FetchContent)

  # Configure threads
  find_package(Threads REQUIRED)

  if(NOT TARGET GTest::gtest AND NOT TARGET gtest)
      find_package(GTest QUIET)

      if(NOT GTest_FOUND)
          FetchContent_Declare(
                  googletest
                  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
          )
          set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
          set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
          FetchContent_MakeAvailable(googletest)
      endif()

      if(TARGET gtest AND NOT TARGET GTest::GTest)
          add_library(GTest::GTest ALIAS gtest)
      endif()

      if(TARGET gtest_main AND NOT TARGET GTest::Main)
          add_library(GTest::Main ALIAS gtest_main)
      endif()
  endif()

  set(DTE3611_HAVE_GTEST TRUE CACHE BOOL "googletest available" FORCE)

  if(TARGET GTest::GTest)

      if(TARGET gtest)
          target_compile_options(gtest PRIVATE
                  $<$<CXX_COMPILER_ID:GNU>:-w>
                  $<$<CXX_COMPILER_ID:Clang>:-w>
          )
      endif()
      if(TARGET gtest_main)
          target_compile_options(gtest_main PRIVATE
                  $<$<CXX_COMPILER_ID:GNU>:-w>
                  $<$<CXX_COMPILER_ID:Clang>:-w>
          )
      endif()
      
    enable_testing()

    # GTEST compile options
    set( GTEST_CLANG_COMPILE_OPTIONS
      # option
      )
    set( GTEST_GCC_COMPILE_OPTIONS
      # option
      )
    set( GTEST_MSVC_COMPILE_OPTIONS
      # option
      )

  endif(TARGET GTest::GTest)
endmacro(SETUP_UNITTEST_ENV)


function(ADD_UNITTESTS UNITTEST_LIB_TOTEST UNITTESTS_INN)
  # Add tests
  # -- ARGN :: additional LIB dependencies

  foreach(TEST ${${UNITTESTS_INN}})

    set( TESTNAME unittest_${TEST} )
    add_executable( ${TESTNAME} )

    target_sources( ${TESTNAME}
      PRIVATE ${TEST}.cpp ${SRCS} )

    target_link_libraries( ${TESTNAME}
      PRIVATE
        ${UNITTEST_LIB_TOTEST}
        ${ARGN}
        GTest::GTest GTest::Main
        Threads::Threads )

    # C++ standard feature set
    target_compile_features( ${TESTNAME}
      PUBLIC $<$<CXX_COMPILER_ID:Clang>:cxx_std_20>
      PUBLIC $<$<CXX_COMPILER_ID:GNU>:cxx_std_20>
      PUBLIC $<$<CXX_COMPILER_ID:MSVC>:cxx_std_20>
      )

    # Compile definitions
    target_compile_definitions( ${TESTNAME}
      PUBLIC $<$<CXX_COMPILER_ID:MSVC>: _USE_MATH_DEFINES>
      )

    # Comple options
    target_compile_options( ${TESTNAME}
      PUBLIC $<$<CXX_COMPILER_ID:Clang>: ${CLANG_COMPILE_OPTIONS} ${GTEST_CLANG_COMPILE_OPTIONS}>
      PUBLIC $<$<CXX_COMPILER_ID:GNU>:   ${GCC_COMPILE_OPTIONS}   ${GTEST_GCC_COMPILE_OPTIONS}>
      PUBLIC $<$<CXX_COMPILER_ID:MSVC>:  ${MSVC_COMPILE_OPTIONS}  ${GTEST_MSVC_COMPILE_OPTIONS}>
      )

    # Turn off platform-spesific extensions
    set_target_properties(${TESTNAME} PROPERTIES CXX_EXTENSIONS OFF)

    gtest_discover_tests( ${TESTNAME} "" AUTO )

  endforeach(TEST)

endfunction(ADD_UNITTESTS)



macro(SETUP_BENCHMARK_ENV)

    include(FetchContent)

    # Enable testing (ctest)
    enable_testing()

    # Threads
    find_package(Threads REQUIRED)

    # Try system package first
    find_package(benchmark QUIET)

    if(NOT benchmark_FOUND)
        # FetchContent fallback
        FetchContent_Declare(
                googlebenchmark
                URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
        )
        # No self-tests, no gtest, no -Werror
        set(BENCHMARK_ENABLE_TESTING       OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_ENABLE_GTEST_TESTS   OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_ENABLE_WERROR        OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(googlebenchmark)
    endif()

    if(TARGET benchmark AND NOT TARGET benchmark::benchmark)
        add_library(benchmark::benchmark ALIAS benchmark)
    endif()
    if(TARGET benchmark_main AND NOT TARGET benchmark::benchmark_main)
        add_library(benchmark::benchmark_main ALIAS benchmark_main)
    endif()

    if(TARGET benchmark)
        target_compile_options(benchmark PRIVATE
                $<$<CXX_COMPILER_ID:GNU>:-w>
                $<$<CXX_COMPILER_ID:Clang>:-w>
                # $<$<CXX_COMPILER_ID:MSVC>:/w>  # décommente si MSVC est trop bruyant
        )
    endif()
    if(TARGET benchmark_main)
        target_compile_options(benchmark_main PRIVATE
                $<$<CXX_COMPILER_ID:GNU>:-w>
                $<$<CXX_COMPILER_ID:Clang>:-w>
                # $<$<CXX_COMPILER_ID:MSVC>:/w>
        )
    endif()

    set( BENCHMARK_CLANG_COMPILE_OPTIONS )
    set( BENCHMARK_GCC_COMPILE_OPTIONS )
    set( BENCHMARK_MSVC_COMPILE_OPTIONS )

endmacro(SETUP_BENCHMARK_ENV)



function(ADD_BENCHMARKS BENCHMARKS_LIB_TOTEST BENCHMARKS_INN)
  # Add tests
  # -- ARGN :: additional LIB dependencies

  foreach(BENCHMARK ${${BENCHMARKS_INN}})
    set(BNAME  benchmark_${BENCHMARK})
    add_executable( ${BNAME} )

    target_sources( ${BNAME} PRIVATE ${BENCHMARK}.cpp )

    target_link_libraries( ${BNAME}
      PRIVATE
        ${BENCHMARKS_LIB_TOTEST}
        ${ARGN}
        benchmark::benchmark
        Threads::Threads )

    # Turn on c++20 compile features -- minimum CMake Version 3.8 required
    target_compile_features( ${BNAME}
      PUBLIC $<$<CXX_COMPILER_ID:Clang>:cxx_std_20>
      PUBLIC $<$<CXX_COMPILER_ID:GNU>:cxx_std_20>
      PUBLIC $<$<CXX_COMPILER_ID:MSVC>:cxx_std_20>
      )

    # Compile definitions
    target_compile_definitions( ${BNAME}
      PUBLIC $<$<CXX_COMPILER_ID:MSVC>: _USE_MATH_DEFINES>
      )

    # Comple options
    target_compile_options( ${BNAME}
      PUBLIC $<$<CXX_COMPILER_ID:Clang>: ${CLANG_COMPILE_OPTIONS} ${BENCHMARK_CLANG_COMPILE_OPTIONS}>
      PUBLIC $<$<CXX_COMPILER_ID:GNU>:   ${GCC_COMPILE_OPTIONS}   ${BENCHMARK_GCC_COMPILE_OPTIONS}>
      PUBLIC $<$<CXX_COMPILER_ID:MSVC>:  ${MSVC_COMPILE_OPTIONS}  ${BENCHMARK_MSVC_COMPILE_OPTIONS}>
      )

    # Turn off platform-spesific extensions
    set_target_properties(${BNAME} PROPERTIES CXX_EXTENSIONS OFF)

    add_test( NAME ${BNAME} COMMAND ${BNAME} )
  endforeach(BENCHMARK ${BENCHMARKS})

endfunction(ADD_BENCHMARKS)

