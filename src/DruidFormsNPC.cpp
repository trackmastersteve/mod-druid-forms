#include "ScriptMgr.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "DatabaseEnv.h"
#include "DruidForms.h"

class npc_druid_forms : public CreatureScript
{
public:
    npc_druid_forms() : CreatureScript("npc_druid_forms") { }

    std::string GetFormName(uint8 formId)
    {
        switch(formId) {
            case 1: return "Cat Form";
            case 5: return "Bear Form";
            case 8: return "Dire Bear Form";
            case 31: return "Moonkin Form";
            case 2: return "Tree of Life Form";
            case 3: return "Travel Form";
            case 4: return "Aquatic Form";
            case 27: return "Flight Form";
            case 29: return "Swift Flight Form";
            default: return "Unknown Form";
        }
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!DruidFormsEnabled || player->getClass() != CLASS_DRUID)
        {
            player->SendSystemMessage("This feature is currently unavailable.");
            return false;
        }

        for (auto const& pair : DruidFormModels)
        {
            uint8 formId = pair.first;
            if (!pair.second.empty())
            {
                AddGossipItemFor(player, GOSSIP_SENDER_MAIN, "Customize " + GetFormName(formId), formId, 0);
            }
        }
        
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (!DruidFormsEnabled)
        {
            CloseGossipMenuFor(player);
            return true;
        }

        if (action == 0)
        {
            uint8 formId = sender;
            AddGossipItemFor(player, GOSSIP_SENDER_MAIN, "<- Back to Main Menu", 0, 0); 
            AddGossipItemFor(player, GOSSIP_SENDER_MAIN, "Reset to Default Native Skin", formId, 999999);

            if (DruidFormModels.find(formId) != DruidFormModels.end())
            {
                // Iterate through the new map structure
                for (auto const& modelPair : DruidFormModels[formId])
                {
                    uint32 modelId = modelPair.first;
                    float scale = modelPair.second;
                    
                    // Show the scale in the menu if it's customized
                    std::string label = "Use Model ID: " + std::to_string(modelId);
                    if (scale != 1.0f)
                    {
                        // Format the float to 1 decimal place for a cleaner UI
                        char scaleStr[16];
                        snprintf(scaleStr, sizeof(scaleStr), " (Scale: %.1f)", scale);
                        label += scaleStr;
                    }

                    AddGossipItemFor(player, GOSSIP_SENDER_MAIN, label, formId, modelId);
                }
            }
            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        if (sender == 0 && action == 0)
        {
            OnGossipHello(player, creature);
            return true;
        }

        uint8 formId = sender;
        uint32 modelId = (action == 999999) ? 0 : action;

        if (modelId == 0)
            CharacterDatabase.Execute("DELETE FROM character_druid_forms WHERE guid = {} AND form_id = {}", player->GetGUID().GetCounter(), formId);
        else
            CharacterDatabase.Execute("REPLACE INTO character_druid_forms (guid, form_id, model_id) VALUES ({}, {}, {})", player->GetGUID().GetCounter(), formId, modelId);

        player->SetCustomDruidForm(formId, modelId);

        if (player->GetShapeshiftForm() == formId)
        {
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
            player->SendSystemMessage("Form updated! Shapeshift again to see the changes.");
        }
        else 
        {
            player->SendSystemMessage("Form appearance updated!");
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

void AddSC_npc_druid_forms()
{
    new npc_druid_forms();
}
