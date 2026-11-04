import os
import sys
import shutil
import re

PLAYER_H = "src/server/game/Entities/Player/Player.h"
SPELL_CPP = "src/server/game/Spells/Auras/SpellAuraEffects.cpp"

# --- Payload 1: Math (Injected inside the native SetObjectScale) ---
PLAYER_H_PAYLOAD_1 = """void SetObjectScale(float scale) override
    {
        // --- DRUID FORMS INJECTION ---
        if (getClass() == CLASS_DRUID)
        {
            uint8 formId = GetShapeshiftForm();
            // If they are in a form, and that form has a custom model applied
            if (formId != 0 && GetCustomDruidForm(formId) != 0)
            {
                // Multiply the server's incoming scale by our config scale
                scale *= GetCustomDruidFormScale(formId);
            }
        }
        // -----------------------------"""

# --- Payload 2: Variables (Injected below InitDisplayIds) ---
PLAYER_H_ANCHOR_2 = "void InitDisplayIds();"
PLAYER_H_PAYLOAD_2 = """void InitDisplayIds();

    // Custom Druid Forms
    std::map<uint8, uint32> CustomDruidForms;
    std::map<uint8, float> CustomDruidFormScales;

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

    float GetCustomDruidFormScale(uint8 formId) const
    {
        auto itr = CustomDruidFormScales.find(formId);
        if (itr != CustomDruidFormScales.end())
            return itr->second;
        return 1.0f; // 1.0f means default scale
    }

    void SetCustomDruidFormScale(uint8 formId, float scale)
    {
        // If it's normal size, erase it to save memory. Otherwise, save the scale.
        if (scale == 1.0f)
            CustomDruidFormScales.erase(formId);
        else
            CustomDruidFormScales[formId] = scale;
    }"""

# --- Payload 3: Spell Aura Intercept ---
SPELL_CPP_ANCHOR = "target->SetDisplayId(modelid);"
SPELL_CPP_PAYLOAD = """// ---- MORPH FEATURE INJECTION ----
                if (target->IsPlayer() && target->ToPlayer()->getClass() == CLASS_DRUID)
                {
                    uint32 customFormModel = target->ToPlayer()->GetCustomDruidForm(form);
                    if (customFormModel != 0)
                    {
                        modelid = customFormModel;
                    }
                }
                // ---------------------------------

                target->SetDisplayId(modelid);"""

def patch_file(filepath):
    if not os.path.exists(filepath):
        print(f"[-] Error: Could not find {filepath}")
        return False

    with open(filepath, 'r') as file:
        content = file.read()

    # Prevent double-patching
    if "CustomDruidForms" in content and filepath == PLAYER_H:
        print(f"[~] {filepath} is already patched. Skipping.")
        return True

    # Create backup in the home directory
    home_dir = os.path.expanduser("~")
    backup_path = os.path.join(home_dir, f"{os.path.basename(filepath)}.backup")
    try:
        shutil.copy2(filepath, backup_path)
        print(f"[*] Backup created: {backup_path}")
    except Exception as e:
        print(f"[-] Backup failed: {e}")
        return False

    if filepath == PLAYER_H:
        if PLAYER_H_ANCHOR_2 not in content:
            print("[-] Anchor not found in Player.h")
            return False
            
        # 1. Inject variables at InitDisplayIds
        content = content.replace(PLAYER_H_ANCHOR_2, PLAYER_H_PAYLOAD_2)
        
        # 2. Inject math inside the native SetObjectScale
        content = re.sub(r"void SetObjectScale\(float scale\) override\s*\{", PLAYER_H_PAYLOAD_1, content)

    elif filepath == SPELL_CPP:
        if SPELL_CPP_ANCHOR not in content:
            print("[-] Anchor not found in SpellAuraEffects.cpp")
            return False
            
        # 3. Inject Morph Feature before SetDisplayId
        content = content.replace(SPELL_CPP_ANCHOR, SPELL_CPP_PAYLOAD)

    with open(filepath, 'w') as file:
        file.write(content)
    
    print(f"[+] Patched {filepath}")
    return True

print("Starting Mod-Druid-Forms Auto-Patcher...")
if patch_file(PLAYER_H) and patch_file(SPELL_CPP):
    print("\n[SUCCESS] Core patched correctly.")
else:
    sys.exit(1)
