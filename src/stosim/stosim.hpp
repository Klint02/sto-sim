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
        template <typename T>
        double computeDelay(const std::unordered_map<std::string, T> &state,double Time);
        bool isAble(const std::unordered_map<std::string, int> &state);
        void update(std::unordered_map<std::string, int> &state);
        
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
            int env;
            
            Vessel(std::string name);
            

            int& environment();
            auto getName();

            template <typename T>
            auto add(std::string key, T value) -> std::expected<std::string, SymbolTableCodes>;

            template <typename T>
            T get(const std::string& key) const;
            void add(std::expected<stochastic::Reaction, stochastic::SymbolTableCodes> reaction);
            std::vector<Reaction> reactions;
            std::unordered_map<std::string, std::any> symbol_table;

        private:
            std::string name;
    };

    //void simulate(Vessel &vessel, double T );
        

    auto operator+(std::expected<std::string, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>(std::expected<std::string, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>(std::expected<Reaction, SymbolTableCodes> lhs, double rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<Reaction, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, std::expected<std::string, SymbolTableCodes> rhs) -> std::expected<Reaction, SymbolTableCodes>;
    auto operator>>=(std::expected<Reaction, SymbolTableCodes> lhs, const int rhs) -> std::expected<Reaction, SymbolTableCodes>;


}


#endif