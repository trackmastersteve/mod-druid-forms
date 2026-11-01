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
            player->GetSession()->SendNotification("This feature is currently unavailable.");
            return false;
        }

        // Generate Main Menu based on populated config options
        for (auto const& pair : DruidFormModels)
        {
            uint8 formId = pair.first;
            if (!pair.second.empty())
            {
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT, "Customize " + GetFormName(formId), formId, 0);
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

        // Sub-Menu Navigation: If action == 0, the player clicked a form in the main menu.
        if (action == 0)
        {
            uint8 formId = sender;
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "<- Back to Main Menu", 0, 0); 
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT, "Reset to Default Native Skin", formId, 999999);

            if (DruidFormModels.find(formId) != DruidFormModels.end())
            {
                for (uint32 modelId : DruidFormModels[formId])
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT, "Use Model ID: " + std::to_string(modelId), formId, modelId);
                }
            }
            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        // Back Button selected
        if (sender == 0 && action == 0)
        {
            OnGossipHello(player, creature);
            return true;
        }

        // Model selected: Save and apply
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
            player->GetSession()->SendNotification("Form updated! Shapeshift again to see the changes.");
        }
        else 
        {
            player->GetSession()->SendNotification("Form appearance updated!");
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

void AddSC_npc_druid_forms()
{
    new npc_druid_forms();
}
