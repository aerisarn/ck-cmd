#include "stdafx.h"
#include <commands/NifScan.h>
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

using namespace ckcmd;
using namespace ckcmd::nifscan;

using namespace Niflib;
using namespace std;

static bool BeginScan();

REGISTER_COMMAND_CPP(NifScan)

NifScan::NifScan()
{
}

NifScan::~NifScan()
{
}

string NifScan::GetName() const
{
    return "NifScan";
}

string NifScan::GetHelp() const
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Usage: ck-cmd games
    string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + "\r\n";

    const char help[] = "TODO: Short description for NifScan";

    return usage + help;
}

string NifScan::GetHelpShort() const
{
    return "TODO: Short help message for ConvertNif";
}

bool NifScan::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
    bool result = BeginScan();
    Log::Info("NifScan Ended");
    return result;
}

//BSXFlags
/*
<niobject name="BSXFlags" abstract="0" inherit="NiIntegerExtraData">
Controls animation and collision.  Integer holds flags:
Bit 0 : has Gamebryo Blending. Not applicable to NIFs that are meant to be attached to others
Bit 1 : has Havok (at least a collision or a phantom collision)
Bit 2 : has Havok Ragdoll (really means it's a skeleton model, even if there's no ragdoll constraint. has at least a BlendCollision object)
Bit 3 : has Multiple Havok Collisions (multiple bhkCollsionObjects)
Bit 4 : has AttachLight/FlameNode (will be replaced at run time with a light) / AddonNode in Skyrim
Bit 5 : has EditorMarkers
Bit 6 : is Dynamic Havok Rigid Bodies(quality != {MO_QUAL_INVALID, MO_QUAL_FIXED}, doesn't make sense without bit 1)
Bit 7 : 1 if a model has a single collision, or a single kinematic chain (or any number of phantom collisions, which is strange, the only model with 2 phantom collision may be an outlier) . This is influenced by the niswitchnode, which will result in a single branch to be displayed at a time so even if more branches have multiple collision, if a collision or kinematic chain is to be displayed at a a single time in a branch, the bit will be set
Bit 8 : bIKTarget(Skyrim)/needsTransformUpdates (never set in vanilla skyrim + DLCs)
Bit 9 : bExternalEmit(Skyrim, has Own Emit into BSLightingShaderPropery or External Emittance into BSEffectShaderProperty)
Bit 10: bMagicShaderParticles(Skyrim) (never set in vanilla skyrim + DLCs)
Bit 11: bLights(Skyrim) (never set in vanilla skyrim + DLCs)
Bit 12: bBreakable(Skyrim) (never set in vanilla skyrim + DLCs)
Bit 13: bSearchedBreakable(Skyrim) .. Runtime only? (never set in vanilla skyrim + DLCs)
</niobject>

*/

//Bit 7 needs hierarchy visitor
inline bool isSingleChain(const NiObjectRef& root, const NifInfo& info) {
	return SingleChunkFlagVerifier(*root, info).singleChunkVerified;
}



