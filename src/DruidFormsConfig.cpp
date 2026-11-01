#include "ScriptMgr.h"
#include "Config.h"
#include "DruidForms.h"
#include <sstream>
#include <algorithm>

bool DruidFormsEnabled = true;
std::map<uint8, std::vector<uint32>> DruidFormModels;

class mod_druid_forms_world : public WorldScript
{
public:
    mod_druid_forms_world() : WorldScript("mod_druid_forms_world") { }

    void LoadModelsForForm(uint8 formId, const char* configKey)
    {
        std::string configString = sConfigMgr->GetOption<std::string>(configKey, "");
        if (configString.empty()) return;

        std::stringstream ss(configString);
        std::string token;
        
        while (std::getline(ss, token, ','))
        {
            // Remove whitespace
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token.empty()) continue;
            
            try 
            {
                uint32 modelId = std::stoul(token);
                DruidFormModels[formId].push_back(modelId);
            } 
            catch (...) { /* Ignore invalid non-numeric config entries */ }
        }
    }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        DruidFormModels.clear();
        DruidFormsEnabled = sConfigMgr->GetOption<bool>("DruidForms.Enable", true);

        if (!DruidFormsEnabled) return;

        LoadModelsForForm(1, "DruidForms.Models.Cat");         // Cat Form
        LoadModelsForForm(5, "DruidForms.Models.Bear");        // Bear Form
        LoadModelsForForm(8, "DruidForms.Models.Bear");        // Dire Bear Form (Shares Bear config)
        LoadModelsForForm(31, "DruidForms.Models.Moonkin");    // Moonkin Form
        LoadModelsForForm(2, "DruidForms.Models.Tree");        // Tree of Life Form
        LoadModelsForForm(3, "DruidForms.Models.Travel");      // Travel Form
        LoadModelsForForm(4, "DruidForms.Models.Aquatic");     // Aquatic Form
        LoadModelsForForm(27, "DruidForms.Models.Flight");     // Flight Form
        LoadModelsForForm(29, "DruidForms.Models.Flight");     // Swift Flight Form (Shares Flight config)
    }
};

void AddSC_mod_druid_forms_world()
{
    new mod_druid_forms_world();
}
