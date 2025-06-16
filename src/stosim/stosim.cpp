#include <expected>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <cmath>
#include <random>
#include <limits>
#include <cfloat>
#include <any>
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

    int& Vessel::environment() {
        return env;
    }

    template <typename T>
    auto Vessel::add(std::string key, T value) -> std::expected<std::string, SymbolTableCodes>
    {
        if (symbol_table.try_emplace(key, value).second)
        {
            return key;
        }
        else
        {
            return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
        }
    }
    
    void Vessel::add(std::expected<stochastic::Reaction, stochastic::SymbolTableCodes> reaction)
    {
        if (reaction)
        {
            reactions.push_back(reaction.value());
        }
    }

    template <typename T>
    T Vessel::get(const std::string& key) const {
        auto it = symbol_table.find(key);
        if (it != symbol_table.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast& e) {
                throw std::runtime_error("Type mismatch");
            }
        } else {
            throw std::out_of_range("Key not found");
        }
    }
/*
    template <typename T>
    double Reaction::computeDelay(const std::unordered_map<std::string, T> &state, double Time)
    {
        double product = 1.0;
        for (const auto &reactant : inputs)
        {
            const auto reactant_state = state.find(reactant);
            if (reactant_state->second > 0)
            {
                product *= reactant_state->second;
            }
            else
            {
                return Time+1;
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

    void simulate(Vessel &vessel, double T )
    {
        double t = 0;

        while (t <= T)
        {
            std::vector<int> delays;
            for (auto &r : vessel.reactions)
            {
                delays.push_back(r.computeDelay(vessel.symbol_table,T));
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

            std::cout << "t = " << t << ": ";
            for (const auto &[name, amount] : vessel.symbol_table)
            {
                std::cout << name << "=" << amount << " ";
            }
            std::cout << "\n";
        }
    }
*/

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
        if (lhs && rhs)
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
        if (lhs && rhs)
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
        if (lhs && rhs)
        {
            std::cout << "Shit aint implemented yet" << std::endl;
            return lhs;
        }
        // TODO(NKC): More detailed error handling
        return std::unexpected(SymbolTableCodes::MALFORMED_REACTANT);
    }

}
