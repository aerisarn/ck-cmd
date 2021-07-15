#define NOMINMAX

#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <core/HKXWrangler.h>
#include <commands/MergeNif.h>
#include <commands/Geometry.h>
#include <core/games.h>
#include <core/bsa.h>
#include <core/NifFile.h>
#include <commands/NifScan.h>

#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\StiffSpring\hkpStiffSpringConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>

#include <Physics\Collide\Util\hkpTriangleUtil.h>

#include <limits>
#include <array>
#include <unordered_map>
#include <DirectXTex.h>

using namespace Niflib;

static bool BeginConversion(string importPath, string exportPath);
static void InitializeHavok();
static void CloseHavok();


static Games& games = Games::Instance();

MergeNif::MergeNif()
{
}

MergeNif::~MergeNif()
{
}

string MergeNif::GetName() const
{
	return "MergeNif";
}

string MergeNif::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd MergeNif [-i <path_to_import>] [-e <path_to_export>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [<path_to_folder1>] [<path_to_folder2>] \r\n";

	//will need to check this help in console/
	const char help[] =
		R"(Merge Nif files and prints differences.
		
		Arguments:
			<path_to_folder1> path to first models directory
			<path_to_folder2> path to  models directory

		)";

	return usage + help;
}

string MergeNif::GetHelpShort() const
{
	//I'm unsure about returning a string.. It doesn't show up on the console..
	//Log::Info("Convert Oblivion version models to Skyrim's format.");
	return "TODO: Short help message for MergeNif";
}

bool MergeNif::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importPath="", exportPath="";

	if (parsedArgs["<path_to_folder1>"].isString())
		importPath = parsedArgs["<path_to_folder1>"].asString();
	if (parsedArgs["<path_to_folder2>"].isString())
		exportPath = parsedArgs["<path_to_folder2>"].asString();

	InitializeHavok();
	BeginConversion(importPath, exportPath);
	CloseHavok();
	return true;
}

static void findFiles(fs::path startingDir, string extension, set<string>& results) {
	if (!exists(startingDir) || !is_directory(startingDir)) return;
	for (auto& dirEntry : fs::recursive_directory_iterator(startingDir))
	{
		if (fs::is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == extension) {
			string relative = fs::relative(dirEntry.path(), startingDir).string();
			std::for_each(relative.begin(), relative.end(), [](char& c) {
				c = ::tolower(c);
			});
			if (relative.find("armor") == 0)
				continue;
			if (relative.find("weapons") == 0)
				continue;
			if (relative.find("clothes") == 0)
				continue;
			if (relative.find("creatures") == 0)
				continue;
			if (relative.find("characters") == 0)
				continue;
			if (relative.find("_far.") != string::npos)
				continue;
			if (relative.find("_lod") != string::npos)
				continue;
			results.insert(relative);
		}
	}
}

template<typename T>
bool blocK_equivalent(const Ref<T>& left_block, const Ref<T>& right_block)
{
	return false; //this is not equal
}

template<>
bool blocK_equivalent(const Ref<NiNode>& left_block, const Ref<NiNode>& right_block)
{
	return left_block->GetName() == right_block->GetName(); //this is not equal
}

template<>
bool blocK_equivalent(const Ref<BSFadeNode>& left_block, const Ref<BSFadeNode>& right_block)
{
	return left_block->GetName() == right_block->GetName(); //this is not equal
}


template<>
bool blocK_equivalent(const Ref<BSShaderTextureSet>& left_block, const Ref<BSShaderTextureSet>& right_block)
{
	auto texs_left = left_block->GetTextures();
	auto texs_right = right_block->GetTextures();
	for (int i = 0; i < texs_left.size(); i++)
	{
		if (!texs_left[i].empty() && (texs_right.size() < i + 1 || !AnimationCache::iequals(texs_left[i],texs_right[i])))
		{
			return false;
		}
	}
	for (int i = 0; i < texs_right.size(); i++)
	{
		if (!texs_right[i].empty() && (texs_left.size() < i + 1 || !AnimationCache::iequals(texs_left[i], texs_right[i])))
		{
			return false;
		}
	}
	return true;
}

