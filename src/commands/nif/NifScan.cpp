#include "stdafx.h"
#include <commands/NifScan.h>
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

using namespace Niflib;
using namespace std;

void BeginScan()
{
	Log::Info("Begin Scan");

	fs::path nif = nif_in / "LongSword.nif";

	NifInfo info;
	vector<NiObjectRef> blocks = ReadNifList(nif.string().c_str(), &info);

	for (int i = 0; i != blocks.size(); i++) {

		if (blocks[i]->IsSameType(BSXFlags::TYPE)) {
			if (DynamicCast<BSXFlags>(blocks[i])->GetName() != "BSX") {
				Log::Info("Block[%i]: BSXFlag blocks need to be named 'BSX'", i);
			}
		}

		if (blocks[i]->IsSameType(NiTriStrips::TYPE)) {
			Log::Info("Block[%i]: NiTriStrips needs to be triangulated.", i);
		}

		//not tested this.
		if (blocks[i]->IsSameType(NiSkinPartition::TYPE)) {
			for (SkinPartition partition : DynamicCast<NiSkinPartition>(blocks[i])->GetPartition()) {
				if (partition.numStrips > 0)
					Log::Info("Block[%i]: NiSkinPartition contains strips. (Obsolete in SSE)", i);
			}
		}

		if (blocks[i]->IsSameType(BSLightingShaderProperty::TYPE)) {
			BSLightingShaderPropertyRef  shaderprop = DynamicCast<BSLightingShaderProperty>(blocks[i]);

			if (shaderprop->GetSkyrimShaderType() == (BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP) || (BSLightingShaderPropertyShaderType::ST_EYE_ENVMAP)) {
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) != SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
					Log::Info("Block[%i]: ShaderType is 'Environment', but ShaderFlags1 does not include 'Environment Mapping'.", i);
				}
				if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) == SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
					Log::Info("Block[%i]: ShaderType is 'Environment', but ShaderFlags 2 has enabled 'glow' flag.", i);
				}
				if (shaderprop->GetTextureSet()->GetTextures().size() >= 5) {
					if (shaderprop->GetTextureSet()->GetTextures()[4] != "") {
						Log::Info("Block[%i]: ShaderType is 'Environment', but no 'Environment' texture is present.", i);
					}
				}
				else {
					Log::Info("Block[%i]: TextureSet size is too small to include 'Environment' texture.", i);
				}
			}
		}

		if (blocks[i]->IsSameType(NiTriShape::TYPE)) {
			NiTriShapeRef shape = DynamicCast<NiTriShape>(blocks[i]);
			NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(shape->GetData());

			if ((data->GetHasVertexColors()) && (shape->GetShaderProperty()->GetShaderFlags2() & SkyrimShaderPropertyFlags2::SLSF2_TREE_ANIM) != SkyrimShaderPropertyFlags2::SLSF2_TREE_ANIM) {
				vector<Color4> vc = data->GetVertexColors();
				bool allWhite = true;
				for (int i = 0; i != vc.size(); i++) {
					if (vc[i].r != 1.0f || vc[i].g != 1.0f || vc[i].b != 1.0f) {
						allWhite = false;
					}
				}
				if (allWhite)
					Log::Info("Block[%i]: Redundant all white #FFFFFFFF vertex colors.", i);
			}


			//scan if normals are incorrect
			//scan if NiAlphaProperty is below BSLightingShaderProperty
			//scan to check if vertex is checked AND shader flag is.
		}

		//check if mesh uses SLSF1_External_Emittance and then if bit 9 is set.
		//check if textures exist.
		//check InvMarker and EditorMarker
		//check NiNode children names and if they are unique to their parent.
	}
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("About - NifScan"); break;
	case hkxcmd::htLong: {
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s about", exeName);
		Log::Info("  NifScan");
	}
						 break;
	}
}

static bool ExecuteCmd(hkxcmdLine &cmdLine) {
	BeginScan();
	return true;
}

REGISTER_COMMAND(NifScan, HelpString, ExecuteCmd);
