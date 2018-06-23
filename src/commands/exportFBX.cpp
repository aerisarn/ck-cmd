#include "stdafx.h"

#include <commands/ExportFBX.h>

#include <commands/Geometry.h>
#include <core/FBXWrangler.h>

#include <core/hkxcmd.h>
#include <core/log.h>
#include <core/games.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>

using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;


//void WriteFBX(NifFile* nif, const fs::path& out) {
//	fs::path out_path = / out;
//	fs::create_directories(out_path.parent_path());
//	FBXWrangler().ExportScene(out_path.string().c_str());
//}

struct AnimationCache {
    AnimData::AnimDataFile animationData;
    AnimData::AnimSetDataFile animationSetData;

    AnimationCache(const string&  animationDataContent, const string&  animationSetDataContent) {
        animationData.parse(animationDataContent);
        animationSetData.parse(animationSetDataContent);
    }

    void printInfo() {
        Log::Info("Parsed correctly %d havok projects", animationData.getProjectList().getStrings().size());
        Log::Info("Found %d creatures:", animationSetData.getProjectsList().getStrings().size());
        set<string> creatures;
        int index = 0;
        for (string creature : animationSetData.getProjectsList().getStrings()) {
            string creature_project_name = fs::path(creature).filename().replace_extension("").string();
            if (creature_project_name.find("Project") != string::npos)
                creature_project_name = creature_project_name.substr(0, creature_project_name.find("Project"));
            Log::Info("\t%d: %s", index++, creature_project_name.c_str());
            creatures.insert(creature_project_name);
        }
        index = 0;
        int misc_index = 0;
        Log::Info("Found %d misc projects:", animationData.getProjectList().getStrings().size() - animationSetData.getProjectsList().getStrings().size());
        for (string misc : animationData.getProjectList().getStrings()) {
            string misc_project_name = fs::path(misc).filename().replace_extension("").string();
            if (!animationData.getProjectBlock(index).getHasAnimationCache()) {
                Log::Info("\t%d: %s", misc_index++, misc_project_name.c_str());
            }
            index++;
            //if (misc_project_name.find("Project") != string::npos)
            //	misc_project_name = misc_project_name.substr(0, misc_project_name.find("Project"));
            //bool is_misc = true;
            //for (string creature : creatures)
            //	if (misc_project_name.find(creature) != string::npos)
            //		is_misc = false;
            //if(is_misc)

        }
    }
};

void loadFileIntoString(const fs::path& path, string& content) {
    std::ifstream fss(path.c_str());
    content.clear();
    //allocate
    fss.seekg(0, std::ios::end);
    content.reserve(fss.tellg());
    //reset and assign
    fss.seekg(0, std::ios::beg);
    content.assign((std::istreambuf_iterator<char>(fss)),
        std::istreambuf_iterator<char>());
}

void loadOverrideOrBSA(const string& path, string& content, const Games::Game& game, const vector<string>& preferredBsas) {
    //search in override
    Games& games = Games::Instance();
    fs::path override_path = games.data(game) / path;
    if (fs::exists(override_path) && fs::is_regular_file(override_path))
        loadFileIntoString(override_path, content);
    else {
        for (string bsa_name : preferredBsas) {
            BSAFile bsa_file(games.data(game) / bsa_name);
            if (bsa_file.find(path)) {
                size_t size = -1;
                const uint8_t* data = bsa_file.extract(path, size);
                content.assign((char*)data, size);
                break;
            }
        }
    }
}


REGISTER_COMMAND_CPP(ExportFBX)

ExportFBX::ExportFBX()
{
}

ExportFBX::~ExportFBX()
{
}

string ExportFBX::GetName() const
{
    return "ExportFBX";
}

string ExportFBX::GetHelp() const
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Usage: ck-cmd exportfbx
    string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + "\r\n";

    const char help[] = "TODO: Short description for ExportFBX";
    return usage + help;
}

string ExportFBX::GetHelpShort() const
{
    return "TODO: Short help message for ExportFBX";
}

bool ExportFBX::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
    Log::Info("Begin Scan");

    Games& games = Games::Instance();
    const Games::GamesPathMapT& installations = games.getGames();