bsx_flags_t ckcmd::nifscan::calculateSkyrimBSXFlags(const vector<NiObjectRef>& blocks, const NifInfo& info) {
	bsx_flags_t flags = 0;
	NiObjectRef root = GetFirstRoot(blocks);
	int num_collisions = 0;
	int num_phantom_collisions = 0;
	bool isSkeleton = false;
	bool isRootNiNode = root->IsSameType(NiNode::TYPE);
	bool isRootBSFade = root->IsSameType(BSFadeNode::TYPE);
	bool isRootBSLeaf = root->IsSameType(BSLeafAnimNode::TYPE);
	bool isRootBSTree = root->IsSameType(BSTreeNode::TYPE);
	bool hasMultiBound = false;
	bool isSkinned = false;
	bool hasExternalSkeleton = false;
	bool hasCollisionList = false;


	set<NiObject*> bones;

	for (NiObjectRef block : blocks) {
		if (block->IsDerivedType(bhkCollisionObject::TYPE)) {
			num_collisions++;
		}
		if (block->IsDerivedType(bhkSPCollisionObject::TYPE)) {
			num_phantom_collisions++;
		}
		if (block->IsDerivedType(bhkBlendCollisionObject::TYPE))
			isSkeleton = true;
		if (block->IsDerivedType(NiSkinInstance::TYPE)) {
			NiSkinInstanceRef niskini = DynamicCast<NiSkinInstance>(block);
			isSkinned = true;
			for (NiNode* bone : niskini->GetBones())
				bones.insert(bone);
		}
		if (block->IsDerivedType(bhkListShape::TYPE))
			hasCollisionList = true;
		if (block->IsSameType(BSMultiBound::TYPE))
			hasMultiBound = true;
	}



	if (isSkinned && root->IsDerivedType(NiNode::TYPE)) {
		NiNodeRef rootn = DynamicCast<NiNode>(root);
		for (NiObjectRef ref : rootn->GetChildren()) {
			set<NiObject*>::iterator it = bones.find(ref);
			if (it != bones.end()) bones.erase(it);

			//if (ref->IsDerivedType(bhkCollisionObject::TYPE))
			//	hasRootCollision = true;
		}
		if (bones.empty())
			hasExternalSkeleton = isRootNiNode;
	}
		
	for (NiObjectRef block : blocks) {
		if ((block->IsDerivedType(NiTimeController::TYPE) || block->IsDerivedType(BSValueNode::TYPE)) && !isSkeleton && !hasExternalSkeleton)
		{
			flags[0] = true;
		}
		if (block->IsDerivedType(bhkRigidBody::TYPE)) {
			bhkRigidBodyRef rigid_body = DynamicCast<bhkRigidBody>(block);
			if (isSkeleton || (rigid_body->GetQualityType() != hkQualityType::MO_QUAL_INVALID && rigid_body->GetQualityType() != hkQualityType::MO_QUAL_FIXED)) {
				flags[6] = true;
			}
		}
		if (isSkeleton /*&& (block->IsDerivedType(bhkMalleableConstraint::TYPE) || block->IsDerivedType(bhkRagdollConstraint::TYPE))*/) {
			flags[2] = true;
		}
		if (block->IsDerivedType(NiNode::TYPE)) {
			NiNodeRef node = DynamicCast<NiNode>(block);
			if (node->GetName().find("AddonNode") != string::npos)
				flags[4] = true;
		}
		//if (block->IsDerivedType(NiObjectNET::TYPE)) {
		//	NiObjectNETRef node = DynamicCast<NiObjectNET>(block);
		//	if (node->GetName().find("EditorMarker") != string::npos) //TODO: wrong: only set if not parented to a NiSwitchNode, needs a visitor
		//		flags[5] = true;
		//}
		if (block->IsDerivedType(BSValueNode::TYPE)) {
			flags[4] = true;
		}
		if(block->IsDerivedType(BSLightingShaderProperty::TYPE)) {
			BSLightingShaderPropertyRef shader = DynamicCast<BSLightingShaderProperty>(block);
			if (shader->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) {
				flags[9] = true;
			}
		}
		if (block->IsDerivedType(BSEffectShaderProperty::TYPE)) {
			BSEffectShaderPropertyRef shader = DynamicCast<BSEffectShaderProperty>(block);
			if (shader->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) {
				flags[9] = true;
			}
		}
		
	}

	bool hasRootCollision =  !isRootBSTree && ((isRootBSFade && DynamicCast<BSFadeNode>(root)->GetCollisionObject() != NULL && 
		DynamicCast<BSFadeNode>(root)->GetCollisionObject()->IsDerivedType(bhkCollisionObject::TYPE)) ||
		(isRootBSLeaf && DynamicCast<BSLeafAnimNode>(root)->GetCollisionObject() != NULL &&
			DynamicCast<BSLeafAnimNode>(root)->GetCollisionObject()->IsDerivedType(bhkCollisionObject::TYPE)) ||
		hasMultiBound); //wrong. may be complex but only in 6 models, need further investigation
	if (isSingleChain(root, info))
		flags[7] = true;

	if (MarkerBranchVisitor(*root, info).marker) {
		flags[5] = true;
	}

	//if ((num_collisions > 1) && !flags[7])
	//	flags[3] = true;

	//if (!hasRootCollision)
	//	flags[3] = true;

	if (num_collisions > 0 || num_phantom_collisions > 0) {
		if (!isSkeleton && num_collisions > 0 && (!hasRootCollision || num_collisions > 1))
			flags[3] = true;
		flags[1] = true;
	}



	return flags;
}

