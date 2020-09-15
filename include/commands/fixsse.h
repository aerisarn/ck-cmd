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

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace ckcmd {
	namespace fixsse {

		class FixSSENif : public Command<FixSSENif>
		{
			REGISTER_COMMAND_HEADER(FixSSENif)

		private:
			FixSSENif();
			virtual ~FixSSENif();

		public:
			virtual string GetName() const;
			virtual string GetHelp() const;
			virtual string GetHelpShort() const;

		protected:
			virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
		};
	}
}
