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

    auto Vessel::add(std::string key, int value) -> std::expected<std::string, SymbolTableCodes>
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

    double Reaction::computeDelay(const std::map<std::string, int> &state)
    {
        double product = 1.0;
        double lambda = 0.001;
        for (const auto &reactant : inputs)
        {
            auto temp = state.find(reactant);
            if (temp != state.end() && temp->second > 0)
            {
                product *= temp->second;
            }
            else
            {
                return INFINITY;
            }
        }

        double rate = lambda * product;

        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::exponential_distribution<> exp_dist(rate);

        return exp_dist(gen);
    }

    bool Reaction::isAble(const std::map<std::string, int> &state)
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

    void Reaction::update(std::map<std::string, int> &state)
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

    void simulate(std::vector<Reaction> &reactions, int T, std::map<std::string, int> &state)
    {
        double t = 0;

        while (t <= T)
        {
            std::vector<int> delays;
            for (auto &r : reactions)
            {
                delays.push_back(r.computeDelay(state));
            }

            double delay = *std::min_element(delays.begin(), delays.end());
            t += delay;
            auto min_it = std::min_element(delays.begin(), delays.end());
            int r_index = std::distance(delays.begin(), min_it);
            if (t > T)
                break;
            Reaction &r = reactions[r_index];
            if (r.isAble(state))
            {
                r.update(state);
            }

            std::cout << "t = " << t << ": ";
            for (const auto &[name, amount] : state)
            {
                std::cout << name << "=" << amount << " ";
            }
            std::cout << "\n";
        }
    }

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

}