bool equivalent(const NiObjectRef& left_block, const NiObjectRef& right_block);

template<>
bool blocK_equivalent(const Ref<BSLightingShaderProperty>& left_block, const Ref<BSLightingShaderProperty>& right_block)
{
	return left_block->GetTextureSet() == right_block->GetTextureSet() ||
		equivalent(StaticCast<NiObject>(left_block->GetTextureSet()), StaticCast<NiObject>(right_block->GetTextureSet()));
}

template<>
bool blocK_equivalent(const Ref<bhkMoppBvTreeShape>& left_block, const Ref<bhkMoppBvTreeShape>& right_block)
{
	return *left_block->GetShape() == *right_block->GetShape() ||
		equivalent(StaticCast<NiObject>(left_block->GetShape()), StaticCast<NiObject>(right_block->GetShape()));
}

template<>
bool blocK_equivalent(const Ref<bhkRigidBody>& left_block, const Ref<bhkRigidBody>& right_block)
{
	return *left_block->GetShape() == *right_block->GetShape() ||
		equivalent(StaticCast<NiObject>(left_block->GetShape()), StaticCast<NiObject>(right_block->GetShape()));
}

template<>
bool blocK_equivalent(const Ref<bhkRigidBodyT>& left_block, const Ref<bhkRigidBodyT>& right_block)
{
	return *left_block->GetShape() == *right_block->GetShape() ||
		equivalent(StaticCast<NiObject>(left_block->GetShape()), StaticCast<NiObject>(right_block->GetShape()));
}

template<>
bool blocK_equivalent(const Ref<bhkCompressedMeshShape>& left_block, const Ref<bhkCompressedMeshShape>& right_block)
{
	return *left_block->GetData() == *right_block->GetData() ||
		equivalent(StaticCast<NiObject>(left_block->GetData()), StaticCast<NiObject>(right_block->GetData()));
}

bool almost(const Vector4& a, const Vector4& b)
{
	return abs(a.x - b.x) < 10e-2 &&
		abs(a.y - b.y) < 10e-2 &&
		abs(a.z - b.z) < 10e-2;
}

bool almost(const vector<Vector3>& a, const vector<Vector3>& b)
{
	if (a.size() != b.size())
		return false;
	for (int i=0; i<a.size(); i++)
	{
		if (abs(a[i].x - b[i].x) > 10e-1 ||
			abs(a[i].x - b[i].x) > 10e-1 ||
			abs(a[i].x - b[i].x) > 10e-1)
			return false;
	}
	return true;
}

bool almost(const vector<TexCoord>& a, const vector<TexCoord>& b)
{
	if (a.size() != b.size())
		return false;
	for (int i = 0; i < a.size(); i++)
	{
		if (abs(a[i].u - b[i].u) > 10e-1 ||
			abs(a[i].v - b[i].v) > 10e-1)
			return false;
	}
	return true;
}

bool almost(const vector<Color4>& a, const vector<Color4>& b)
{
	if (a.size() != b.size())
		return false;
	for (int i = 0; i < a.size(); i++)
	{
		if (abs(a[i].r - b[i].r) > 10e-1 ||
			abs(a[i].g - b[i].g) > 10e-1 ||
			abs(a[i].b - b[i].b) > 10e-1 ||
			abs(a[i].a - b[i].a) > 10e-1)
			return false;
	}
	return true;
}

template<>
bool blocK_equivalent(const Ref<bhkCompressedMeshShapeData>& left_block, const Ref<bhkCompressedMeshShapeData>& right_block)
{
	return almost(left_block->GetBoundsMax(), right_block->GetBoundsMax()) &&
		almost(left_block->GetBoundsMin(), right_block->GetBoundsMin());
}	

