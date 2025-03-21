#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>
#include "json/json.h"

using namespace std;

// Random
static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 rng{seed};

float drop_roll()
{
    std::uniform_real_distribution<float> roll(0.0, 1.0);
    return roll(rng);
}

float flip_roll()
{
    std::uniform_int_distribution<> roll(0, 1);
    return roll(rng);
}


int main()
{
    // Updating the json
    // Flower: ^Peony$|Tulip$|Rose$|Daisy$|Lilac$|Hyacinth$|Nemesia$|Snapdragon
    // Fish: ^Shrimp$|Cod$|Salmon$|Bass$|Lobster$|Swordfish$|Shark$|King Crab
    // Bone: ^(?:\w+\s)?(bone|fang)$

    Json::Value values;
    std::ifstream values_file("values.json", std::ifstream::binary);
    values_file >> values;

    const unsigned int HOURS_IN_DAY = 24;
    const unsigned int LOOT_ROLLS_PER_KEY = 3;
    const unsigned int HOUR_IN_SECONDS = 3600;
    const unsigned int DISPLAY_PRECISION = 15;
    const unsigned int SIMULATED_HOURS = values["config"]["simulated_hours"].asInt();
    const bool SIMPLE_DISPLAY = values["config"]["simple_display"].asBool();
    const float POTION_COST_PER_HOUR = (HOUR_IN_SECONDS / values["player"]["potion_speed"].asFloat()) * values["player"]["potion_cost"].asFloat();
    const float SIM_POTION_COST = POTION_COST_PER_HOUR * SIMULATED_HOURS;
    const float INSATIABLE_PIES_PER_HOUR = ((HOUR_IN_SECONDS * values["player"]["insatiable_consume_rate"].asFloat()) / values["player"]["food_health"].asFloat());
    const float INSATIABLE_COST_PER_HOUR = INSATIABLE_PIES_PER_HOUR * values["player"]["food_cost"].asFloat();

    float food_cost_per_hour = 0.0;
    float sim_food_cost = 0.0;
    unsigned int keys_per_hour = 0;
    float keys_cost_per_hour = 0;

    long long generated_sim_gold = 0;

    int total_loot_cycles = 0;
    int total_keys_used = 0;
    int total_preserved_keys = 0;
    int total_bone = 0;
    int total_bone_one = 0;
    int total_bone_two = 0;
    int total_log = 0;
    int total_ore = 0;
    int total_flower = 0;
    int total_fish = 0;
    int total_gem_one = 0;
    int total_gem_two = 0;
    unsigned int counter = 0;
    unsigned int rolls = 0;


    for (unsigned int i = 0; i < values["dungeon"].size(); i++) {
        food_cost_per_hour = (values["player"]["food_cost"].asFloat() * values["dungeon"][i]["food_hour"].asFloat()) + INSATIABLE_COST_PER_HOUR;
        sim_food_cost = food_cost_per_hour * SIMULATED_HOURS;

        keys_per_hour = values["dungeon"][i]["monsters_hour"].asFloat() / LOOT_ROLLS_PER_KEY;
        keys_cost_per_hour = keys_per_hour * values["key_values"][i]["cost"].asInt();

        generated_sim_gold = 0;
        total_loot_cycles = 0;
        total_keys_used = 0;
        total_preserved_keys = 0;

        total_bone = 0;
        total_bone_one = 0;
        total_bone_two = 0;
        total_log = 0;
        total_ore = 0;
        total_flower = 0;
        total_fish = 0;
        total_gem_one = 0;
        total_gem_two = 0;

        std::cout << "---------------------------------------------------------" << "\n";
        std::cout << "Dungeon Level: " << values["dungeon"][i]["level"] << "\n";
        if (!SIMPLE_DISPLAY) {

            std::cout << "Insatiable Pies Per Hour: " << INSATIABLE_PIES_PER_HOUR << "\n";
            std::cout << "Insatiable Pies Cost Per Hour: " << INSATIABLE_COST_PER_HOUR << "\n";
            std::cout << "Pies Per Hour: " << values["dungeon"][i]["food_hour"].asFloat() << "\n";
            std::cout << "Pies Cost Per Hour: " << values["dungeon"][i]["food_hour"].asFloat() * values["player"]["food_cost"].asFloat() << "\n";
            std::cout << "Total Pies Per Hour: " << INSATIABLE_PIES_PER_HOUR + values["dungeon"][i]["food_hour"].asFloat() << "\n";
            std::cout << "Total Food Cost Per Hour: " << food_cost_per_hour << "\n";

            std::cout << std::setprecision(DISPLAY_PRECISION) << "Food Cost Sim: " << sim_food_cost << "\n";
        }
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Food Cost Per Day: " << food_cost_per_hour * HOURS_IN_DAY << "\n";

        if (!SIMPLE_DISPLAY) {
            std::cout << "\n";
        }


        if (!SIMPLE_DISPLAY) {
            std::cout << "Potion Cost Per Hour: " << POTION_COST_PER_HOUR << "\n";
        }

        std::cout << std::setprecision(DISPLAY_PRECISION) << "Potion Cost Per Day: " << POTION_COST_PER_HOUR * HOURS_IN_DAY << "\n";

        if (!SIMPLE_DISPLAY) {
            std::cout << std::setprecision(DISPLAY_PRECISION) << "Potion Cost Sim: " << SIM_POTION_COST << "\n";
            std::cout << "\n";
        }

        std::uniform_int_distribution<> dist{values["dungeon"][i]["drops"]["gold"]["gold_min"].asInt(), values["dungeon"][i]["drops"]["gold"]["gold_max"].asInt()};

        for(unsigned int j = 0; j < (SIMULATED_HOURS * keys_per_hour); ++j) {
            ++total_keys_used;

            if (values["player"]["preserve_dungeon_chance"].asFloat() >= drop_roll()) {
                j -= 1;
                total_keys_used -= 1;
                total_preserved_keys += 1;
            }

            counter = 0;
            rolls = 3;

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

                Monster estimate already includes the eff proc
                So all we need to calculate is the rolls/double loot chance
            */
           // Each loot roll has a chance to double (extra roll)
            for (int i = 0; i < 3; i++) {
                if (values["player"]["double_loot_chance"].asFloat() >= drop_roll()) {
                    rolls += 1;
                }
            }
            
            // Roll loot
            while(counter++ < rolls) {
                // Roll for savage and determine which low tier bone we get
                if (values["player"]["savage_bone_drop_chance"].asFloat() >= drop_roll()) {
                    for (int i = 0; i < 2; i++) {
                        if(flip_roll() == 1) {
                            total_bone_one += 1;
                        } else {
                            total_bone_two += 1;
                        }
                    }
                } else {
                    total_bone += 1;
                }

                if (values["dungeon"][i]["drops"]["gold"]["rate"].asFloat() >= drop_roll()) {

                    generated_sim_gold += dist(rng);

                    if (values["player"]["additional_coins_chance"].asFloat() > 0) {
                        if (values["player"]["additional_coins_chance"].asFloat() >= drop_roll()) {
                            generated_sim_gold += dist(rng);
                        }
                    }
                }
                if (values["dungeon"][i]["drops"]["log"]["rate"].asFloat() >= drop_roll()) {
                    total_log += 1;
                }
                if (values["dungeon"][i]["drops"]["ore"]["rate"].asFloat() >= drop_roll()) {
                    total_ore += 1;
                }
                if (values["dungeon"][i]["drops"]["flower"]["rate"].asFloat() >= drop_roll()) {
                    total_flower += 1;
                }
                if (values["dungeon"][i]["drops"]["fish"]["rate"].asFloat() >= drop_roll()) {
                    total_fish += 1;
                }
                if (values["dungeon"][i]["drops"]["gem_one"]["rate"].asFloat() >= drop_roll()) {
                    total_gem_one += 1;
                }
                if (values["dungeon"][i]["drops"]["gem_two"]["rate"].asFloat() >= drop_roll()) {
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

        if (!SIMPLE_DISPLAY) {
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
        }

        long long material_gross = log_value + ore_value + flower_value + fish_value + gem_one_value + gem_two_value + bone_value + bone_one_value + bone_two_value;
        long long material_net = material_gross * 0.95;

        std::cout << "Total Gross value from materials: " << material_gross << "\n";
        std::cout << "Total Net value from materials (5% tax): " << material_net << "\n";

        long long generated_gross = generated_sim_gold + material_gross;
        long long generated_net = generated_sim_gold + material_net;

        std::cout << "\n";

        std::cout << "Gross Gold Generated from Sim: " << generated_gross << "\n";
        std::cout << "Gross Gold Generated from Sim Per Hour: " << generated_gross / SIMULATED_HOURS << "\n";
        std::cout << "Net Gold Generated from Sim: " << generated_net - (keys_cost_per_hour * SIMULATED_HOURS) - sim_food_cost - SIM_POTION_COST << "\n";
        std::cout << "Net Gold Generated from Sim Per Hour: " << (generated_net - (keys_cost_per_hour * SIMULATED_HOURS) - sim_food_cost - SIM_POTION_COST) / SIMULATED_HOURS << "\n";
        std::cout << "Gross Gold Per Key: " << (generated_gross / (keys_per_hour * SIMULATED_HOURS)) - values["key_values"][i]["cost"].asInt() << "\n";
        std::cout << "Net Gold Per Key: " << (generated_net / (keys_per_hour * SIMULATED_HOURS)) - values["key_values"][i]["cost"].asInt() << "\n";

        // Determine optimal key cost for setup
        int op_key_gold = values["key_values"][i]["cost"].asInt();
        long long op_net_gold = 0;
        bool profit_already = false;

        if ((generated_net - ((op_key_gold * keys_per_hour) * SIMULATED_HOURS) - sim_food_cost - SIM_POTION_COST) > 0) {
            profit_already = true;
        }

        while (true) {
            if (op_key_gold == 0) {
                break;
            }

            op_net_gold = (generated_net - ((op_key_gold * keys_per_hour) * SIMULATED_HOURS) - sim_food_cost - SIM_POTION_COST);

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

        std::cout << "Simulated Hours: " << SIMULATED_HOURS << "\n";
        std::cout << "Total Keys used: " << total_keys_used << "\n";
        std::cout << "Total Keys Preserved: " << total_preserved_keys << "\n";
        std::cout << "Loot Cycles Per Hour: " << (total_loot_cycles / SIMULATED_HOURS) << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Loot Cycles Sim: " << total_loot_cycles << "\n";
        std::cout << "Keys Per Hour: " << keys_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Per Day: " << keys_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Sim: " << keys_per_hour * SIMULATED_HOURS << "\n";

        std::cout << "\n";

        std::cout << "Estimated Loot Cycles Per Hour: " << ((keys_per_hour * LOOT_ROLLS_PER_KEY) * (1 + values["player"]["preserve_dungeon_chance"].asFloat())) << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Estimated Loot Cycles Per Day: " << ((keys_per_hour * LOOT_ROLLS_PER_KEY) * (1 + values["player"]["preserve_dungeon_chance"].asFloat())) * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Estimated Loot Cycles Sim: " << ((keys_per_hour * LOOT_ROLLS_PER_KEY) * (1 + values["player"]["preserve_dungeon_chance"].asFloat())) * SIMULATED_HOURS << "\n";

        std::cout << "\n";

        std::cout << "Keys Cost: " << values["key_values"][i]["cost"].asInt() << "\n";
        std::cout << "Keys Cost Per Hour: " << keys_cost_per_hour << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Cost Per Day: " << keys_cost_per_hour * HOURS_IN_DAY << "\n";
        std::cout << std::setprecision(DISPLAY_PRECISION) << "Keys Cost Sim: " << keys_cost_per_hour * SIMULATED_HOURS << "\n";

        std::cout << "\n";

        std::cout << "Using the gold+materials to pay for all costs." << "\n";
        std::cout << "The Optimal Net Zero Key Cost is: " << op_key_gold << "\n";

        if (i == values["dungeon"].size() - 1) {
            std::cout << "---------------------------------------------------------" << "\n";
        }
    }
}
