#ifndef STOSIM_HEADER
#define STOSIM_HEADER
namespace stochastic {


    enum class SimCodes
    {
        SUCCESS,
        FAILURE,
        TOTAL_FAILURE
    };

    enum class SymbolTableCodes {
        SUCCESS,
        ALREADY_PRESENT,
        
    };

    void runSimulation(int simulation);

    void runSimulations(const std::vector<int>& simulation_vessels, bool multithread) /*-> std::expected<std::string, SimCodes>*/;


    class Vessel {
        public:
            Vessel(std::string name);
    
            auto add(std::string key, int value) -> std::expected<int, SymbolTableCodes>;
    
        private:
            std::string name;
            std::unordered_map<std::string, int> symbol_table;
    };

}
#endif