#if 0

    map<NifFile*, fs::path> skeletons;
    map<NifFile*, set<NifFile*>> pieces;

    Games::Game toDo = Games::TES5;

    for (const auto& bsa : games.bsas(toDo)) {
        std::cout << "Scan: " << bsa.filename() << std::endl;
        BSAFile bsa_file(bsa);



        for (const auto& nif : bsa_file.assets(".*\.nif")) {
            Log::Info("Current File: %s", nif.c_str());

            size_t size = -1;
            const uint8_t* data = bsa_file.extract(nif, size);

            std::string sdata((char*)data, size);
            std::istringstream iss(sdata);

            bool save = false;

            NifFile* niffile = new NifFile(iss);
            if (niffile->isSkeletonOnly()) {
                save = true;
                //skeletons.insert(niffile);
                //out_path = fs::path(".") / fs::path("nif") / "skeletons" / fs::path(nif);
                //fs::create_directories(out_path.parent_path());
                //niffile->Save(out_path.string().c_str());
                if (toDo == Games::TES4)
                    niffile->name = fs::path(nif).parent_path().filename().replace_extension("").string();
                else if (toDo == Games::TES5) //Character Assets
                    niffile->name = fs::path(nif).parent_path().parent_path().filename().replace_extension("").string();
                skeletons[niffile] = fs::path(fs::path(nif));
            }

            if (!save) delete niffile;
            delete data;

        }
    }
    for (const auto& bsa : games.bsas(Games::TES5)) {
        std::cout << "Scan: " << bsa.filename() << std::endl;
        BSAFile bsa_file(bsa);
        for (const auto& nif : bsa_file.assets(".*\.nif")) {
            Log::Info("Current File: %s", nif.c_str());

            size_t size = -1;
            const uint8_t* data = bsa_file.extract(nif, size);

            std::string sdata((char*)data, size);
            std::istringstream iss(sdata);

            bool save = false;

            NifFile* niffile = new NifFile(iss);
            if (niffile->hasExternalSkin()) {

                //meshes.insert(niffile);
                niffile->name = fs::path(nif).filename().replace_extension("").string();
                for (const auto & skeleton : skeletons)
                    if (nif.find(skeleton.second.parent_path().string()) != string::npos) {
                        save = true;
                        pieces[skeleton.first].insert(niffile);
                    }
                //out_path = fs::path(".") / fs::path("nif") / "meshes" / fs::path(nif);
                //fs::create_directories(out_path.parent_path());
                //niffile->Save(out_path.string().c_str());
            }

            if (!save) delete niffile;
            delete data;

        }
    }
#else
    //FBXWrangler* wrangler = new FBXWrangler();
    //NifFile* skeleton = new NifFile("D:\\git\\resources\\in\\Data\\Meshes\\actors\\TES4Goblin\\CharacterAssets\\skeleton.nif");
    //NifFile* mesh = new NifFile("D:\\git\\resources\\in\\Data\\Meshes\\actors\\TES4Goblin\\CharacterAssets\\TestGoblin.nif");
    ////wrangler->AddExternalSkinnedMeshes(*skeleton, {mesh});
    //wrangler->AddNif(*skeleton);
    //wrangler->ExportScene("D:\\git\\resources\\in\\Data\\Meshes\\actors\\TES4Goblin\\CharacterAssets\\skeleton.fbx");
    //wrangler->CloseScene();
    //wrangler->NewScene();
    //wrangler->AddNif(*mesh);
    //wrangler->ExportScene("D:\\git\\resources\\in\\Data\\Meshes\\actors\\TES4Goblin\\CharacterAssets\\TestGoblin.fbx");

    Games::Game tes5 = Games::TES5;

    if (!games.isGameInstalled(tes5)) {
        Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
        return false;
    }

    string animDataContent;
    string animSetDataContent;

    const std::string animDataPath = "meshes\\animationdatasinglefile.txt";
    const std::string animSetDataPath = "meshes\\animationsetdatasinglefile.txt";

    const vector<string> bsas = { "Update.bsa", "Skyrim - Animations.bsa" };

    //by priority order, we first check for overrides
    loadOverrideOrBSA(animDataPath, animDataContent, tes5, bsas);
    loadOverrideOrBSA(animSetDataPath, animSetDataContent, tes5, bsas);

    AnimationCache cache(animDataContent, animSetDataContent);
    cache.printInfo();

#endif

    //FBXWrangler* wrangler = new FBXWrangler();

    //for (const auto & piece : pieces) {
    //	for (const auto & mesh : piece.second) {
    //		wrangler->NewScene();
    //		wrangler->AddExternalSkinnedMeshes(*piece.first, { mesh });
    //		fs::path out_path = fs::path(".") / fs::path("fbx") / fs::path(piece.first->name + "_" + mesh->name + ".fbx");
    //		fs::create_directories(out_path.parent_path());
    //		wrangler->ExportScene(out_path.string());
    //		wrangler->CloseScene();
    //	}
    //}

    //for (auto & piece : pieces) {
    //	for (auto & mesh : piece.second) {
    //		delete mesh;
    //	}
    //	if (piece.first) delete piece.first;
    //}

    //	delete wrangler;
    return true;
}