template<>
bool blocK_equivalent(const Ref<NiTriShapeData>& left_block, const Ref<NiTriShapeData>& right_block)
{
	return almost(left_block->GetVertices(), right_block->GetVertices()) &&
		( (left_block->GetHasUv() ==false && left_block->GetHasUv() == false) || (almost(left_block->GetUvSets()[0], right_block->GetUvSets()[0]))) &&
		almost(left_block->GetVertexColors(),right_block->GetVertexColors());
}

template<typename T>
bool equivalent(const NiObjectRef& left_block, const NiObjectRef& right_block)
{
	return blocK_equivalent(DynamicCast<T>(left_block), DynamicCast<T>(right_block));
}

bool equivalent(const NiObjectRef& left_block, const NiObjectRef& right_block)
{
	if (left_block->IsSameType(BSShaderTextureSet::TYPE) && right_block->IsSameType(BSShaderTextureSet::TYPE)) {
		return equivalent<BSShaderTextureSet>(left_block, right_block);
	}
	if (left_block->IsSameType(BSLightingShaderProperty::TYPE) && right_block->IsSameType(BSLightingShaderProperty::TYPE)) {
		return equivalent<BSLightingShaderProperty>(left_block, right_block);
	}
	if (left_block->IsSameType(NiNode::TYPE) && right_block->IsSameType(NiNode::TYPE)) {
		return equivalent<NiNode>(left_block, right_block);
	}
	if (left_block->IsSameType(BSFadeNode::TYPE) && right_block->IsSameType(BSFadeNode::TYPE)) {
		return equivalent<NiNode>(left_block, right_block);
	}
	if (left_block->IsSameType(BSXFlags::TYPE) && right_block->IsSameType(BSXFlags::TYPE)) {
		return true; // not interested as automatically fixed
	}
	if (left_block->IsSameType(bhkMoppBvTreeShape::TYPE) && right_block->IsSameType(bhkMoppBvTreeShape::TYPE)) {
		return equivalent<bhkMoppBvTreeShape>(left_block, right_block); // not interested as automatically fixed
	}
	if (left_block->IsSameType(bhkRigidBody::TYPE) && right_block->IsSameType(bhkRigidBody::TYPE)) {
		return equivalent<bhkRigidBody>(left_block, right_block);; // not interested as automatically fixed
	}
	if (left_block->IsSameType(bhkRigidBodyT::TYPE) && right_block->IsSameType(bhkRigidBodyT::TYPE)) {
		return equivalent<bhkRigidBodyT>(left_block, right_block);; // not interested as automatically fixed
	}
	//Hack
	if (left_block->IsSameType(bhkRigidBody::TYPE) && right_block->IsSameType(bhkRigidBodyT::TYPE)) {
		return equivalent<bhkRigidBody>(left_block, right_block);; // not interested as automatically fixed
	}
	if (left_block->IsSameType(bhkRigidBodyT::TYPE) && right_block->IsSameType(bhkRigidBody::TYPE)) {
		return equivalent<bhkRigidBody>(left_block, right_block);; // not interested as automatically fixed
	}
	//Hack end
	if (left_block->IsSameType(bhkCompressedMeshShape::TYPE) && right_block->IsSameType(bhkCompressedMeshShape::TYPE)) {
		return equivalent<bhkCompressedMeshShape>(left_block, right_block);; // not interested as automatically fixed
	}
	if (left_block->IsSameType(bhkCompressedMeshShapeData::TYPE) && right_block->IsSameType(bhkCompressedMeshShapeData::TYPE)) {
		return equivalent<bhkCompressedMeshShapeData>(left_block, right_block);; // not interested as automatically fixed
	}
	if (left_block->IsSameType(NiTriShapeData::TYPE) && right_block->IsSameType(NiTriShapeData::TYPE)) {
		return equivalent<NiTriShapeData>(left_block, right_block);; // not interested as automatically fixed
	}
	return false;
}

static inline void NiObjectsDiff(const vector<NiObjectRef>& left_blocks, const vector<NiObjectRef>& right_blocks, vector<NiObjectRef>& result)
{
	for (const auto& left_block : left_blocks) {
		bool found = false;
		for (const auto& right_block : right_blocks) {
			if (*left_block == *right_block || equivalent(left_block, right_block)) {
				found = true;
				break;
			}
		}
		if (!found)
			result.push_back(left_block);
	}
}

