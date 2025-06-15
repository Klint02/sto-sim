#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_map> //Should not need to be included. Look into why
#include <expected>      //Should not need to be included. Look into why
#include <algorithm>
#include "stosim/stosim.hpp"
#include "vessels/vessels.hpp"

int main(){
    std::cout << "Hello, from sto-sim!\n";
    std::vector<int> sims = {33,5,20,1};
    //stochastic::runSimulations(sims, true);


    auto bab = stochastic::Vessel("Heelo");
    auto A = bab.add("A", 1);
    auto B = bab.add("B", 0);
    auto C = bab.add("C", 3);
    auto D = bab.add("C", 3);
    double lambda = 0.001;
    double beta = 50;

    if (D.error() == stochastic::SymbolTableCodes::ALREADY_PRESENT) {
        std::cout << "Error: Shit already present, my guy" << std::endl;
    }

    auto test = (A + C) >> lambda >>= B + C;

    if (test.error() == stochastic::SymbolTableCodes::MALFORMED_REACTANT) {
        std::cout << "Error: Yall used a malformed reactant, smh" << std::endl;
    } else {
        std::cout << test.value() << std::endl;
    }

    stochastic::Vessel v = simple();

    // Define state: A = 100, B = 0, C = 1
    std::map<std::string, int> state = {
        {"A", 100},
        {"B", 0},
        {"C", 1}};

    // Define reactions (e.g., A + C -> B + C)
    std::vector<stochastic::Reaction> reactions = {test.value()};

    // Run simulation up to time T = 100
    stochastic::simulate(reactions, 1600, state);


    return 0;
}