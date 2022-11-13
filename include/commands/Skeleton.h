#ifndef EXPORTRIG_H
#define EXPORTRIG_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>
#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Niflib {
	template<class T> class Ref;
	class NiObject;
}


typedef std::vector<Niflib::Ref<Niflib::NiObject>> NifFileType;
typedef std::vector<Niflib::Ref<Niflib::NiObject>> KFFileType;

//skeleton blocks, additional meshes blocks, keyframes blocks
typedef std::tuple<NifFileType, std::map<fs::path, NifFileType>, std::map<fs::path, KFFileType>> NifFolderType;


template <class TYPE>
class hkRefPtr;

class hkaSkeleton;

class Skeleton : public Command<Skeleton>
{
	REGISTER_COMMAND_HEADER(Skeleton)

private:
	Skeleton();
	virtual ~Skeleton();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

	static bool Convert(const NifFolderType& in, const string& outpath, hkRefPtr<hkaSkeleton>& converted_skeleton, std::map<fs::path, std::string>& body_parts);
	static bool Convert(const string& inpath, const string& animations_path, const string& outpath);

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //EXPORTRIG_H