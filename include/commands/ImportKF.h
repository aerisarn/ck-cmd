#pragma once
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

namespace ckcmd {
    namespace HKX {
        struct RootMovement;
    }
}

#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Niflib {
    template<class T> class Ref;
    class NiObject;
    class NiNode;
}


typedef std::vector<Niflib::Ref<Niflib::NiObject>> NifFileType;
typedef std::vector<Niflib::Ref<Niflib::NiObject>> KFFileType;

//skeleton blocks, additional meshes blocks, keyframes blocks
typedef std::tuple<NifFileType, std::map<fs::path, NifFileType>, std::map<fs::path, KFFileType>> NifFolderType;

template <class TYPE>
class hkRefPtr;
class hkTransform;
class hkaSkeleton;

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

    static void ExportAnimations(const NifFolderType& in, const hkRefPtr<hkaSkeleton>& skeleton
        , const string& outdir
        , std::map<fs::path, ckcmd::HKX::RootMovement>& rootMovements
        , const set<Niflib::Ref<Niflib::NiNode>>& other_bones_in_accum
        , const hkTransform& pelvis_local
    );

    static void ExportAnimations(const string& rootdir, const fs::path& skelfile
        , const vector<fs::path>& animlist, const string& outdir
        , hkPackFormat pkFormat, const hkPackfileWriter::Options& packFileOptions
        , hkSerializeUtil::SaveOptionBits flags
        , ckcmd::HKX::RootMovement& root_info
        , bool norelativepath = false);

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};