#include <core/games.h>
#include <core/bsa.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;

//Duplicate Method, just wanted to test.
void findFilesn(fs::path startingDir, string extension, vector<fs::path>& results) {
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

void ScanNif(vector<NiObjectRef> blocks, NifInfo info)
{
	Games& games = Games::Instance();
	const Games::GamesPathMapT& installations = games.getGames();

	NiObjectRef root = GetFirstRoot(blocks);

	bsx_flags_t calculated = calculateSkyrimBSXFlags(blocks, info);
	bsx_flags_t actual;

	bool write = false;
	bool hasVFOnShader = false;
	vector<string> names = vector<string>();

	for (int i = 0; i != blocks.size(); i++) {

		if (blocks[i]->IsSameType(BSXFlags::TYPE)) {
			BSXFlagsRef ref = DynamicCast<BSXFlags>(blocks[i]);
			if (ref->GetName() != "BSX") {
				Log::Info("Block[%i]: A 'BSXFlag' block needs to be named 'BSX'", i);
			}
			actual = ref->GetIntegerData();
			//fxdragoncrashfurrow01 has bit 1 not set but I can't get why

			if (ref->GetIntegerData() != calculated.to_ulong()) {
				write = true;
				Log::Info("Block[%d]: BSXFlag: value: [%d %s], estimate: [%d %s]", i, actual.to_ulong(), actual.to_string().c_str(), calculated.to_ulong(), calculated.to_string().c_str());
			}
		}

		if (blocks[i]->IsSameType(BSInvMarker::TYPE)) {
			if (DynamicCast<BSInvMarker>(blocks[i])->GetName() != "INV") {
				Log::Info("Block[%i]: A 'BSInvMarker' block needs to be named 'INV'", i);
			}
		}

		if (blocks[i]->IsSameType(BSFurnitureMarker::TYPE)) {
			if (DynamicCast<BSFurnitureMarker>(blocks[i])->GetName() != "FRN") {
				Log::Info("Block[%i]: A 'BSFurnitureMarker' block needs to be named 'FRN'", i);
			}
		}

		if (blocks[i]->IsSameType(BSBound::TYPE)) {
			if (DynamicCast<BSBound>(blocks[i])->GetName() != "BBX") {
				Log::Info("Block[%i]: A 'BSBound' block needs to be named 'BBX'", i);
			}
		}

		if (blocks[i]->IsSameType(Niflib::BSBehaviorGraphExtraData::TYPE)) {
			if (DynamicCast<BSBehaviorGraphExtraData>(blocks[i])->GetName() != "BGED") {
				Log::Info("Block[%i]: A 'BSBehaviorGraphExtraData' block needs to be named 'BGED'", i);
			}
		}

		if (blocks[i]->IsSameType(BSBoneLODExtraData::TYPE)) {
			if (DynamicCast<Niflib::BSBoneLODExtraData>(blocks[i])->GetName() != "BSBoneLOD") {
				Log::Info("Block[%i]: A 'BSBoneLODExtraData' block needs to be named 'BSBoneLOD'", i);
			}
		}

		if (blocks[i]->IsSameType(NiTriStrips::TYPE)) {
			Log::Info("Block[%i]: NiTriStrips needs to be triangulated.", i);
		}

		if (blocks[i]->IsSameType(NiSkinPartition::TYPE)) {
			for (SkinPartition partition : DynamicCast<NiSkinPartition>(blocks[i])->GetPartition()) {
				if (partition.numStrips > 0)
					Log::Info("Block[%i]: NiSkinPartition contains strips. (Obsolete in SSE)", i);
			}
		}

		if (blocks[i]->IsDerivedType(NiTimeController::TYPE)) {
			NiTimeControllerRef ref = DynamicCast<NiTimeController>(blocks[i]);
			if (ref->GetTarget() == NULL) {
				Log::Info("Block[%i]: Controller as no target. This will increase the chances of a crash.", i);
			}
		}

		if (blocks[i]->IsDerivedType(NiControllerSequence::TYPE)) {
			NiSequenceRef ref = DynamicCast<NiSequence>(blocks[i]);

			if (ref->GetControlledBlocks().size() != 0) {
				vector<ControlledBlock> blocks = ref->GetControlledBlocks();

				for (int y = 0; y != blocks.size(); y++) {
					if (blocks[y].controllerType == "") {
						Log::Info("Block[%i]: ControlledBlock number %i, has a blank controller type.", i, y);
					}
				}
			}
		}
		if (blocks[i]->IsDerivedType(NiObjectNET::TYPE)) {
			NiObjectNETRef  node = DynamicCast<NiObjectNET>(blocks[i]);
			if (std::find(names.begin(), names.end(), node->GetName()) == names.end()) {
				names.push_back(node->GetName());
			}
		}

		if (blocks[i]->IsSameType(BSLightingShaderProperty::TYPE)) {
			BSLightingShaderPropertyRef  shaderprop = DynamicCast<BSLightingShaderProperty>(blocks[i]);
			if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS) == SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS) {
				hasVFOnShader = true;
			}
			if (shaderprop->GetSkyrimShaderType() == BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP /*|| BSLightingShaderPropertyShaderType::ST_EYE_ENVMAP*/) {
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) != SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
					Log::Info("Block[%i]: ShaderType is 'Environment', but ShaderFlags1 does not include 'Environment Mapping'.", i);
				}
				if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) == SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
					Log::Info("Block[%i]: ShaderType is 'Environment', but ShaderFlags 2 has enabled 'glow' flag.", i);
				}
				if (shaderprop->GetTextureSet()->GetTextures().size() >= 5) {
					if (shaderprop->GetTextureSet()->GetTextures()[4] == "") {
						Log::Info("Block[%i]: ShaderType is 'Environment', but no 'Environment' texture is present.", i);
					}
				}
				else {
					Log::Info("Block[%i]: TextureSet size is too small to include 'Environment' texture.", i);
				}
			}
			if (shaderprop->GetSkyrimShaderType() == BSLightingShaderPropertyShaderType::ST_GLOW_SHADER) {
				Log::Info("Block is glow shader.");
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) != SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) {
					Log::Info("Block[%i]: ShaderType is 'Glow', but ShaderFlags1 does not include 'External Emittance'.", i);
				}
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) == SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
					Log::Info("Block[%i]: ShaderType is 'Glow', but ShaderFlags1 includes 'Environment Mapping'.", i);
				}
				if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) != SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
					Log::Info("Block[%i]: ShaderType is 'Glow', but ShaderFlags2 does not include 'Glow Map'.", i);
				}
				if (shaderprop->GetTextureSet()->GetTextures().size() > 3) {
					if (shaderprop->GetTextureSet()->GetTextures()[2] == "") {
						Log::Info("Block[%i]: ShaderType is 'Glow', but no 'Glow' texture is present.", i);
					}
				}
				else {
					Log::Info("Block[%i]: TextureSet size is too small to include 'Glow' texture.", i);
				}
			}
		}

		if (blocks[i]->IsSameType(BSShaderTextureSet::TYPE)) {
			BSShaderTextureSetRef set = DynamicCast<BSShaderTextureSet>(blocks[i]);
			if (set->GetTextures().size() != 0) {
				for (std::string texture : set->GetTextures()) {
					bool isTexture = (texture.substr(0, 7) != "textures"); //Possible issue: If the user has capitalized "textures"#
					if (!isTexture) {
						Log::Info("Block[%i]: TextureSet includes paths not relative to data.", i);
					}
					else {
						bool doesExist = false;
						if ((fs::exists(games.data(Games::TES5) / texture)) || (fs::exists(games.data(Games::TES5) / "textures" / texture))) { //this needs sorting out.
							doesExist = true;
						}
						if (!doesExist) {
							Log::Info("Block[%i]: Texture: '%s' does not exist!", i, texture.c_str());
						}
					}

				}
			}
		}

		if (blocks[i]->IsSameType(NiTriShape::TYPE)) {
			NiTriShapeRef shape = DynamicCast<NiTriShape>(blocks[i]);
			NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(shape->GetData());

			if (data != NULL && shape != NULL && data != NULL) {
				if (data->GetHasVertexColors() != hasVFOnShader) {
					Log::Info("Block[%i]: Has Vertex Colors flag in NiTriShapeData must match 'Vertex Colors' in BSLightingShaderProperty", i);
				}
				if ((data->GetHasVertexColors()) && shape->GetShaderProperty() != NULL && (shape->GetShaderProperty()->GetShaderFlags2() & SkyrimShaderPropertyFlags2::SLSF2_TREE_ANIM) != SkyrimShaderPropertyFlags2::SLSF2_TREE_ANIM) {
					vector<Color4> vc = data->GetVertexColors();
					bool allWhite = true;
					for (int x = 0; x != vc.size(); x++) {
						if (vc[x].r != 1.0f || vc[x].g != 1.0f || vc[x].b != 1.0f || vc[x].a != 1.0f) {
							allWhite = false;
							break;
						}
					}
					if (allWhite)
						Log::Info("Block[%i]: Redundant all white #FFFFFFFF vertex colors.", i);
				}
			}


			//scan if normals are incorrect
			//scan if NiAlphaProperty is below BSLightingShaderProperty
		}

		//check NiNode children names and if they are unique to their parent.
	}
}

