#ifndef _DRUIDFORMS_H
#define _DRUIDFORMS_H

#include "Common.h"
#include <map>

// Global definitions for the module
extern bool DruidFormsEnabled;

// Map structure: Form ID -> (Model ID -> Scale)
extern std::map<uint8, std::map<uint32, float>> DruidFormModels;

#endif
