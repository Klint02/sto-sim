#include <iostream>
#include <vector>
#include <unordered_map> //Should not need to be included. Look into why
#include <expected> //Should not need to be included. Look into why

#include "stosim.hpp"

int main(){
    std::cout << "Hello, from sto-sim!\n";
    std::vector<int> sims = {33,5,20,1};
    stochastic::runSimulations(sims, true);

    return 0;
}
