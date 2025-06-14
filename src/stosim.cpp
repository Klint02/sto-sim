#include <expected>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <unordered_map>

#include "stosim.hpp"

namespace stochastic {

    //TODO(NKC): Fix overloading so one may call runSim(int) and runSim(vector<int>, bool)
    void runSimulation(int simulation)
    {
        std::this_thread::sleep_for(std::chrono::seconds(simulation));
        std::cout << simulation << std::endl;
    }

    void runSimulations(const std::vector<int>& simulation_vessels, bool multithread) //-> std::expected<std::string, SimCodes>
    {
        if (multithread) {
            std::vector<std::jthread> simulations;
            for (const auto& simulation : simulation_vessels) {
                simulations.emplace_back(runSimulation, simulation);
            }

            for (auto& s : simulations) {
                if (s.joinable()) {
                    s.join();
                }
            }
        } else {
            for (const auto& simulation : simulation_vessels) {
                runSimulation(simulation);
            }
        }

        std::cout << "Done with simulations" << std::endl;
    }

    auto Vessel::add(std::string key, int value) -> std::expected<int, SymbolTableCodes> {
        if (symbol_table.try_emplace(key, value).second) {
            return symbol_table[key];
        } else {
            return std::unexpected(SymbolTableCodes::ALREADY_PRESENT);
        }
    }

}

