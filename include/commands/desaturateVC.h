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
#include <obj/NiDynamicEffect.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#include <bitset>
#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace ckcmd {
	namespace desaturateVC {

		class DesaturateVC : public CommandBase<DesaturateVC>
		{
			COMMAND_PARAMETERS_LIST
			{
				//COMMAND_PARAMETER(bool, a);
			};

			REGISTER_COMMAND_HEADER(DesaturateVC)

		private:
			DesaturateVC();
			virtual ~DesaturateVC();

		public:
			static string GetName();
			static string GetHelp();
			static string GetHelpShort();

			virtual bool InternalRunCommand(const CommandSettings& settings);
		};
	}
}
