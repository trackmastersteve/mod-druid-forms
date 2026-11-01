#include "ScriptMgr.h"
#include "Config.h"
#include "DruidForms.h"
#include <sstream>
#include <algorithm>
#include <cctype>

// Global definitions for the module
bool DruidFormsEnabled = true;
std::map<uint8, std::map<uint32, float>> DruidFormModels;

class DruidForms_WorldScript : public WorldScript
{
public:
    DruidForms_WorldScript() : WorldScript("DruidForms_WorldScript") { }

    void ParseModelConfig(uint8 formId, const std::string& configString)
    {
        std::istringstream ss(configString);
        std::string token;

        while (std::getline(ss, token, ',')) 
        {
            // Remove any spaces from the token
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token.empty()) continue;

            uint32 modelId = 0;
            float scale = 1.0f; // Default scale

            size_t colonPos = token.find(':');
            if (colonPos != std::string::npos) 
            {
                modelId = std::stoul(token.substr(0, colonPos));
                scale = std::stof(token.substr(colonPos + 1));
            } 
            else 
            {
                modelId = std::stoul(token);
            }

            DruidFormModels[formId][modelId] = scale;
        }
    }

    void OnConfigLoad(bool /*reload*/) override
    {
        DruidFormModels.clear();

        DruidFormsEnabled = sConfigMgr->GetOption<bool>("DruidForms.Enable", true);

        if (!DruidFormsEnabled)
            return;

        ParseModelConfig(1, sConfigMgr->GetOption<std::string>("DruidForms.Models.Cat", ""));
        ParseModelConfig(5, sConfigMgr->GetOption<std::string>("DruidForms.Models.Bear", ""));
        ParseModelConfig(8, sConfigMgr->GetOption<std::string>("DruidForms.Models.DireBear", ""));
        ParseModelConfig(31, sConfigMgr->GetOption<std::string>("DruidForms.Models.Moonkin", ""));
        ParseModelConfig(2, sConfigMgr->GetOption<std::string>("DruidForms.Models.Tree", ""));
        ParseModelConfig(3, sConfigMgr->GetOption<std::string>("DruidForms.Models.Travel", ""));
        ParseModelConfig(4, sConfigMgr->GetOption<std::string>("DruidForms.Models.Aquatic", ""));
        ParseModelConfig(27, sConfigMgr->GetOption<std::string>("DruidForms.Models.Flight", ""));
        ParseModelConfig(29, sConfigMgr->GetOption<std::string>("DruidForms.Models.SwiftFlight", ""));
    }
};

void AddSC_DruidFormsConfig()
{
    new DruidForms_WorldScript();
}
