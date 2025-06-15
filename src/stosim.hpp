#ifndef STOSIM_HEADER
#define STOSIM_HEADER
namespace stochastic {

/*
    struct simulation {
        Vessel Master;
        std::vector<int> timeseries;
        std::vector<Vessel> data;
    };
*/

    struct Reaction {
        std::vector<std::string> input;
        std::vector<std::string> output;
        double delay;

        friend std::ostream& operator<<(std::ostream& os, const Reaction& obj);
    };

    enum class SimCodes
    {
        SUCCESS,
        FAILURE,
        TOTAL_FAILURE
    };

    enum class SymbolTableCodes {
        MALFORMED_REACTANT,
        ALREADY_PRESENT,

    };

    void runSimulation(int simulation);

    void runSimulations(const std::vector<int>& simulation_vessels, bool multithread) /*-> std::expected<std::string, SimCodes>*/;


    class Vessel {
        public:
            Vessel(std::string name);
    
            auto add(std::string key, int value) -> std::expected<std::string, SymbolTableCodes>;
    
        private:
            std::string name;
            std::unordered_map<std::string, int> symbol_table;
    };

    auto operator+(std::expected<std::string, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>(std::expected<std::string, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>(std::expected<Reaction, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<Reaction, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;

}
#endif