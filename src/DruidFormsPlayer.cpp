#include "ScriptMgr.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "DruidForms.h"

class player_druid_forms : public PlayerScript
{
public:
    // Update 1: explicitly register the PLAYERHOOK_ON_LOGIN event in the constructor
    player_druid_forms() : PlayerScript("player_druid_forms", { PLAYERHOOK_ON_LOGIN }) { }

    // Update 2: Rename OnLogin to OnPlayerLogin to match the modern AC hook signature
    void OnPlayerLogin(Player* player) override
    {
        player->CustomDruidForms.clear();

        if (!DruidFormsEnabled || player->getClass() != CLASS_DRUID) 
            return;

        QueryResult result = CharacterDatabase.Query("SELECT form_id, model_id FROM character_druid_forms WHERE guid = {}", player->GetGUID().GetCounter());
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint8 formId = fields[0].GetUInt8();
                uint32 modelId = fields[1].GetUInt32();
                
                player->SetCustomDruidForm(formId, modelId);
            } while (result->NextRow());
        }
    }
};

void AddSC_player_druid_forms()
{
    new player_druid_forms();
}
