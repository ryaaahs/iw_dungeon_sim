# iw_dungeon_sim

# About
CPP CLI application to simulate the loot drops for IWRPG Dungeons

## Usage:  
Create your own copy of `values_template.json` and rename to `values.json`  
Update the values within the `values.json` file  
Values that need to be updated:  
    -config  
    -player  
    -dungeon `monsters_hour` and `food_hour` for each entry    
    -key_values `cost` for each entry    
    -log `value` for each entry  
    -ore `value` for each entry  
    -flower `value` for each entry  
    -fish `value` for each entry  
    -gemstone `value` for each entry  
    -bone `value` for each entry  

Once the json is updated, you can compile the source using any cpp compiler of your choice.

## Example:
```
---------------------------------------------------------
Dungeon Level: 100
Food Cost Per Day: 1350720
Potion Cost Per Hour: 14881.8896484375

Total Gross value from materials: 1117966
Total Net value from materials (5% tax): 1062067

Gross Gold Generated from Sim: 4995301
Gross Gold Generated from Sim Per Hour: 208137
Net Gold Generated from Sim: 994212.625
Net Gold Generated from Sim Per Hour: 41425.52734375
Gross Gold Per Key: 2851.19921875
Net Gold Per Key: 2819.29345703125

Simulated Hours: 24
Total Keys used: 1819
Loot Cycles Per Hour: 311
Loot Cycles Sim: 7473
Keys Per Hour: 73
Keys Per Day: 1752
Keys Sim: 1752

Estimated Loot Cycles Per Hour: 227.759994506836
Estimated Loot Cycles Per Day: 5466.23974609375
Estimated Loot Cycles Sim: 5466.23974609375

Keys Cost: 1277
Keys Cost Per Hour: 93221
Keys Cost Per Day: 2237304
Keys Cost Sim: 2237304

Using the gold to pay for all costs.
The Optimal Net Zero Key Cost is: 1844
---------------------------------------------------------
```