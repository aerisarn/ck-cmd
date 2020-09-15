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

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace ckcmd {
	namespace desaturateVC {

		class DesaturateVC : public Command<DesaturateVC>
		{
			REGISTER_COMMAND_HEADER(DesaturateVC)

		private:
			DesaturateVC();
			virtual ~DesaturateVC();

		public:
			virtual string GetName() const;
			virtual string GetHelp() const;
			virtual string GetHelpShort() const;

		protected:
			virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
		};
	}
}
