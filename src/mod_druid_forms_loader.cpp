// Forward declare the AddSC functions from our other CPP files
void AddSC_DruidFormsConfig();
void AddSC_npc_druid_forms();

// The main loader function called by the AzerothCore engine
void Addmod_druid_formsScripts()
{
    AddSC_DruidFormsConfig();
    AddSC_npc_druid_forms();
}