template<typename T>
typename T::size_type LevenshteinDistance(const T& source, const T& target) {
	if (source.size() > target.size()) {
		return LevenshteinDistance(target, source);
	}

	using TSizeType = typename T::size_type;
	const TSizeType min_size = source.size(), max_size = target.size();
	std::vector<TSizeType> lev_dist(min_size + 1);

	for (TSizeType i = 0; i <= min_size; ++i) {
		lev_dist[i] = i;
	}

	for (TSizeType j = 1; j <= max_size; ++j) {
		TSizeType previous_diagonal = lev_dist[0], previous_diagonal_save;
		++lev_dist[0];

		for (TSizeType i = 1; i <= min_size; ++i) {
			previous_diagonal_save = lev_dist[i];
			if (source[i - 1] == target[j - 1]) {
				lev_dist[i] = previous_diagonal;
			}
			else {
				lev_dist[i] = std::min(std::min(lev_dist[i - 1], lev_dist[i]), previous_diagonal) + 1;
			}
			previous_diagonal = previous_diagonal_save;
		}
	}

	return lev_dist[min_size];
}


size_t CharDiff(const string& source, const string& target) {
	if (source.size() > target.size()) {
		return CharDiff(target, source);
	}

	size_t diff = target.size() - source.size();
	for (int i = 0; i < source.size(); i++)
	{
		if (source.at(i) != target.at(i))
			diff++;
		if (diff > 30)
			break;
	}

	return diff;
}

struct NiObjectStringDistance {

	bool operator() (NiObjectRef i, NiObjectRef j) 
	{ 
		return LevenshteinDistance(i->asString(true), j->asString(true));
	}
};

