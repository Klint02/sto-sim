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

int main(){
    std::cout << "Hello, from sto-sim!\n";

    //R5
    //Usage of the vessel functions and simulation of them
    auto simple_vessel = simple();
    auto circadian_rhythm_vessel = circadian_rhythm();
    auto seihr_vessel = seihr(10000);
    stochastic::simulate(simple_vessel, stochastic::SimulationOutputType::LIVE, "");
    stochastic::simulate(circadian_rhythm_vessel, stochastic::SimulationOutputType::LIVE, "");
    stochastic::simulate(seihr_vessel, stochastic::SimulationOutputType::LIVE, "");

    return 0;
}