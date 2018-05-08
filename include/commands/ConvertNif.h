#pragma once
#include <filesystem>

#include <niflib.h>
#include <obj\NiObject.h>
#include <obj\BSFadeNode.h>

#include <objDecl.cpp>
#include <field_visitor.h>
#include <interfaces\typed_visitor.h>

//hierarchy
#include <obj/NiTimeController.h>
#include <obj/NiExtraData.h>
#include <obj/NiCollisionObject.h>
#include <obj/NiProperty.h>
#include <obj/NiDynamicEffect.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#include <filesystem>

namespace fs = std::experimental::filesystem;

static const fs::path nif_in = "resources\\in\\";
static const fs::path resources = "resources\\";
static const fs::path nif_out = "D:\\git\\ck-cmd\\resources\\out";

//void findFiles(fs::path startingDir, string extension, vector<fs::path>& results);
//bool BeginConversion();