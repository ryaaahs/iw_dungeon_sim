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

## Example Output:
```
---------------------------------------------------------
Dungeon Level: 85
Food Cost Per Day: 870000
Potion Cost Per Day: 353763.78125

Total Gross value from materials: 1540781
Total Net value from materials (5% tax): 1463741

Gross Gold Generated from Sim: 5310137
Gross Gold Generated from Sim Per Hour: 221255
Net Gold Generated from Sim: 889333.25
Net Gold Generated from Sim Per Hour: 37055.55078125
Gross Gold Per Key: 912
Net Gold Per Key: 880

Simulated Hours: 24
Total Keys used: 2400
Total Keys Preserved: 112
Loot Cycles Per Hour: 424
Loot Cycles Sim: 10199
Keys Per Hour: 100
Keys Per Day: 2400
Keys Sim: 2400

Estimated Loot Cycles Per Hour: 312
Estimated Loot Cycles Per Day: 7488
Estimated Loot Cycles Sim: 7488

Keys Cost: 1300
Keys Cost Per Hour: 130000
Keys Cost Per Day: 3120000
Keys Cost Sim: 3120000

Using the gold+materials to pay for all costs.
The Optimal Net Zero Key Cost is: 1670
---------------------------------------------------------
```