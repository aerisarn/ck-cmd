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
#include "stdafx.h"

using namespace Niflib;
using namespace std;
using namespace std::experimental::filesystem;

static const path nif_in = "resources\\in\\";
static const path resources = "resources\\";
static const path nif_out = "resources\\out";

void findFiles(path startingDir, string extension, vector<path>& results);
bool BeginConversion();