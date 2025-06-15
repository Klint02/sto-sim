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
            double compute_delay(double lambda, const std::vector<Vessel>& R, const std::map<std::string,int>& state);

            auto add(std::string key, int value) -> std::expected<int, SymbolTableCodes>;
            auto getName();
        private:
            std::string name;
            std::unordered_map<std::string, int> symbol_table;
    };
    struct Reaction{
        double lambda;
        std::vector<std::string> inputs;
        std::vector<std::string> products;
        double computeDelay(const std::map<std::string, int> &state);
        bool isAble(const std::map<std::string, int> &state);
        void update(std::map<std::string, int> &state);
    };
    void simulate(std::vector<Reaction> &reactions, int T, std::map<std::string, int> &state);
        
}
#endif