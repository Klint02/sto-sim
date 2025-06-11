#include <iostream>
#include <vector>

#include "stosim.hpp"

int main(){
    std::cout << "Hello, from sto-sim!\n";
    std::vector<int> sims = {33,5,20,1};
    runSimulations(sims, true);

    return 0;
}
