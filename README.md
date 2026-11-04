# Mod Druid Forms (AzerothCore)

An AzerothCore module that allows Druid players to customize the appearance of their shapeshift forms (Cat, Bear, Moonkin, Tree, Travel, Aquatic, and Flight) via an in-game NPC. 

Unlike standard morph modules that rely on aura hooks (which cause a visual "flicker" when shifting or zoning), this module uses a **hybrid approach**. It pairs a standard AzerothCore module with a tiny, surgical core modification to intercept the server's native form calculations. This ensures your custom forms load flawlessly and instantly, every single time.

## Prerequisites
* **AzerothCore (WotLK 3.3.5a)**
* **MPQ Editor:** Required if you plan to pack custom client-side DBC files or custom model textures beyond the vanilla assets.

## Features
* **NPC Form Customizer:** Adds a custom NPC (Keeper Remulos, Entry: `99999`) who offers a dynamic menu of form skins.
* **Dynamic Scaling:** Intercepts the core engine's `SetObjectScale` function to cleanly apply custom scaling without permanent shrinking bugs.
* **Configurable:** Easily add or remove model IDs and scales for any form via the `mod_druid_forms.conf` file without needing to recompile the server.
* **Flicker-Free:** Natively injects into `SpellAuraEffects.cpp` so custom models are applied exactly like native ones.
* **Global Toggle:** Can be completely disabled via the config file.

## Important Note
Because this module requires a hybrid approach, it **is not** a pure plug-and-play module and requires slight modifications to the core AzerothCore source code. We have provided an automated Python script to safely apply these edits for you before compiling.

---

## Installation Guide

### Step 1: Download & Apply Core Modifications
Instead of manually editing C++ files, you can use the included installer script to safely patch the core engine.

1. Clone or copy the `mod-druid-forms` directory into your AzerothCore `modules/` folder.
2. Open a terminal in your **AzerothCore root directory** and run the auto-patcher script:
   ```bash
   python3 modules/mod-druid-forms/install_core_mods.py
   ```
   *(Note: The script will automatically create backups of the original `Player.h` and `SpellAuraEffects.cpp` files in your home directory before applying the patches).*

### Step 2: Compile the Server
1. Re-run CMake to generate the new build files and register the module.
2. Compile your server as you normally would.

### Step 3: Database Import
Because the SQL files are located in the `data/sql/` directory, the AzerothCore database auto-assembler will automatically detect and import the tables into your `acore_world` and `acore_characters` databases the next time you build and start the server. No manual SQL execution is required!

### Step 4: Configuration
1. Navigate to `modules/mod-druid-forms/conf/`.
2. Copy `mod_druid_forms.conf.dist` and rename it to `mod_druid_forms.conf`.
3. Open the file and adjust the comma-separated `Model IDs` for each form to your liking. *Note: You must ensure the model IDs you use are valid for your client (or have been added via custom DBC patches).*
4. **Applying Scales:** To apply a custom scale to a specific model, append `:Scale` directly to the ID (e.g., `17772:0.4`). If no scale is provided, it defaults to 1.0.
5. **Flight Forms:** Both regular Flight Form and Swift Flight Form use the same internal Form ID (27) in the core engine. Group all of your bird models under the regular `DruidForms.Models.Flight` list in the config file.

---

## Usage
Once the server is compiled and running:
1. Log into the game with a GM account.
2. Type `.npc add 99999` to spawn the Form Customizer NPC.
3. Interact with the NPC on a Druid character to select your new forms!
