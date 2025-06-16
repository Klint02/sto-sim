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

    struct Reaction{
        std::vector<std::string> inputs;
        std::vector<std::string> products;
        double delay;
        double computeDelay(const std::unordered_map<std::string, int> &state,double T);
        bool isAble(const std::unordered_map<std::string, int> &state);
        void update(std::unordered_map<std::string, int> &state);
        
        friend std::ostream& operator<<(std::ostream& os, const Reaction& obj);
    };

    enum class SimulationOutputType {
        LIVE,
        FILE,
        CONCLUSION,
        NONE
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



    
    class Vessel {
        public:
            int env;
            
            Vessel(std::string name);
            
            std::string csv_fields;
            int& environment();
            auto getName();
            auto add(std::string key, int value) -> std::expected<std::string, SymbolTableCodes>;
            void add(std::expected<stochastic::Reaction, stochastic::SymbolTableCodes> reaction);
            std::vector<Reaction> reactions;
            std::unordered_map<std::string, int> symbol_table;
            double sim_duration;

        private:
            std::string name;
    };

    void simulate(Vessel &vessel, SimulationOutputType output_type, std::string path);
    void runSimulations(std::vector<Vessel> simulation_vessels, bool multithread, int amount) /*-> std::expected<std::string, SimCodes>*/;

    //R1
    //Overloading headers
    auto operator+(std::expected<std::string, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>(std::expected<std::string, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>(std::expected<Reaction, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<Reaction, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, const int rhs) -> std::expected<Reaction, SymbolTableCodes>;


}
#endif