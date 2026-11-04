import os
import sys
import shutil

# Define target files
PLAYER_H = "src/server/game/Entities/Player/Player.h"
SPELL_CPP = "src/server/game/Spells/Auras/SpellAuraEffects.cpp"

# --- Injection Payloads ---
PLAYER_H_ANCHOR = "void InitDisplayIds();"
PLAYER_H_PAYLOAD = """
    // --- DRUID FORMS MODULE ---
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

    void SetObjectScale(float scale) override
    {
        if (getClass() == CLASS_DRUID)
        {
            uint8 formId = GetShapeshiftForm();
            if (formId != 0 && GetCustomDruidForm(formId) != 0)
            {
                scale *= GetCustomDruidFormScale(formId);
            }
        }
        Unit::SetObjectScale(scale);
        SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, scale * DEFAULT_WORLD_OBJECT_SIZE);
        SetFloatValue(UNIT_FIELD_COMBATREACH, scale * DEFAULT_COMBAT_REACH);
    }
    // --------------------------
"""

SPELL_CPP_ANCHOR = "target->SetDisplayId(modelid);"
SPELL_CPP_PAYLOAD = """
                // --- DRUID FORMS MODULE ---
                if (target->IsPlayer() && target->ToPlayer()->getClass() == CLASS_DRUID)
                {
                    uint32 customFormModel = target->ToPlayer()->GetCustomDruidForm(form);
                    if (customFormModel != 0)
                    {
                        modelid = customFormModel;
                    }
                }
                // --------------------------
                
                target->SetDisplayId(modelid);"""

def patch_file(filepath, anchor, payload, check_string):
    if not os.path.exists(filepath):
        print(f"[-] Error: Could not find {filepath}. Run this script from the azerothcore root.")
        return False

    with open(filepath, 'r') as file:
        content = file.read()

    if check_string in content:
        print(f"[~] {filepath} is already patched. Skipping.")
        return True

    if anchor not in content:
        print(f"[-] Error: Could not find the anchor text in {filepath}. The core might have changed.")
        return False

    # Backup the original file to the home directory
    home_dir = os.path.expanduser("~")
    filename = os.path.basename(filepath)
    backup_path = os.path.join(home_dir, f"{filename}.backup")
    
    try:
        shutil.copy2(filepath, backup_path)
        print(f"[*] Backup created at: {backup_path}")
    except Exception as e:
        print(f"[-] Error creating backup for {filepath}: {e}")
        return False

    # Insert the payload directly after the anchor for Player.h, or replace the anchor for SpellAuraEffects.cpp
    if filepath == PLAYER_H:
        patched_content = content.replace(anchor, anchor + "\n" + payload)
    else:
        patched_content = content.replace(anchor, payload)

    with open(filepath, 'w') as file:
        file.write(patched_content)
    
    print(f"[+] Successfully patched {filepath}")
    return True

print("Starting Mod-Druid-Forms Auto-Patcher...")
p1 = patch_file(PLAYER_H, PLAYER_H_ANCHOR, PLAYER_H_PAYLOAD, "CustomDruidForms")
p2 = patch_file(SPELL_CPP, SPELL_CPP_ANCHOR, SPELL_CPP_PAYLOAD, "customFormModel")

if p1 and p2:
    print("\n[SUCCESS] Core files patched successfully! You can now compile the server.")
else:
    print("\n[FAILED] One or more files failed to patch.")
    sys.exit(1)
