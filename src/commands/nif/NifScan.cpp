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

	fs::path nif = nif_in / "farmhouse06.nif";

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

		if (blocks[i]->IsSameType(NiTriShapeData::TYPE)) {
			//scan vertex for useless #FFFFFFFF
			//scan if normals are incorrect
			//scan if NiAlphaProperty is below BSLightingShaderProperty
			//scan to check if vertex is checked AND shader flag is.
			//scan environment mapping shader AND shader flag.
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
