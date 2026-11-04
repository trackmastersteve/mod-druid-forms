import os
import sys
import shutil
import re

PLAYER_H = "src/server/game/Entities/Player/Player.h"
SPELL_CPP = "src/server/game/Spells/Auras/SpellAuraEffects.cpp"

# --- Payload 1: Variables (Injected at InitDisplayIds) ---
PLAYER_H_ANCHOR_1 = "void InitDisplayIds();"
PLAYER_H_PAYLOAD_1 = """
    // --- DRUID FORMS MODULE ---
    std::map<uint8, uint32> CustomDruidForms;
    std::map<uint8, float> CustomDruidFormScales;
    
    uint32 GetCustomDruidForm(uint8 formId) const
    {
        auto itr = CustomDruidForms.find(formId);
        if (itr != CustomDruidForms.end()) return itr->second;
        return 0; 
    }

    void SetCustomDruidForm(uint8 formId, uint32 modelId)
    {
        if (modelId == 0) CustomDruidForms.erase(formId);
        else CustomDruidForms[formId] = modelId;
    }

    float GetCustomDruidFormScale(uint8 formId) const
    {
        auto itr = CustomDruidFormScales.find(formId);
        if (itr != CustomDruidFormScales.end()) return itr->second;
        return 1.0f; 
    }

    void SetCustomDruidFormScale(uint8 formId, float scale)
    {
        if (scale == 1.0f) CustomDruidFormScales.erase(formId);
        else CustomDruidFormScales[formId] = scale;
    }
    // --------------------------
"""

# --- Payload 2: Math (Injected inside the native SetObjectScale) ---
PLAYER_H_ANCHOR_2 = r"(void SetObjectScale\(float scale\) override\s*\{)"
PLAYER_H_PAYLOAD_2 = r"""\1
        // --- DRUID FORMS INJECTION ---
        if (getClass() == CLASS_DRUID)
        {
            uint8 formId = GetShapeshiftForm();
            if (formId != 0 && GetCustomDruidForm(formId) != 0)
            {
                scale *= GetCustomDruidFormScale(formId);
            }
        }
        // -----------------------------"""

# --- Payload 3: Spell Aura Intercept ---
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

def patch_file(filepath):
    if not os.path.exists(filepath):
        print(f"[-] Error: Could not find {filepath}")
        return False

    with open(filepath, 'r') as file:
        content = file.read()

    if "CustomDruidForms" in content and filepath == PLAYER_H:
        print(f"[~] {filepath} is already patched. Skipping.")
        return True

    # Backup creation
    home_dir = os.path.expanduser("~")
    backup_path = os.path.join(home_dir, f"{os.path.basename(filepath)}.backup")
    try:
        shutil.copy2(filepath, backup_path)
    except Exception as e:
        print(f"[-] Backup failed: {e}")
        return False

    if filepath == PLAYER_H:
        if PLAYER_H_ANCHOR_1 not in content:
            print("[-] Anchor 1 not found in Player.h")
            return False
        # Inject variables
        content = content.replace(PLAYER_H_ANCHOR_1, PLAYER_H_ANCHOR_1 + "\n" + PLAYER_H_PAYLOAD_1)
        
        # Inject inside existing function using regex
        if not re.search(PLAYER_H_ANCHOR_2, content):
            print("[-] Native SetObjectScale not found in Player.h")
            return False
        content = re.sub(PLAYER_H_ANCHOR_2, PLAYER_H_PAYLOAD_2, content)

    elif filepath == SPELL_CPP:
        if SPELL_CPP_ANCHOR not in content:
            print("[-] Anchor not found in SpellAuraEffects.cpp")
            return False
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
