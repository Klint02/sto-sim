#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_map> //Should not need to be included. Look into why
#include <expected>      //Should not need to be included. Look into why
#include <algorithm>
#include <any>
#include "stosim/stosim.hpp"
#include "vessels/vessels.hpp"

int main(){
    std::cout << "Hello, from sto-sim!\n";
    std::vector<int> sims = {33,5,20,1};
    //stochastic::runSimulations(sims, true);



    //const auto A = bab.add("A","Heelo");
    //const auto B = bab.add("B",10);
    //const auto D = bab.add("D",true);

    auto bab = stochastic::Vessel("Heelo");
    const auto C = bab.add("C",10.01);
    const auto bim = bab.get<double>(C.value());
    
    std::cout << bim << std::endl;

    // if (D.error() == stochastic::SymbolTableCodes::ALREADY_PRESENT) {
    //     std::cout << "Error: Shit already present, my guy" << std::endl;
    // }

    // auto test = (A + C) >> lambda >>= B + C;

    // if (test.error() == stochastic::SymbolTableCodes::MALFORMED_REACTANT) {
    //     std::cout << "Error: Yall used a malformed reactant, smh" << std::endl;
    // } else {
    //     std::cout << test.value() << std::endl;
    // }

    //stochastic::Vessel v = simple();

    // Define state: A = 100, B = 0, C = 1
    

    // Define reactions (e.g., A + C -> B + C)
    //std::vector<stochastic::Reaction> reactions = {test.value()};

    // Run simulation up to time T = 100
    //stochastic::simulate(bab, 100.0);


    return 0;
}