static bool BeginScan() {
	Log::Info("Begin Scan");

	Games& games = Games::Instance();
	const Games::GamesPathMapT& installations = games.getGames();

	NifInfo info;

	vector<fs::path> nifs;

	findFilesn(games.data(Games::TES5) / "meshes", ".nif", nifs);

	if (nifs.empty())
	{
		Log::Info("No NIFs found. Checking BSAs.");

		for (const auto& bsa : games.bsas(Games::TES5)) {
			std::cout << "Scan: " << bsa.filename() << std::endl;

			BSAFile bsa_file(bsa);
			for (const auto& nif : bsa_file.assets(".*\.nif")) {
				Log::Info("Current File: %s", nif.c_str());

				size_t size = -1;
				const uint8_t* data = bsa_file.extract(nif, size);

				std::string sdata((char*)data, size);
				std::istringstream iss(sdata);
				try {
					vector<NiObjectRef> blocks = ReadNifList(iss, &info);
					ScanNif(blocks, info);
				}
				catch (const std::exception& e) {
					Log::Info("ERROR: %s", e.what());
				}
				delete data;
			}
		}

		return true;
	}
	else 
	{
		for (size_t i = 0; i < nifs.size(); i++) {
			Log::Info("Current File: %s", nifs[i].string().c_str());

			try {
				vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
				ScanNif(blocks, info);
			}
			catch(const std::exception& e) {
				Log::Info("ERROR: %s", e.what());
			}
		}
	}
	
	//if (write) {
	//	fs::path out_path = nif_err / bsa.filename() / (actual ^ calculated).to_string() / fs::path(to_string(calculated.to_ulong())) / nif;
	//	fs::create_directories(out_path.parent_path());
	//	WriteNifTree(out_path.string(), root, info);
	//}
}
