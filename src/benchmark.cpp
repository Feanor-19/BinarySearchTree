#include <benchmark/benchmark.h>

#include <fstream>

#include "driver.hpp"

#include "BST.hpp"
#include <set>

template <class ...Args>
void BM_MY_BST(benchmark::State &state, Args&&... args)
{
    //benchmark arg setup
    auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
    std::string bench_data_filename = std::get<0>(args_tuple); 

    //setup
    std::ifstream file_in(bench_data_filename);
    if (!file_in.is_open())
    {
        state.SkipWithError("Can't open input file");
        return;
    }

    std::ofstream file_out(bench_data_filename + ".my_bst.out");
    if (!file_out.is_open())
    {
        state.SkipWithError("Can't open out file");
        return;
    }

    for (auto _ : state)
    {
        //this code is timed
        Driver::driver<BST<int>, int>(file_in, file_out);
    }
}

template <class ...Args>
void BM_STD_SET(benchmark::State &state, Args&&... args)
{
    //benchmark arg setup
    auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
    std::string bench_data_filename = std::get<0>(args_tuple); 

    //setup
    std::ifstream file_in(bench_data_filename);
    if (!file_in.is_open())
    {
        state.SkipWithError("Can't open input file");
        return;
    }

    std::ofstream file_out(bench_data_filename + ".std_set.out");
    if (!file_out.is_open())
    {
        state.SkipWithError("Can't open out file");
        return;
    }

    for (auto _ : state)
    {
        //this code is timed
        Driver::driver<std::set<int>, int>(file_in, file_out);
    }
}

BENCHMARK_CAPTURE(BM_MY_BST, rand_10000, \
                  std::string("benchmark_data/rand_10000.dat"));

BENCHMARK_CAPTURE(BM_STD_SET, rand_10000, \
                   std::string("benchmark_data/rand_10000.dat"));

BENCHMARK_MAIN();