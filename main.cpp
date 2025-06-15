#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_map> //Should not need to be included. Look into why
#include <expected>      //Should not need to be included. Look into why
#include <algorithm>
#include "stosim.hpp"



int main()
{
    // Define state: A = 100, B = 0, C = 1
    std::map<std::string, int> state = {
        {"A", 100},
        {"B", 0},
        {"C", 1}};

    // Define reactions (e.g., A + C -> B + C)
    std::vector<stochastic::Reaction> reactions = {
        stochastic::Reaction{0.001, {"A", "C"}, {"B", "C"}}
    };

    // Run simulation up to time T = 100
    stochastic::simulate(reactions, 1600, state);

    return 0;
}


