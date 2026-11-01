# Mod Druid Forms (AzerothCore)

An AzerothCore module that allows Druid players to customize the appearance of their shapeshift forms (Cat, Bear, Moonkin, Tree, Travel, Aquatic, and Flight) via an in-game NPC. 

Unlike standard morph modules that rely on aura hooks (which cause a visual "flicker" when shifting or zoning), this module uses a **hybrid approach**. It pairs a standard AzerothCore module with a tiny, surgical core modification to intercept the server's native form calculations. This ensures your custom forms load flawlessly and instantly, every single time.

## Features
* **NPC Form Customizer:** Adds a custom NPC (Broll Bearmantle, Entry: `99999`) who offers a dynamic menu of form skins.
* **Configurable:** Easily add or remove model IDs for any form via the `mod_druid_forms.conf` file without needing to recompile the server.
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
    
    uint32 GetCustomDruidForm(uint8 formId) const
    {
        auto itr = CustomDruidForms.find(formId);
        if (itr != CustomDruidForms.end())
            return itr->second;
        return 0; // 0 means default form
    }

    void SetCustomDruidForm(uint8 formId, uint32 modelId)
    {
        if (modelId == 0)
            CustomDruidForms.erase(formId);
        else
            CustomDruidForms[formId] = modelId;
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
Navigate to `modules/mod-druid-forms/sql/`.
1. Execute `characters/base/character_druid_forms.sql` on your **Characters** database.
2. Execute `world/base/creature_template_druid_npc.sql` on your **World** database.

### Step 4: Configuration
1. Navigate to `modules/mod-druid-forms/conf/`.
2. Copy `mod_druid_forms.conf.dist` and rename it to `mod_druid_forms.conf`.
3. Open the file and adjust the comma-separated `Model IDs` for each form to your liking. *Note: You must ensure the model IDs you use are valid for your client (or have been added via custom DBC patches).*

---

## Usage
Once the server is compiled and running:
1. Log into the game with a GM account.
2. Type `.npc add 99999` to spawn the Form Customizer NPC.
3. Interact with the NPC on a Druid character to select your new forms!