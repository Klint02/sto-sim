#include <expected>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <cmath>
#include <random>
#include <map>
#include <limits>
#include <cfloat>
#include <fstream>

#include "stosim.hpp"

namespace stochastic
{

    std::ostream &operator<<(std::ostream &os, const Reaction &obj)
    {
        for (auto &reactant : obj.inputs)
        {
            os << reactant << " ";
        }

        os << "-(" << obj.delay << ")-> ";

        for (auto &reactant : obj.products)
        {
            os << reactant << " ";
        }

        return os;
    }

    // TODO(NKC): Fix overloading so one may call runSim(int) and runSim(vector<int>, bool)
    void runSimulation(int simulation)
    {
        std::this_thread::sleep_for(std::chrono::seconds(simulation));
        std::cout << simulation << std::endl;
    }

    //R8
    //TODO(NKC): Implement multithreading
    void runSimulations(const std::vector<int> &simulation_vessels, bool multithread) //-> std::expected<std::string, SimCodes>
    {
        if (multithread)
        {
            std::vector<std::jthread> simulations;
            for (const auto &simulation : simulation_vessels)
            {
                simulations.emplace_back(runSimulation, simulation);
            }

            for (auto &s : simulations)
            {
                if (s.joinable())
                {
                    s.join();
                }
            }
        }
        else
        {
            for (const auto &simulation : simulation_vessels)
            {
                runSimulation(simulation);
            }
        }

        std::cout << "Done with simulations" << std::endl;
    }

    Vessel::Vessel(std::string name)
    {
        this->name = name;
    }

    int &Vessel::environment()
    {
        return env;
    }

    //R3
    //Creates/ adds a reactant to the symbol_table of a vessel. If a symbol is already present it returns that error code 
    auto Vessel::add(std::string key, int value) -> std::expected<std::string, SymbolTableCodes>
    {
        if (symbol_table.try_emplace(key, value).second)
        {
            return key;
        }
        else
        {
            return std::unexpected(SymbolTableCodes::ALREADY_PRESENT);
        }
    }

    void Vessel::add(std::expected<stochastic::Reaction, stochastic::SymbolTableCodes> reaction)
    {
        if (reaction)
        {
            reactions.push_back(reaction.value());
        }
    }

    //R3
    //Lookup of things in a symbol_table (should proberbly be placed inside the vessel itself)
    const auto lookup (const std::string& reactant, const std::unordered_map<std::string, int> &state) {
        return state.find(reactant);
    }


    double Reaction::computeDelay(const std::unordered_map<std::string, int> &state, double T)
    {
        double product = 1.0;
        for (const auto &reactant : inputs)
        {
            const auto reactant_state = lookup(reactant, state);
            if (reactant_state != state.end() && reactant_state->second > 0)
            {
                product *= reactant_state->second;
            }
            else
            {
                return T + 1;
            }
        }

        double rate = delay * product;

        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::exponential_distribution<> exp_dist(rate);

        return exp_dist(gen);
    }

    bool Reaction::isAble(const std::unordered_map<std::string, int> &state)
    {
        for (const auto &reactant : inputs)
        {
            if (state.at(reactant) <= 0)
            {
                return false;
            }
        }
        return true;
    }

    void Reaction::update(std::unordered_map<std::string, int> &state)
    {
        for (const auto &reactant : inputs)
        {
            state[reactant] -= 1;
        }
        for (const auto &reactant : products)
        {
            state[reactant] += 1;
        }
    }


    //R4
    //Simulation algorithm that runs on a given vessel
    void simulate(Vessel &vessel, double T, bool csv)
    {
        double t = 0;
        std::ofstream myfile;
        myfile.open("tester.csv",std::ios::trunc);
        myfile << vessel.csv_fields;

        while (t <= T)
        {
            std::vector<double> delays;
            for (auto &r : vessel.reactions)
            {
                delays.push_back(r.computeDelay(vessel.symbol_table, T));
            }

            double delay = *std::min_element(delays.begin(), delays.end());
            t += delay;
            auto min_it = std::min_element(delays.begin(), delays.end());
            int r_index = std::distance(delays.begin(), min_it);
            if (t > T)
                break;
            Reaction &r = vessel.reactions[r_index];
            if (r.isAble(vessel.symbol_table))
            {
                r.update(vessel.symbol_table);
            }

            //R6
            //dumping of data to a file
            if (csv)
            {
                
                myfile << t << "|";
                for (const auto &[name, amount] : vessel.symbol_table)
                {
                    myfile << amount << "|";
                }
                myfile << "\n"; 
                
            }
            else
            //Printing state in terminal each round.
            {

                std::cout << "t = " << t << ": ";
                for (const auto &[name, amount] : vessel.symbol_table)
                {
                    std::cout << name << "=" << amount << " ";
                }
                std::cout << "\n";
            }
        }
        myfile.close();
    }

    //R1
    //Our type for the reaction overloads is an std::expected containing a string or a reason for why the reaction failed.
    //This lets us check for failed reactions by simply using an if statement for both lhs and rhs.
    auto operator+(std::expected<std::string, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>
    {
        if (lhs && rhs)
        {
            Reaction reaction;
            reaction.inputs.push_back(lhs.value());
            reaction.inputs.push_back(rhs.value());
            return reaction;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>(std::expected<std::string, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>
    {
        if (lhs)
        {
            Reaction reaction;
            reaction.inputs.push_back(lhs.value());
            reaction.delay = rhs;
            return reaction;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>(std::expected<Reaction, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>
    {
        if (lhs)
        {
            lhs.value().delay = rhs;
            return lhs;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<Reaction, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>
    {
        if (lhs && rhs)
        {
            lhs.value().products.swap(rhs.value().inputs);
            return lhs;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>
    {
        if (lhs && rhs)
        {
            lhs.value().products.push_back(rhs.value());
            return lhs;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, const int rhs) -> std::expected<Reaction, SymbolTableCodes>
    {
        if (lhs)
        {
            std::cout << "Shit aint implemented yet" << std::endl;
            return lhs;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

}
