#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_map> 
#include <expected>      
#include <algorithm>
#include "stosim.hpp"

int main(){
    std::cout << "Hello, from sto-sim!\n";
    std::vector<int> sims = {33,5,20,1};
    //stochastic::runSimulations(sims, true);


    auto bab = stochastic::Vessel("Heelo");
    auto A = bab.add("A", 100);
    auto B = bab.add("B", 0);
    auto C = bab.add("C", 1);
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

    bab.inputs.push_back(test.value());

    // Define reactions (e.g., A + C -> B + C)
    std::vector<stochastic::Reaction> reactions = {test.value()};

    // Run simulation up to time T = 100
    stochastic::simulate(bab, 100);


    return 0;
}