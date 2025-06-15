#include <expected>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <unordered_map>

#include "stosim.hpp"

namespace stochastic {

    std::ostream& operator<<(std::ostream& os, const Reaction& obj) {
        for (auto& reactant : obj.input) {
            os << reactant << " "; 
        }

        os << "-(" << obj.delay << ")-> ";
        
        for (auto& reactant : obj.output) {
            os << reactant << " "; 
        }
        
        return os;
    }

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

    Vessel::Vessel(std::string name) {
        this->name = name;
    }


    auto Vessel::add(std::string key, int value) -> std::expected<std::string, SymbolTableCodes> {
        if (symbol_table.try_emplace(key, value).second) {
            return key;
        } else {
            return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
        }
    }

    auto operator+(std::expected<std::string, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes> {
        if (lhs && rhs) {
            Reaction reaction;
            reaction.input.push_back(lhs.value());
            reaction.input.push_back(rhs.value());
            return reaction;
        }
        //TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>(std::expected<std::string, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes> {
        if (lhs && rhs) {
            Reaction reaction;
            reaction.input.push_back(lhs.value());
            reaction.delay = rhs;
            return reaction;
        }
        //TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>(std::expected<Reaction, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes> {
        if (lhs && rhs) {
            lhs.value().delay = rhs;
            return lhs;
        }
        //TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<Reaction, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes> {
        if (lhs && rhs) {
            lhs.value().output.swap(rhs.value().input);
            return lhs;
        }
        //TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes> {
        if (lhs && rhs) {
            lhs.value().output.push_back(rhs.value());
            return lhs;
        }
        //TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }



}

