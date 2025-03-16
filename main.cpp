#include <iostream>
#include <iomanip>
#include <random>
#include <fstream>
#include "json/json.h"

using namespace std;

int main()
{
    // Updating the json
    // Flower: ^Peony$|Tulip$|Rose$|Daisy$|Lilac$|Hyacinth$|Nemesia$|Snapdragon
    // Fish: ^Shrimp$|Cod$|Salmon$|Bass$|Lobster$|Swordfish$|Shark$|King Crab
    // Bone: ^(?:\w+\s)?(bone|fang)$


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
    float insatiable_pies_per_hour = ((HOUR_IN_SECONDS * values["player"]["insatiable"].asFloat()) / values["player"]["food_health"].asFloat());
    float insatiable_cost_per_hour = insatiable_pies_per_hour * values["player"]["food_cost"].asFloat();

    for (unsigned int i = 0; i < values["dungeon"].size(); i++) {
        float food_cost_per_hour = (values["player"]["food_cost"].asFloat() * values["dungeon"][i]["food_hour"].asFloat()) + insatiable_cost_per_hour;
        float keys_per_hour = values["dungeon"][i]["monsters_hour"].asFloat() / LOOT_ROLLS_PER_KEY;
        float keys_cost_per_hour = keys_per_hour * values["key_values"][i]["cost"].asInt();
        float sim_food_cost = food_cost_per_hour * simulated_hours;
        float sim_potion_cost = potion_cost_per_hour * simulated_hours;
        long long generated_sim_gold = 0;
        float time_per_key = HOUR_IN_SECONDS / keys_per_hour;
        float total_time_per_key_roll = 0;
        int total_loot_cycles = 0;
        int total_keys_used = 0;

        int total_bone = 0;
        int total_bone_one = 0;
        int total_bone_two = 0;
        int total_log = 0;
        int total_ore = 0;
        int total_flower = 0;
        int total_fish = 0;
        int total_gem_one = 0;
        int total_gem_two = 0;

        std::cout << "---------------------------------------------------------" << "\n";

        std::cout << "Dungeon Level: " << values["dungeon"][i]["level"] << "\n";
        std::cout << "Insatiable Pies Per Hour: " << insatiable_pies_per_hour << "\n";
        std::cout << "Insatiable Pies Cost Per Hour: " << insatiable_cost_per_hour << "\n";
        std::cout << "Pies Per Hour: " << values["dungeon"][i]["food_hour"].asFloat() << "\n";
        std::cout << "Pies Cost Per Hour: " << values["dungeon"][i]["food_hour"].asFloat() * values["player"]["food_cost"].asFloat() << "\n";
        std::cout << "Total Pies Per Hour: " << insatiable_pies_per_hour + values["dungeon"][i]["food_hour"].asFloat() << "\n";
        std::cout << "Total Food Cost Per Hour: " << food_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Food Cost Per Day: " << food_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Food Cost Sim: " << sim_food_cost << "\n";

        std::cout << "\n";

        std::cout << "Potion Cost Per Hour: " << potion_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Potion Cost Per Day: " << potion_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Potion Cost Sim: " << sim_potion_cost << "\n";

        std::cout << "\n";

        std::uniform_int_distribution<> dist{values["dungeon"][i]["drops"]["gold"]["gold_min"].asInt(), values["dungeon"][i]["drops"]["gold"]["gold_max"].asInt()};
        std::uniform_real_distribution<> dist_roll{0, 1};

        for(int j = 0; j < (simulated_hours * keys_per_hour); ++j) {
            ++total_keys_used;

            if (values["player"]["preserve_dungeon"].asFloat() >= dist_roll(gen)) {
                j -= 1;
            }

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

            if (values["player"]["double_loot_chance"].asFloat() >= dist_roll(gen)) {
                rolls += 3;
            }

            // Check for efficiency
            if (values["player"]["skill_efficiency"].asFloat() >= dist_roll(gen)) {
                rolls += 3;
                ++j;
                ++total_keys_used;

                if (values["player"]["preserve_dungeon"].asFloat() >= dist_roll(gen)) {
                    j -= 1;
                }

                if (values["player"]["double_loot_chance"].asFloat() >= dist_roll(gen)) {
                    rolls += 3;
                }
            }

            // Roll loot
            while(counter++ < rolls) {
                if (values["player"]["savage"].asFloat() >= dist_roll(gen)) {
                    total_bone_one += 1;
                    total_bone_two += 1;
                } else {
                    total_bone += 1;
                }

                if (values["dungeon"][i]["drops"]["gold"]["rate"].asFloat() >= dist_roll(gen)) {
                    generated_sim_gold += dist(gen);
                }
                if (values["dungeon"][i]["drops"]["log"]["rate"].asFloat() >= dist_roll(gen)) {
                    total_log += 1;
                }
                if (values["dungeon"][i]["drops"]["ore"]["rate"].asFloat() >= dist_roll(gen)) {
                    total_ore += 1;
                }
                if (values["dungeon"][i]["drops"]["flower"]["rate"].asFloat() >= dist_roll(gen)) {
                    total_flower += 1;
                }
                if (values["dungeon"][i]["drops"]["fish"]["rate"].asFloat() >= dist_roll(gen)) {
                    total_fish += 1;
                }
                if (values["dungeon"][i]["drops"]["gem_one"]["rate"].asFloat() >= dist_roll(gen)) {
                    total_gem_one += 1;
                }
                if (values["dungeon"][i]["drops"]["gem_two"]["rate"].asFloat() >= dist_roll(gen)) {
                    total_gem_two += 1;
                }
            }
            total_loot_cycles += rolls;
        }

        std::cout << "\n";

        string bone_name = values["dungeon"][i]["drops"]["bone"]["name"].asString();
        unsigned int bone_value = total_bone * values["bone"][bone_name]["value"].asInt();

        string bone_one_name = values["dungeon"][i]["drops"]["bone_one"]["name"].asString();
        unsigned int bone_one_value = total_bone_one * values["bone"][bone_one_name]["value"].asInt();

        string bone_two_name = values["dungeon"][i]["drops"]["bone_two"]["name"].asString();
        unsigned int bone_two_value = total_bone_two * values["bone"][bone_two_name]["value"].asInt();

        string log_name = values["dungeon"][i]["drops"]["log"]["name"].asString();
        unsigned int log_value = total_log * values["log"][log_name]["value"].asInt();

        string ore_name = values["dungeon"][i]["drops"]["ore"]["name"].asString();
        unsigned int ore_value = total_ore * values["ore"][ore_name]["value"].asInt();

        string flower_name = values["dungeon"][i]["drops"]["flower"]["name"].asString();
        unsigned int flower_value = total_flower * values["flower"][flower_name]["value"].asInt();

        string fish_name = values["dungeon"][i]["drops"]["fish"]["name"].asString();
        unsigned int fish_value = total_fish * values["fish"][fish_name]["value"].asInt();

        string gemstone_one_name = values["dungeon"][i]["drops"]["gem_one"]["name"].asString();
        unsigned int gem_one_value = total_gem_one * values["gemstone"][gemstone_one_name]["value"].asInt();

        string gemstone_two_name = values["dungeon"][i]["drops"]["gem_two"]["name"].asString();
        unsigned int gem_two_value = total_gem_two * values["gemstone"][gemstone_two_name]["value"].asInt();

        std::cout << "Total " << bone_name << ": " << total_bone << "\n";
        std::cout << "Gross value " << bone_name << ": " << bone_value << "\n";

        std::cout << "Total " << bone_one_name << ": " << total_bone_one << "\n";
        std::cout << "Gross value " << bone_one_name << ": " << bone_one_value << "\n";

        std::cout << "Total " << bone_two_name << ": " << total_bone_two << "\n";
        std::cout << "Gross value " << bone_two_name << ": " << bone_two_value << "\n";

        std::cout << "Total " << log_name << ": " << total_log << "\n";
        std::cout << "Gross value " << log_name << ": " << log_value << "\n";

        std::cout << "Total " << ore_name << ": " << total_log << "\n";
        std::cout << "Gross value " << ore_name << ": " << ore_value << "\n";

        std::cout << "Total " << flower_name << ": " << total_flower << "\n";
        std::cout << "Gross value " << flower_name << ": " << flower_value << "\n";

        std::cout << "Total " << fish_name << ": " << total_fish << "\n";
        std::cout << "Gross value " << fish_name << ": " << fish_value << "\n";

        std::cout << "Total " << gemstone_one_name << ": " << total_gem_one << "\n";
        std::cout << "Gross value " << gemstone_one_name << ": " << gem_one_value << "\n";

        std::cout << "Total " << gemstone_two_name << ": " << total_gem_two << "\n";
        std::cout << "Gross value " << gemstone_two_name << ": " << gem_two_value << "\n";

        std::cout << "\n";

        long long material_gross = log_value + ore_value + flower_value + fish_value + gem_one_value + gem_two_value + bone_value + bone_one_value + bone_two_value;
        long long material_net = material_gross * 0.95;

        std::cout << "Total Gross value from materials: " << material_gross << "\n";
        std::cout << "Total Net value from materials (5% tax): " << material_net << "\n";

        long long generated_gross = generated_sim_gold + material_gross;
        long long generated_net = generated_sim_gold + material_net;

        std::cout << "\n";

        std::cout << "Gross Gold Generated from Sim: " << generated_gross << "\n";
        std::cout << "Gross Gold Generated from Sim Per Hour: " << generated_gross / simulated_hours << "\n";
        std::cout << "Net Gold Generated from Sim: " << generated_net - (keys_cost_per_hour * simulated_hours) - sim_food_cost - sim_potion_cost << "\n";
        std::cout << "Net Gold Generated from Sim Per Hour: " << (generated_net - (keys_cost_per_hour * simulated_hours) - sim_food_cost - sim_potion_cost) / simulated_hours << "\n";
        std::cout << "Gross Gold Per Key: " << generated_gross / (keys_per_hour * simulated_hours) << "\n";
        std::cout << "Net Gold Per Key: " << generated_net / (keys_per_hour * simulated_hours) << "\n";

        // Determine optimal key cost for setup
        int op_key_gold = values["key_values"][i]["cost"].asInt();
        long long op_net_gold = 0;
        bool profit_already = false;

        if ((generated_net - ((op_key_gold * keys_per_hour) * simulated_hours) - sim_food_cost - sim_potion_cost) > 0) {
            profit_already = true;
        }

        while (true) {
            if (op_key_gold == 0) {
                break;
            }

            op_net_gold = (generated_net - ((op_key_gold * keys_per_hour) * simulated_hours) - sim_food_cost - sim_potion_cost);

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
        std::cout << "Total Keys used: " << total_keys_used << "\n";
        std::cout << "Loot Cycles Per Hour: " << (total_loot_cycles / simulated_hours) << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Loot Cycles Sim: " << total_loot_cycles << "\n";
        std::cout << "Keys Per Hour: " << keys_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Per Day: " << keys_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Sim: " << keys_per_hour * simulated_hours << "\n";

        std::cout << "\n";

        std::cout << "Estimated Loot Cycles Per Hour: " << ((keys_per_hour * LOOT_ROLLS_PER_KEY) * (1 + values["player"]["preserve_dungeon"].asFloat())) << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Estimated Loot Cycles Per Day: " << ((keys_per_hour * LOOT_ROLLS_PER_KEY) * (1 + values["player"]["preserve_dungeon"].asFloat())) * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Estimated Loot Cycles Sim: " << ((keys_per_hour * LOOT_ROLLS_PER_KEY) * (1 + values["player"]["preserve_dungeon"].asFloat())) * simulated_hours << "\n";

        std::cout << "\n";

        std::cout << "Keys Cost: " << values["key_values"][i]["cost"].asInt() << "\n";
        std::cout << "Keys Cost Per Hour: " << keys_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Cost Per Day: " << keys_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Cost Sim: " << keys_cost_per_hour * simulated_hours << "\n";

        std::cout << "\n";

        std::cout << "Using the gold to pay for all costs." << "\n";
        std::cout << "The Optimal Net Zero Key Cost is: " << op_key_gold << "\n";

        if (i == values["dungeon"].size() - 1) {
            std::cout << "---------------------------------------------------------" << "\n";
        }
    }
}
