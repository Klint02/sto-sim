#include <iostream>
#include <vector>
#include <unordered_map> //Should not need to be included. Look into why
#include <expected> //Should not need to be included. Look into why

#include "stosim.hpp"

int main(){
    std::cout << "Hello, from sto-sim!\n";
    std::vector<int> sims = {33,5,20,1};
    //stochastic::runSimulations(sims, true);


    auto bab = stochastic::Vessel("Heelo");
    auto A = bab.add("A", 1);
    auto B = bab.add("B", 0);
    auto C = bab.add("C", 3);
    auto D = bab.add("C", 3);
    double lambda = 50;
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

    return 0;
}