// Command Base
#ifndef NIFSCAN_CMD
#define NIFSCAN_CMD
#include <commands/Geometry.h>
#include <commands/CommandBase.h>
#include <filesystem>

namespace fs = std::experimental::filesystem;

static const fs::path nif_scan_in = ".\\resources\\in";
static const fs::path nif_scan_err = "D:\\git\\ck-cmd\\resources\\err";

namespace ckcmd {
	namespace nifscan {

        class NifScan : public CommandBase
        {
            REGISTER_COMMAND_HEADER(NifScan)

        private:
            NifScan();
            virtual ~NifScan();

        public:
            virtual string GetName() const;
            virtual string GetHelp() const;
            virtual string GetHelpShort() const;

        protected:
            virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
        };

		using namespace Niflib;
	}
}
#endif