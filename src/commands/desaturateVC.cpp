#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/desaturateVC.h>
#include <core/MathHelper.h>

using namespace ckcmd;
using namespace ckcmd::desaturateVC;

using namespace Niflib;
using namespace std;

static bool BeginScan(string scanPath);

REGISTER_COMMAND_CPP(DesaturateVC)

DesaturateVC::DesaturateVC()
{
}

DesaturateVC::~DesaturateVC()
{
}

string DesaturateVC::GetName() const
{
	return "DesaturateVC";
}

string DesaturateVC::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd nifscan [-i <path_to_scan>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [-i <path_to_scan>]\r\n";

	const char help[] =
		R"(Scan Skyrim meshes for any errors.
		
		Arguments:
			<path_to_scan> path to models you want to check for errors)";

	return usage + help;
}

string DesaturateVC::GetHelpShort() const
{
	return "TODO: Short help message for ConvertNif";
}

void findFilesWithExtension(fs::path startingDir, string extension, vector<fs::path>& results) {
	if (!exists(startingDir) || !is_directory(startingDir)) return;
	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(startingDir))
	{
		if (is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == extension) {
			results.push_back(dirEntry.path().string());
		}
	}
}

void desaturate(vector<NiObjectRef> blocks, NifInfo info) {
	for (auto& block : blocks) {
		if (block->IsDerivedType(NiTriShapeData::TYPE)) {
			NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(block);
			vector<Color4 > vcs = data->GetVertexColors();
			for (auto& vc : vcs) {
				float gray = (vc.r * 0.2126 + vc.g * 0.7152 + vc.b * 0.0722);
				vc.r = gray; vc.g = gray; vc.b = gray;
			}
			data->SetVertexColors(vcs);
		}
		if (block->IsDerivedType(NiTriStripsData::TYPE)) {
			NiTriStripsDataRef data = DynamicCast<NiTriStripsData>(block);
			vector<Color4 > vcs = data->GetVertexColors();
			for (auto& vc : vcs) {
				float gray = (vc.r * 0.2126 + vc.g * 0.7152 + vc.b * 0.0722);
				vc.r = gray; vc.g = gray; vc.b = gray;
			}
			data->SetVertexColors(vcs);
		}
	}
}

bool DesaturateVC::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string scanPath;

	scanPath = parsedArgs["<path_to_scan>"].asString();
	if (fs::exists(scanPath) && fs::is_directory(scanPath)) {
		vector<fs::path> nifs; findFilesWithExtension(scanPath, ".nif", nifs);
		for (size_t i = 0; i < nifs.size(); i++) {
			Log::Info("Current File: %s", nifs[i].string().c_str());
			NifInfo info;
			try {
				vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
				desaturate(blocks, info);
				fs::path out = scanPath / fs::path("out") / relative_to(nifs[i], scanPath);
				if (!fs::exists(out.parent_path())) fs::create_directory(out.parent_path());
				Log::Info("Output File: %s", out.string().c_str());
				WriteNifTree(out.string(), GetFirstRoot(blocks), info);
			}
			catch (const std::exception& e) {
				Log::Info("ERROR: %s", e.what());
			}
		}
		Log::Info("Done..");
	}
	int a;
	cin >> a;
	//bool result = BeginScan(scanPath);
	return true;
}