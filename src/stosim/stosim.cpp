#include <expected>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>
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

    Vessel::Vessel(std::string name)
    {
        this->name = name;
    }

    int &Vessel::environment()
    {
        return env;
    }

    // R3
    // Creates/ adds a reactant to the symbol_table of a vessel. If a symbol is already present it returns that error code
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

    // R3
    // Lookup of things in a symbol_table (should proberbly be placed inside the vessel itself)
    const auto lookup(const std::string &reactant, const std::unordered_map<std::string, int> &state)
    {
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

    // R4
    // Simulation algorithm that runs on a given vessel
    void simulate(Vessel &vessel, SimulationOutputType output_type, std::string path)
    {
        double t = 0;
        std::ostringstream output;

        while (t <= vessel.sim_duration)
        {
            std::vector<double> delays;
            for (auto &r : vessel.reactions)
            {
                delays.push_back(r.computeDelay(vessel.symbol_table, vessel.sim_duration));
            }

            double delay = *std::min_element(delays.begin(), delays.end());
            t += delay;
            auto min_it = std::min_element(delays.begin(), delays.end());
            int r_index = std::distance(delays.begin(), min_it);
            if (t > vessel.sim_duration)
                break;
            Reaction &r = vessel.reactions[r_index];
            if (r.isAble(vessel.symbol_table))
            {
                r.update(vessel.symbol_table);
            }

            if (output_type == SimulationOutputType::FILE || output_type == SimulationOutputType::CONCLUSION)
            {
                for (const auto &[name, amount] : vessel.symbol_table)
                {
                    output << amount << "|";
                }
                output << "\n";
            }
            else if (output_type == SimulationOutputType::LIVE)
            {
                std::cout << "t = " << t << ": ";
                for (const auto &[name, amount] : vessel.symbol_table)
                {
                    std::cout << name << "=" << amount << " ";
                }
                std::cout << "\n";
            }
        }

        // R6
        // dumping of data to a file
        if (output_type == SimulationOutputType::FILE)
        {
            std::ofstream myfile;
            myfile.open(path, std::ios::trunc);
            myfile << vessel.csv_fields;
            myfile << output.rdbuf();
            myfile.close();
        }
        else if (output_type == SimulationOutputType::CONCLUSION)
        {
            std::cout <<  vessel.csv_fields + output.str() << std::endl;;
        }
    }

    // R8
    // TODO(NKC): Implement multithreading
    void runSimulations(std::vector<Vessel> simulation_vessels, bool multithread, int amount) //-> std::expected<std::string, SimCodes>
    {
        if (multithread)
        {
            std::vector<std::future<void>> simulations;
            for (auto& simulation : simulation_vessels)
            {
                if (simulations.size() >= amount) {
                    //std::cout << "thread threshhold reached. Waiting for dequeue" << std::endl;
                }
                while (simulations.size() >= amount) {
                    bool done = true;
                    for (auto& simulation : simulations) {
                        if (simulation.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
                            done = false;
                        }
                    }
                    if (done) {
                        simulations.clear();
                        //std::cout << "Simulation batch done. Dequeueing " << std::endl;

                    }
                }
                simulations.emplace_back(std::async(std::launch::async, [&]() {
                    simulate(simulation, SimulationOutputType::NONE, "");
                }));
            }

            for (auto& s : simulations)
            {
                s.get();
            }
        }
        else
        {
            for (auto& simulation : simulation_vessels)
            {
                simulate(simulation, SimulationOutputType::NONE, "");
            }
        }

        std::cout << "Done with simulations" << std::endl;
    }

    // R1
    // Our type for the reaction overloads is an std::expected containing a string or a reason for why the reaction failed.
    // This lets us check for failed reactions by simply using an if statement for both lhs and rhs.
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
            //std::cout << "Shit aint implemented yet" << std::endl;
            return lhs;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

}
