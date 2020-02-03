// Command Base
#include <commands/CommandBase.h>

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
#pragma once

#include <obj/NiDynamicEffect.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#include <bitset>
#include <filesystem>

#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>
#include <core/MathHelper.h>
#include <commands/Geometry.h>

#include <unordered_map>

namespace fs = std::experimental::filesystem;

Niflib::NiTriShapeRef remake_partitions(Niflib::NiTriBasedGeomRef iShape, int & maxBonesPerPartition, int & maxBonesPerVertex, bool make_strips, bool pad);


namespace ckcmd {
	namespace fixsse {

		class FixSSENif : public CommandBase<FixSSENif>
		{
			COMMAND_PARAMETERS_LIST
			{
				//COMMAND_PARAMETER(bool, a);
			};

			REGISTER_COMMAND_HEADER(FixSSENif)

		private:
			FixSSENif();
			virtual ~FixSSENif();

		public:
			static string GetName();
			static string GetHelp();
			static string GetHelpShort();

			virtual bool InternalRunCommand(const CommandSettings& settings);
		};
	}
}