bool BeginConversion(string importPath, string exportPath) {
	char fullName[MAX_PATH], exeName[MAX_PATH];
	GetModuleFileName(NULL, fullName, MAX_PATH);
	_splitpath(fullName, NULL, NULL, exeName, NULL);

	set<string> left_nifs;
	set<string> right_nifs;

	if (fs::exists(importPath) && fs::is_directory(importPath) &&
		fs::exists(exportPath) && fs::is_directory(exportPath))
	{
		findFiles(importPath, ".nif", left_nifs);
		findFiles(exportPath, ".nif", right_nifs);

		Log::Info("found %d files on the first folder and %d files on the second folder", left_nifs.size(), right_nifs.size());
		set<string> result_left;
		set<string> result_right;
		set_difference(left_nifs.begin(), left_nifs.end(), right_nifs.begin(), right_nifs.end(),
			inserter(result_left, result_left.end()));
		set_difference(right_nifs.begin(), right_nifs.end(), left_nifs.begin(), left_nifs.end(),
			inserter(result_right, result_right.end()));

		if (result_left.size())
		{
			Log::Info("Only Left:");
			for (const auto& difference : result_left) {
				Log::Info(difference.c_str());
			}
		}

		if (result_right.size())
		{
			Log::Info("Only Right:");
			for (const auto& difference : result_right) {
				Log::Info(difference.c_str());
			}
		}

		vector<string> intersection;
		set_intersection(left_nifs.begin(), left_nifs.end(),
			right_nifs.begin(), right_nifs.end(),
			std::back_inserter(intersection));

		vector<string> different_blocksize;

		ofstream left_result_txt;
		ofstream right_result_txt;

		fs::create_directories("left");
		fs::create_directories("right");

		for (const auto& nif : intersection) {

			fs::path left_path = fs::path("left") / fs::path(nif); left_path.replace_extension(".diff");
			fs::create_directories(left_path.parent_path());
			fs::path right_path = fs::path("right") / fs::path(nif); right_path.replace_extension(".diff");
			fs::create_directories(right_path.parent_path());

			left_result_txt.open(left_path.string());
			right_result_txt.open(right_path.string());

			Log::Info("Processing %s", nif.c_str());
			left_result_txt << nif.c_str() << endl;
			right_result_txt << nif.c_str() << endl;
			vector<NiObjectRef> left_blocks;
			vector<NiObjectRef> right_blocks;
			try {
				NifInfo left_info;
				left_blocks = ReadNifList((fs::path(importPath) / nif).string(), &left_info);
			}
			catch (...) {
				Log::Error("Unable to read %s", (fs::path(importPath) / nif).string().c_str());
				continue;
			}
			try {
				NifInfo right_info;
				right_blocks = ReadNifList((fs::path(exportPath) / nif).string(), &right_info);
			}
			catch (...) {
				Log::Error("Unable to read %s", (fs::path(exportPath) / nif).string().c_str());
				continue;
			}

			sort(left_blocks.begin(), left_blocks.end());
			sort(right_blocks.begin(), right_blocks.end());

			vector<NiObjectRef> result_left;
			vector<NiObjectRef> result_right;

			NiObjectsDiff(left_blocks, right_blocks, result_left);
			NiObjectsDiff(right_blocks, left_blocks, result_right);

			vector<pair<NiObjectRef, NiObjectRef>> nearest;

			auto it = result_left.begin();
			//for (int i = 0; i < result_left.size(); i++)
			while (it != result_left.end())
			{
				bool found = false;
				if (!(*it)->IsSameType(BSBehaviorGraphExtraData::TYPE) && result_right.size() > 0)
				{
					int max_distance = numeric_limits<int>::max();
					int max_distance_index = -1;
					
					for (int j = 0; j < result_right.size(); j++) {
						if ((*it)->GetInternalType().IsSameType(result_right[j]->GetInternalType()) ||
							(*it)->IsSameType(bhkRigidBody::TYPE) && result_right[j]->IsSameType(bhkRigidBodyT::TYPE))
						{
							size_t distance = CharDiff((*it)->asString(true), result_right[j]->asString(true));
							if (distance < max_distance)
							{
								max_distance = distance;
								max_distance_index = j;
							}
						}
					}
					if (max_distance_index >= 0)
					{
						found = true;
						nearest.push_back({ (*it), result_right[max_distance_index] });
						result_right.erase(result_right.begin() + max_distance_index);
					}
				}
				if (found)
					it = result_left.erase(it);
				else
					it++;
			}
			
			for (const auto& object_pair : nearest) {
				int index_left = std::distance(left_blocks.begin(), std::find(left_blocks.begin(), left_blocks.end(), object_pair.first));
				left_result_txt << "######### Block " << index_left << " type: " << object_pair.first->GetInternalType().GetTypeName() << std::endl;
				left_result_txt << object_pair.first->asString(true) << endl;
				
				int index_right = std::distance(right_blocks.begin(), std::find(right_blocks.begin(), right_blocks.end(), object_pair.second));
				right_result_txt << "######### Block " << index_right << " type: " << object_pair.second->GetInternalType().GetTypeName() << std::endl;
				right_result_txt << object_pair.second->asString(true) << endl;
			}

			for (const auto& object : result_left) {
				int index_left = std::distance(left_blocks.begin(), std::find(left_blocks.begin(), left_blocks.end(), object));
				left_result_txt << "######### Block " << index_left << " type: " << object->GetInternalType().GetTypeName() << std::endl;
				left_result_txt << object->asString(true) << endl;
			}

			for (const auto& object : result_right) {
				int index_right = std::distance(right_blocks.begin(), std::find(right_blocks.begin(), right_blocks.end(), object));
				right_result_txt << "######### Block " << index_right << " type: " << object->GetInternalType().GetTypeName() << std::endl;
				right_result_txt << object->asString(true) << endl;
			}

			left_result_txt.close();
			right_result_txt.close();
		}
	}
	Log::Info("Done");
	return true;
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("About - Help about this program."); break;
	case hkxcmd::htLong: {
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s about", exeName);
		Log::Info("  Prints additional information about this program.");
	}
						 break;
	}
}

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}