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
Because this module requires modifying the core AzerothCore source code, it **is not** a pure plug-and-play module. You must make two small edits to your core files before compiling.

---

## Installation Guide

### Step 1: Core Modifications
You need to add a memory map to the `Player` class and a hook into the shapeshift aura effect.

**1. Edit `src/server/game/Entities/Player/Player.h`**
Find `void InitDisplayIds();` inside the `public:` block of `class Player` and add the following right below it:
```cpp
    // Custom Druid Forms
    std::map<uint8, uint32> CustomDruidForms;
    std::map<uint8, float> CustomDruidFormScales;
    
    uint32 GetCustomDruidForm(uint8 formId) const
    {
        auto itr = CustomDruidForms.find(formId);
        if (itr != CustomDruidForms.end())
            return itr->second;
        return 0; 
    }

    void SetCustomDruidForm(uint8 formId, uint32 modelId)
    {
        if (modelId == 0)
            CustomDruidForms.erase(formId);
        else
            CustomDruidForms[formId] = modelId;
    }

    float GetCustomDruidFormScale(uint8 formId) const
    {
        auto itr = CustomDruidFormScales.find(formId);
        if (itr != CustomDruidFormScales.end())
            return itr->second;
        return 1.0f; 
    }

    void SetCustomDruidFormScale(uint8 formId, float scale)
    {
        if (scale == 1.0f)
            CustomDruidFormScales.erase(formId);
        else
            CustomDruidFormScales[formId] = scale;
    }

    // Intercept the core scale calculation
    void SetObjectScale(float scale) override
    {
        // --- DRUID FORMS INJECTION ---
        if (getClass() == CLASS_DRUID)
        {
            uint8 formId = GetShapeshiftForm();
            if (formId != 0 && GetCustomDruidForm(formId) != 0)
            {
                scale *= GetCustomDruidFormScale(formId);
            }
        }
        // -----------------------------

        Unit::SetObjectScale(scale);
        SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, scale * DEFAULT_WORLD_OBJECT_SIZE);
        SetFloatValue(UNIT_FIELD_COMBATREACH, scale * DEFAULT_COMBAT_REACH);
    }
```

**2. Edit `src/server/game/Spells/Auras/SpellAuraEffects.cpp`**
Find `void AuraEffect::HandleAuraModShapeshift`. Inside that function, locate the `if (allow)` block. Inject the custom check right before `target->SetDisplayId(modelid);`:
```cpp
            if (allow)
            {
                // ------- DRUID-FORMS INJECTION ---
                if (target->IsPlayer() && target->ToPlayer()->getClass() == CLASS_DRUID)
                {
                    uint32 customFormModel = target->ToPlayer()->GetCustomDruidForm(form);
                    if (customFormModel != 0)
                    {
                        modelid = customFormModel; // Override the native calculation
                    }
                }
                // ---------------------------------

                target->SetDisplayId(modelid);
            }
```

### Step 2: Module Setup
1. Clone or copy the `mod-druid-forms` directory into your AzerothCore `modules/` folder.
2. Re-run CMake to generate the new build files and register the module.
3. Compile the server.

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
