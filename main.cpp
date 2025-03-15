#include <iostream>
#include <iomanip>
#include <random>
#include <fstream>
#include "json/json.h"

using namespace std;

int main()
{
    // Random
    std::random_device seed;
    std::mt19937 gen{seed()}; // seed the generator

    Json::Value values;
    std::ifstream values_file("values.json", std::ifstream::binary);
    values_file >> values;

    const unsigned int HOURS_IN_DAY = 24;
    const unsigned int LOOT_ROLLS_PER_KEY = 3;
    const unsigned int HOUR_IN_SECONDS = 3600;
    const unsigned int DISPLAY_PRECISION = 15;

    unsigned int simulated_hours = values["config"]["simulated_hours"].asInt();
    float potion_cost_per_hour = (HOUR_IN_SECONDS / values["player"]["potion_speed"].asFloat()) * values["player"]["potion_cost"].asFloat();
    float insatiable_cost_per_hour = ((HOUR_IN_SECONDS * values["player"]["insatiable"].asFloat()) / values["player"]["food_health"].asFloat()) * values["player"]["food_cost"].asFloat();

    for (unsigned int i = 0; i < values["dungeon"].size(); i++) {
        float food_cost_per_hour = (values["player"]["food_cost"].asFloat() * values["dungeon"][i]["food_hour"].asFloat()) + insatiable_cost_per_hour;
        float keys_per_hour = values["dungeon"][i]["monsters_hour"].asFloat() / LOOT_ROLLS_PER_KEY;
        float keys_cost_per_hour = keys_per_hour * values["key_values"][i]["cost"].asInt();
        float sim_food_cost = food_cost_per_hour * simulated_hours;
        float sim_potion_cost = potion_cost_per_hour * simulated_hours;
        long long generated_sim_gold = 0;
        float time_per_key = HOUR_IN_SECONDS / keys_per_hour;
        float total_time_per_key_roll = 0;

        std::cout << "---------------------------------------------------------" << "\n";

        std::cout << "Dungeon Level: " << values["dungeon"][i]["level"] << "\n";
        std::cout << "Food Cost Per Hour: " << food_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Food Cost Per Day: " << food_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Food Cost Sim: " << sim_food_cost << "\n";

        std::cout << "\n";

        std::cout << "Potion Cost Per Hour: " << potion_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Potion Cost Per Day: " << potion_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Potion Cost Sim: " << sim_potion_cost << "\n";

        std::cout << "\n";

        std::cout << "Keys Per Hour: " << keys_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Per Day: " << keys_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Sim: " << keys_per_hour * simulated_hours << "\n";

        std::cout << "\n";

        std::cout << "Loot Cycles Per Hour: " << keys_per_hour * LOOT_ROLLS_PER_KEY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Loot Cycles Per Day: " << (keys_per_hour * LOOT_ROLLS_PER_KEY) * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Loot Cycles Sim: " << (keys_per_hour * LOOT_ROLLS_PER_KEY) * simulated_hours << "\n";

        std::cout << "\n";

        std::cout << "Keys Cost: " << values["key_values"][i]["cost"].asInt() << "\n";
        std::cout << "Keys Cost Per Hour: " << keys_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Cost Per Day: " << keys_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Cost Sim: " << keys_cost_per_hour * simulated_hours << "\n";

        std::cout << "\n";

        std::uniform_int_distribution<> dist{values["dungeon"][i]["gold_min"].asInt(), values["dungeon"][i]["gold_max"].asInt()};
        std::uniform_real_distribution<> dist_roll{0, 1};

        for(unsigned int j = 0; j < (simulated_hours * keys_per_hour); j++) {
            total_time_per_key_roll += time_per_key;
            unsigned counter = 0;
            unsigned rolls = 3;

            // Dungeon
            /*
                > > > Do combat
                > > Roll Loot
                +1 loot -1 key
                > Proc Double Loot
                +1 loot
                > > Proc Efficiency
                +1 loot -1 key
                > Proc Double Loot
                +1 loot
            */

            if (values["player"]["double_loot_chance"] >= dist_roll(gen)) {
                rolls += 3;
            }

            // Check for efficiency
            if (values["player"]["skill_efficiency"] >= dist_roll(gen)) {
                rolls += 3;
                j++;
                if (values["player"]["double_loot_chance"] >= dist_roll(gen)) {
                    rolls += 3;
                }
            }

            // Roll loot
            while(counter++ < rolls) {
                if (0.8 >= dist_roll(gen)) {
                    generated_sim_gold += dist(gen);
                }
            }
        }

        std::cout << "Gross Gold Generated from Sim: " << generated_sim_gold << "\n";
        std::cout << "Gross Gold Generated from Sim Per Hour: " << generated_sim_gold / simulated_hours << "\n";
        std::cout << "Net Gold Generated from Sim: " << generated_sim_gold - (keys_cost_per_hour * simulated_hours) - sim_food_cost - sim_potion_cost << "\n";
        std::cout << "Net Gold Generated from Sim Per Hour: " << (generated_sim_gold - (keys_cost_per_hour * simulated_hours) - sim_food_cost - sim_potion_cost) / simulated_hours << "\n";
        std::cout << "Gross Gold Per Key: " << generated_sim_gold / (keys_per_hour * simulated_hours) << "\n";
        std::cout << "Net Gold Per Key (Gross Gold Per Key - Key Cost): " << generated_sim_gold / (keys_per_hour * simulated_hours) - values["key_values"][i]["cost"].asInt() << "\n";

        // Determine optimal key cost for setup
        int op_key_gold = values["key_values"][i]["cost"].asInt();
        long long op_net_gold = 0;
        bool profit_already = false;

        if ((generated_sim_gold - ((op_key_gold * keys_per_hour) * simulated_hours) - sim_food_cost - sim_potion_cost) > 0) {
            profit_already = true;
        }

        while (true) {
            if (op_key_gold == 0) {
                break;
            }

            op_net_gold = (generated_sim_gold - ((op_key_gold * keys_per_hour) * simulated_hours) - sim_food_cost - sim_potion_cost);

            if (!profit_already && op_net_gold >= 0) {
                break;
            } else if (profit_already && op_net_gold <= 0) {
                // Move to the previous value for near net zero value
                op_key_gold -= 1;
                break;
            }

            if (profit_already) {
                op_key_gold += 1;
            } else {
                op_key_gold -= 1;
            }
        }

        std::cout << "\n";

        std::cout << "Simulated Hours: " << simulated_hours << "\n";
        std::cout << "Total Time Spent: " << total_time_per_key_roll / HOUR_IN_SECONDS << "\n";
        std::cout << "Percent of Simulated Hours to Total Time Spent: " << (total_time_per_key_roll / HOUR_IN_SECONDS ) / simulated_hours  << "\n";
        std::cout << "To do the full Simulated Hours (" << simulated_hours << "h), you need an additional (35-40%) keys: " << (simulated_hours * keys_per_hour) * 0.35 << " or " << (simulated_hours * keys_per_hour) * 0.4 << "\n";

        std::cout << "\n";

        std::cout << "Using the gold to pay for all costs." << "\n";
        std::cout << "The Optimal Net Zero Key Cost is: " << op_key_gold << "\n";

        if (i == values["dungeon"].size() - 1) {
            std::cout << "---------------------------------------------------------" << "\n";
        }
    }
}
