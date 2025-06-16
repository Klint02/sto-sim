#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_map> //Should not need to be included. Look into why
#include <expected>      //Should not need to be included. Look into why
#include <algorithm>
#include <chrono>
#include <unistd.h>
#include <thread>

#include "stosim/stosim.hpp"
#include "vessels/vessels.hpp"

std::chrono::seconds to_seconds(std::chrono::nanoseconds nanos)
{
    return std::chrono::duration_cast<std::chrono::seconds>(nanos);
}

enum class SimulationType
{
    SIMPLE,
    CIRCADIAN_RHYTHM,
    SEIHR
};

inline void benchmark(SimulationType type, int sim_count, int threads)
{
    std::cout << "Starting " << threads << " threaded benchmark of ";
    const auto before = std::chrono::system_clock::now();

    std::vector<stochastic::Vessel> simulations_simple;
    switch (type)
    {
    case SimulationType::SIMPLE:
        std::cout << sim_count << " SIMPLE algorithm vessels " << std::endl;

        for (int i = 0; i < sim_count; i++)
        {
            simulations_simple.push_back(simple());
        }
        break;

    case SimulationType::CIRCADIAN_RHYTHM:
        std::cout << sim_count << " CIRCADIAN_RHYTHM algorithm vessels " << std::endl;

        for (int i = 0; i < sim_count; i++)
        {
            simulations_simple.push_back(circadian_rhythm());
        }
        break;

    case SimulationType::SEIHR:
        std::cout << sim_count << " SEIHR(10000) algorithm vessels " << std::endl;

        for (int i = 0; i < sim_count; i++)
        {
            simulations_simple.push_back(seihr(10000));
        }
        break;
    }

    if (threads < 2) {
        stochastic::runSimulations(simulations_simple, false, threads);

    } else {
        stochastic::runSimulations(simulations_simple, true, threads);

    }

    const auto after = std::chrono::system_clock::now();
    const auto difference = after - before;
    std::cout << to_seconds(difference) << std::endl;
}

int main()
{
    std::cout << "Hello, from benchmarking!\n";

    // R10
    //Benchmarking runs for the algorithms; 
    //Each run takes 100 vessels and will be run across 1, 2, 4, 8, 16 threads on an imac with a i7-4790k
    //It is expected that 8 and 16 wont be much different as there only are 8 threads in the CPU
    //We can however observe if there is a loss in time due to the queueing system
    benchmark(SimulationType::SIMPLE, 100, 1);
    benchmark(SimulationType::SIMPLE, 100, 2);
    benchmark(SimulationType::SIMPLE, 100, 4);
    benchmark(SimulationType::SIMPLE, 100, 8);
    benchmark(SimulationType::SIMPLE, 100, 16);

    benchmark(SimulationType::CIRCADIAN_RHYTHM, 100, 1);
    benchmark(SimulationType::CIRCADIAN_RHYTHM, 100, 2);
    benchmark(SimulationType::CIRCADIAN_RHYTHM, 100, 4);
    benchmark(SimulationType::CIRCADIAN_RHYTHM, 100, 8);
    benchmark(SimulationType::CIRCADIAN_RHYTHM, 100, 16);

    benchmark(SimulationType::SEIHR, 100, 1);
    benchmark(SimulationType::SEIHR, 100, 2);
    benchmark(SimulationType::SEIHR, 100, 4);
    benchmark(SimulationType::SEIHR, 100, 8);
    benchmark(SimulationType::SEIHR, 100, 16);
    return 0;
}
