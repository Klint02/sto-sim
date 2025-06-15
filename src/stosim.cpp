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
    auto Vessel::getName()
    {
        return this->name;
    }
    auto Vessel::add(std::string key, int value) -> std::expected<int, SymbolTableCodes>
    {
        if (symbol_table.try_emplace(key, value).second)
        {
            return symbol_table[key];
        }
        else
        {
            return std::unexpected(SymbolTableCodes::ALREADY_PRESENT);
        }
    }
    

   
        double lambda;
        std::vector<std::string> inputs;
        std::vector<std::string> products;

        double Reaction::computeDelay(const std::map<std::string, int> &state)
        {
            double product = 1.0;
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
}
