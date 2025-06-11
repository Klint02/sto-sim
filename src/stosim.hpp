#ifndef STOSIM_HEADER
#define STOSIM_HEADER

enum class SimCodes
{
    SUCCESS,
    FAILURE,
    TOTAL_FAILURE
};

void runSimulation(int simulation);

void runSimulations(const std::vector<int>& simulation_vessels, bool multithread) /*-> std::expected<std::string, SimCodes>*/;


#endif