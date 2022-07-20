#pragma once
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

namespace ckcmd {
    namespace HKX {
        struct RootMovement;
    }
}

class ImportKF : public Command<ImportKF>
{
    REGISTER_COMMAND_HEADER(ImportKF)

private:
    ImportKF();
    virtual ~ImportKF();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

    static void ExportAnimations(const string& rootdir, const fs::path& skelfile
        , const vector<fs::path>& animlist, const string& outdir
        , hkPackFormat pkFormat, const hkPackfileWriter::Options& packFileOptions
        , hkSerializeUtil::SaveOptionBits flags
        , ckcmd::HKX::RootMovement& root_info
        , bool norelativepath = false);

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};