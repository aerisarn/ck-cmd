#include "stdafx.h"
#include <commands/NifScan.h>
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>
#include <commands/Geometry.h>
#include <commands/DDS.h>

using namespace ckcmd;
using namespace ckcmd::nifscan;

using namespace Niflib;
using namespace std;

class RebuildVisitor;
class FixTargetsVisitor;

static bool BeginScan(string scanPath);

REGISTER_COMMAND_CPP(NifScan)

static Games& games = Games::Instance();
const Games::GamesPathMapT& installations = games.getGames();

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

	// Usage: ck-cmd nifscan [-i <path_to_scan>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [-i <path_to_scan>]\r\n";

	const char help[] =
		R"(Scan Skyrim meshes for any errors.
		
		Arguments:
			<path_to_scan> path to models you want to check for errors)";

    return usage + help;
}

string NifScan::GetHelpShort() const
{
    return "TODO: Short help message for ConvertNif";
}

class RebuildVisitor : public RecursiveFieldVisitor<RebuildVisitor> {
	set<NiObject*> objects;
public:
	vector<NiObjectRef> blocks;

	RebuildVisitor(NiObject* root, const NifInfo& info) :
		RecursiveFieldVisitor(*this, info) {
		root->accept(*this, info);

		for (NiObject* ptr : objects) {
			blocks.push_back(ptr);
		}
	}


	template<class T>
	inline void visit_object(T& obj) {
		objects.insert(&obj);
	}


	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<class T>
	inline void visit_field(T& obj) {}
};

class FixTargetsVisitor : public RecursiveFieldVisitor<FixTargetsVisitor> {
	vector<NiObjectRef>& blocks;
public:


	FixTargetsVisitor(NiObject* root, const NifInfo& info, vector<NiObjectRef>& blocks) :
		RecursiveFieldVisitor(*this, info), blocks(blocks) {
		root->accept(*this, info);
	}


	template<class T>
	inline void visit_object(T& obj) {}

	template<>
	inline void visit_object(NiDefaultAVObjectPalette& obj) {
		vector<AVObject > av_objects = obj.GetObjs();
		for (AVObject& av_object : av_objects) {
			for (NiObjectRef ref : blocks) {
				if (ref->IsDerivedType(NiAVObject::TYPE)) {
					NiAVObjectRef av_ref = DynamicCast<NiAVObject>(ref);
					if (av_ref->GetName() == av_object.name) {
						av_object.avObject = DynamicCast<NiAVObject>(av_ref);
					}
				}
			}
		}
		obj.SetObjs(av_objects);
	}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<>
	void visit_compound(AVObject& avlink) {
		//relink av objects on converted nistrips;

	}

	template<class T>
	inline void visit_field(T& obj) {}
};


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

static int CheckDDS(fs::path filename, int slot, bool hasAlpha, bool isSpecular)
{
	ifstream is;
	int magic;
	DDS_HEADER header = {};

	is.open(filename, ios::binary);

	if (is.is_open())
	{
		is.read((char*)&magic, sizeof(int));

		if (magic != DDS_MAGIC)
			return -1;

		is.read((char*)&header.dwSize, sizeof(DWORD));
		is.read((char*)&header.dwFlags, sizeof(DWORD));
		is.read((char*)&header.dwHeight, sizeof(DWORD));
		is.read((char*)&header.dwWidth, sizeof(DWORD));
		is.read((char*)&header.dwPitchOrLinearSize, sizeof(DWORD));
		is.read((char*)&header.dwDepth, sizeof(DWORD));
		is.read((char*)&header.dwMipMapCount, sizeof(DWORD));
		is.read((char*)&header.dwReserved1, sizeof(DWORD)*11);
		is.read((char*)&header.ddspf.dwSize, sizeof(DWORD));
		is.read((char*)&header.ddspf.dwFlags, sizeof(DWORD));
		is.read((char*)&header.ddspf.dwFourCC, sizeof(DWORD)); //DXT1, DXT3 or DXT5
		is.close();

		if (header.ddspf.dwFourCC == DDS_DXT3)
			Log::Error("DXT3 is not used in Skyrim.");

		if (header.dwMipMapCount == 0)
			Log::Error("No mipmaps found. Mipmaps should be generated for optimisation.");

		if (slot == 0) {
			if (header.ddspf.dwFourCC == DDS_DXT1)
			{
				if (hasAlpha)
					Log::Error("Block has alpha but diffuse texture is DXT1. Needs to be DXT5.");
			}
			else if (header.ddspf.dwFourCC == DDS_DXT5)
			{
				if (!hasAlpha)
				{
					Log::Error("Block does not have alpha but diffuse texture is DXT5. Needs to be DXT1.");
					return 1;
				}
			}
		}
		if (slot == 1) {
			if (header.ddspf.dwFourCC == DDS_DXT1)
			{
				if (isSpecular)
					Log::Error("Block has specular flag but normal texture is DXT1. Needs to be DXT5.");
			}
			else if (header.ddspf.dwFourCC == DDS_DXT5)
			{
				if (!isSpecular)
					Log::Error("Block does not have specular flag but normal texture is DXT5. Needs to be DXT1.");
			}
		}
		if (slot == 4 || slot == 5) {
			if (header.ddspf.dwFourCC != DDS_DXT1)
				Log::Error("Environment/Cube map is needs to be DXT1.");
		}
	}
	return 0;
}

NiTriShapeRef triangulate_strip(NiTriStripsRef& stripsRef)
{
	//Convert NiTriStrips to NiTriShapes first of all.
	NiTriShapeRef shapeRef = new NiTriShape();
	shapeRef->SetName(stripsRef->GetName());
	shapeRef->SetExtraDataList(stripsRef->GetExtraDataList());
	shapeRef->SetTranslation(stripsRef->GetTranslation());
	shapeRef->SetRotation(stripsRef->GetRotation());
	shapeRef->SetScale(stripsRef->GetScale());
	shapeRef->SetFlags(524302);
	shapeRef->SetData(stripsRef->GetData());
	shapeRef->SetShaderProperty(stripsRef->GetShaderProperty());
	shapeRef->SetProperties(stripsRef->GetProperties());
	shapeRef->SetAlphaProperty(stripsRef->GetAlphaProperty());

	NiTriStripsDataRef stripsData = DynamicCast<NiTriStripsData>(stripsRef->GetData());
	NiTriShapeDataRef shapeData = new  NiTriShapeData();

	shapeData->SetHasVertices(stripsData->GetHasVertices());
	shapeData->SetVertices(stripsData->GetVertices());
	shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(stripsData->GetVectorFlags()));
	shapeData->SetUvSets(stripsData->GetUvSets());
	if (!shapeData->GetUvSets().empty())
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(shapeData->GetBsVectorFlags() | BSVF_HAS_UV));
	shapeData->SetCenter(stripsData->GetCenter());
	shapeData->SetRadius(stripsData->GetRadius());
	shapeData->SetHasVertexColors(stripsData->GetHasVertexColors());
	shapeData->SetVertexColors(stripsData->GetVertexColors());
	shapeData->SetConsistencyFlags(stripsData->GetConsistencyFlags());
	vector<Triangle> triangles = Geometry::triangulate(stripsData->GetPoints());
	shapeData->SetNumTriangles(triangles.size());
	shapeData->SetNumTrianglePoints(triangles.size() * 3);
	shapeData->SetHasTriangles(1);
	shapeData->SetTriangles(triangles);

	shapeData->SetHasNormals(stripsData->GetHasNormals());
	shapeData->SetNormals(stripsData->GetNormals());

	vector<Vector3> vertices = shapeData->GetVertices();
	Vector3 COM;
	if (vertices.size() != 0)
		COM = (COM / 2) + (ckcmd::Geometry::centeroid(vertices) / 2);
	vector<Triangle> faces = shapeData->GetTriangles();
	vector<Vector3> normals = shapeData->GetNormals();
	if (vertices.size() != 0 && faces.size() != 0 && shapeData->GetUvSets().size() != 0) {
		vector<TexCoord> uvs = shapeData->GetUvSets()[0];
		ckcmd::Geometry::TriGeometryContext g(vertices, COM, faces, uvs, normals);
		shapeData->SetHasNormals(1);
		//recalculate
		shapeData->SetNormals(g.normals);
		shapeData->SetTangents(g.tangents);
		shapeData->SetBitangents(g.bitangents);
		if (vertices.size() != g.normals.size() || vertices.size() != g.tangents.size() || vertices.size() != g.bitangents.size())
			throw runtime_error("Geometry mismatch!");
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(shapeData->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
	}
	else {
		shapeData->SetTangents(stripsData->GetTangents());
		shapeData->SetBitangents(stripsData->GetBitangents());
	}

	shapeRef->SetData(DynamicCast<NiGeometryData>(shapeData));

	//TODO: shared normals no more supported
	shapeData->SetMatchGroups(vector<MatchGroup>{});

	shapeRef->SetSkin(stripsRef->GetSkin());
	shapeRef->SetSkinInstance(stripsRef->GetSkinInstance());

	return shapeRef;
}

void ScanNif(vector<NiObjectRef>& blocks, NifInfo info)
{
	NiObjectRef root = GetFirstRoot(blocks);

	bsx_flags_t calculated = calculateSkyrimBSXFlags(blocks, info);
	bsx_flags_t actual;

	bool write = false;
	bool hasVFOnShader = false;
	vector<string> names = vector<string>();

	for (int i = 0; i != blocks.size(); i++) {

		if (blocks[i]->IsSameType(NiSkinPartition::TYPE)) {
			for (SkinPartition partition : DynamicCast<NiSkinPartition>(blocks[i])->GetPartition()) {
				if (partition.numStrips > 0)
					Log::Error("Block[%i]: NiSkinPartition contains strips. (Obsolete in SSE)", i);
			}
		}

		if (blocks[i]->IsDerivedType(NiTimeController::TYPE)) {
			NiTimeControllerRef ref = DynamicCast<NiTimeController>(blocks[i]);
			if (ref->GetTarget() == NULL) {
				Log::Error("Block[%i]: Controller has no target. This will increase the chances of a crash.", i);
			}
		}

		if (blocks[i]->IsDerivedType(NiControllerSequence::TYPE)) {
			NiSequenceRef ref = DynamicCast<NiSequence>(blocks[i]);

			if (ref->GetControlledBlocks().size() != 0) {
				vector<ControlledBlock> blocks = ref->GetControlledBlocks();

				for (int y = 0; y != blocks.size(); y++) {
					if (blocks[y].controllerType == "") {
						Log::Error("Block[%i]: ControlledBlock number %i, has a blank controller type.", i, y);
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
			if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS) == SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS && !hasVFOnShader) {
				//Log::Error("Block[%i]: 'Has Vertex Colors' in NiTriShapeData must match 'Vertex Colors' in BSLightingShaderProperty flags", i);
			}
			if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS) != SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS && hasVFOnShader) {
				//Log::Error("Block[%i]: 'Has Vertex Colors' in NiTriShapeData must match 'Vertex Colors' in BSLightingShaderProperty flags", i);
			}
			hasVFOnShader = false; //Maybe this will fix issues
			if (shaderprop->GetSkyrimShaderType() == BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP /*|| BSLightingShaderPropertyShaderType::ST_EYE_ENVMAP*/) {
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) != SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
					Log::Error("Block[%i]: ShaderType is 'Environment', but ShaderFlags1 does not include 'Environment Mapping'.", i);
				}
				if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) == SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
					Log::Error("Block[%i]: ShaderType is 'Environment', but ShaderFlags 2 has enabled 'glow' flag.", i);
				}
				if (shaderprop->GetTextureSet()->GetTextures().size() >= 5) {
					if (shaderprop->GetTextureSet()->GetTextures()[4] == "") {
						Log::Error("Block[%i]: ShaderType is 'Environment', but no 'Environment' texture is present.", i);
					}
				}
				else {
					Log::Error("Block[%i]: TextureSet size is too small to include 'Environment' texture.", i);
				}
			}
			if (shaderprop->GetSkyrimShaderType() == BSLightingShaderPropertyShaderType::ST_GLOW_SHADER) {
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) != SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) {
					Log::Error("Block[%i]: ShaderType is 'Glow', but ShaderFlags1 does not include 'External Emittance'.", i);
				}
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) == SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
					Log::Error("Block[%i]: ShaderType is 'Glow', but ShaderFlags1 includes 'Environment Mapping'.", i);
				}
				if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) != SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
					Log::Error("Block[%i]: ShaderType is 'Glow', but ShaderFlags2 does not include 'Glow Map'.", i);
				}
				if (shaderprop->GetTextureSet()->GetTextures().size() > 3) {
					if (shaderprop->GetTextureSet()->GetTextures()[2] == "") {
						Log::Error("Block[%i]: ShaderType is 'Glow', but no 'Glow' texture is present.", i);
					}
				}
				else {
					Log::Error("Block[%i]: TextureSet size is too small to include 'Glow' texture.", i);
				}
			}
			if (shaderprop->GetSkyrimShaderType() == BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP) {
				if ((shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) != SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
					Log::Error("Block[%i]: ShaderType is 'Environment', but ShaderFlags1 does not include 'Environment Mapping'.", i);
				}
				if ((shaderprop->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) == SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
					Log::Error("Block[%i]: ShaderType is 'Environment', but ShaderFlags2 includes 'Glow Map'.", i);
				}
				if ((shaderprop->GetEnvironmentMapScale() == 0)) {
					Log::Error("Block[%i]: ShaderType is 'Environment', but map scale equals 0, making it obsolete.", i);
				}
				if (shaderprop->GetTextureSet()->GetTextures().size() > 5) {
					if (shaderprop->GetTextureSet()->GetTextures()[4] == "") {
						Log::Error("Block[%i]: ShaderType is 'Environment', but no 'Cube map' texture is present.", i);
					}
					if (shaderprop->GetTextureSet()->GetTextures()[5] == "") {
						Log::Error("Block[%i]: ShaderType is 'Environment', but no 'mask' texture is present.", i);
					}
				}
				else {
					Log::Error("Block[%i]: TextureSet size is too small to include 'Environment' textures.", i);
				}
			}
		}

		if (blocks[i]->IsSameType(NiTriShape::TYPE)) {
			bool hasAlpha = false;
			bool isSpecular = false;

			NiTriShapeRef shape = DynamicCast<NiTriShape>(blocks[i]);
			NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(shape->GetData());
			BSLightingShaderPropertyRef  shaderprop = DynamicCast<BSLightingShaderProperty>(shape->GetShaderProperty());

			if (data != NULL && shape != NULL && shaderprop != NULL) {
				if (shape->GetAlphaProperty() != NULL)
					hasAlpha = true;

				if (shaderprop->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_SPECULAR)
					isSpecular = true;

				if (data->GetHasVertexColors()) {
					hasVFOnShader = true;
				}
			}
		}
	}
}

void visitNode(NiNodeRef obj)
{
	vector<Ref<NiAVObject>> children = obj->GetChildren();
	vector<Ref<NiProperty>> properties = obj->GetProperties();
	vector<Ref<NiExtraData>> extraDatas = obj->GetExtraDataList();
	vector<Ref<NiAVObject>>::iterator eraser = children.begin();
	while (eraser != children.end())
	{
		if (*eraser == NULL) {
			eraser = children.erase(eraser);
		}
		else
			eraser++;
	}
	vector<Ref<NiExtraData>>::iterator eraser2 = extraDatas.begin();
	while (eraser2 != extraDatas.end())
	{
		if (*eraser2 == NULL) {
			eraser2 = extraDatas.erase(eraser2);
		}
		else
			eraser2++;
	}

	for (NiExtraDataRef& extra : extraDatas)
	{
		if (extra->IsSameType(BSInvMarker::TYPE)) {
			if (DynamicCast<BSInvMarker>(extra)->GetName() != "INV") {
				Log::Error("Detected BSInvMarker type but name was not \"INV\"");
				DynamicCast<BSInvMarker>(extra)->SetName(IndexString("INV"));
			}
		}
		else if (extra->IsSameType(BSFurnitureMarker::TYPE)) {
			if (DynamicCast<BSFurnitureMarker>(extra)->GetName() != "FRN") {
				Log::Error("Detected BSFurnitureMarker type but name was not \"FRN\"");
				DynamicCast<BSFurnitureMarker>(extra)->SetName(IndexString("FRN"));
			}
		}
		else if (extra->IsSameType(BSBound::TYPE)) {
			if (DynamicCast<BSBound>(extra)->GetName() != "BBX") {
				Log::Error("Detected BSBound type but name was not \"BBX\"");
				DynamicCast<BSBound>(extra)->SetName(IndexString("BBX"));
			}
		}
		else if (extra->IsSameType(BSBehaviorGraphExtraData::TYPE)) {
			if (DynamicCast<BSBehaviorGraphExtraData>(extra)->GetName() != "BGED") {
				Log::Error("Detected BSBehaviorGraphExtraData type but name was not \"BGED\"");
				DynamicCast<BSBehaviorGraphExtraData>(extra)->SetName(IndexString("BGED"));
			}
		}
		else if (extra->IsSameType(BSBoneLODExtraData::TYPE)) {
			if (DynamicCast<BSBoneLODExtraData>(extra)->GetName() != "BSBoneLOD") {
				Log::Error("Detected BSBoneLOD type but name was not \"BSBoneLOD\"");
				DynamicCast<BSBoneLODExtraData>(extra)->SetName(IndexString("BSBoneLOD"));
			}
		}
		else if (extra->IsSameType(BSXFlags::TYPE)) {
			BSXFlagsRef ref = DynamicCast<BSXFlags>(extra);
			if (ref->GetName() != "BSX") {
				Log::Error("Detected BSXFlags type but name was not \"BSX\"");
				ref->SetName(IndexString("BSX"));
			}
		}
	}

	int index = 0;
	for (NiAVObjectRef& block : children)
	{
		if (block->IsSameType(NiTriStrips::TYPE)) {
			NiTriStripsRef stripsRef = DynamicCast<NiTriStrips>(block);
			NiTriShapeRef shape = triangulate_strip(stripsRef);
			children[index] = shape;
		}

		index++;
	}

	for (NiAVObjectRef& block : children)
	{
		if (block->IsSameType(NiTriShape::TYPE)) {
			bool hasStrips = false;
			NiTriShapeRef shape = DynamicCast<NiTriShape>(block);
			NiSkinInstanceRef skin = shape->GetSkinInstance();
			if (skin != NULL) {
				NiSkinDataRef iSkinData = skin->GetData();
				NiSkinPartitionRef iSkinPart = skin->GetSkinPartition();

				if (iSkinPart == NULL)
					iSkinPart = iSkinData->GetSkinPartition();
				if (iSkinPart != NULL)
				{
					vector<SkinPartition >& pblocks = iSkinPart->GetSkinPartitionBlocks();
					for (const auto& pb : pblocks)
					{
						if (pb.strips.size() > 0)
						{
							hasStrips = true;
							break;
						}
					}
				}
			}

			if (hasStrips) {
				//redo partitions destripping
				NiTriBasedGeomRef geo = StaticCast<NiTriBasedGeom>(shape);
				int bb = 60;
				int bv = 4;
				//repartitioner.cast(nif, iBlock, bb, bv, false, false);
				remake_partitions(geo, bb, bv, false, false);
			}

			BSLightingShaderPropertyRef lp = DynamicCast<BSLightingShaderProperty>(shape->GetShaderProperty());

			if (shape != NULL && lp != NULL)
			{
				NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(shape->GetData());

				if (data != NULL)
				{
					//forcefully recalulcator tangents for now.. 
					vector<Vector3> vertices = data->GetVertices();
					//lets recalculate center;
					Vector3 center;
					for (const Vector3& v : vertices) {
						center += v;
					}
					center /= vertices.size();

					//and then rad;
					float radius = 0.0f;
					for (const Vector3& v : vertices) {
						float z;
						if ((z = (center - v).Magnitude()) > radius)
							radius = z;
					}
					Vector3 COM;
					if (vertices.size() != 0)
						COM = (COM / 2) + (ckcmd::Geometry::centeroid(vertices) / 2);
					vector<Triangle> faces = data->GetTriangles();
					vector<Vector3> normals = data->GetNormals();
					if (vertices.size() != 0 && faces.size() != 0 && data->GetUvSets().size() != 0) {
						vector<TexCoord> uvs = data->GetUvSets()[0];

						Geometry::TriGeometryContext g(vertices, COM, faces, uvs, normals);
						data->SetHasNormals(1);
						data->SetNormals(g.normals);
						data->SetTangents(g.tangents);
						data->SetBitangents(g.bitangents);
					}
					data->SetBsVectorFlags(static_cast<BSVectorFlags>(data->GetBsVectorFlags() | BSVectorFlags::BSVF_HAS_TANGENTS));
					data->SetCenter(center);
					data->SetRadius(radius);
				}
				else
				{
					Log::Warn("NiTriShape block with no NiTriShapeData attached!");
				}

				vector<Color4> vc = data->GetVertexColors();
				bool allWhite = true;
				for (int x = 0; x != vc.size(); x++) {
					if (vc[x].r != 1.0f || vc[x].g != 1.0f || vc[x].b != 1.0f || vc[x].a != 1.0f) {
						allWhite = false;
						break;
					}
				}
				if (allWhite)
				{
					//remove bloat.
					data->SetHasVertexColors(false);
					data->SetVertexColors(vector<Color4>());
					lp->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lp->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
				}

				bool hasAlpha = false, isSpecular = false;
				if (shape->GetAlphaProperty() != NULL)
					hasAlpha = true;

				if (lp->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_SPECULAR)
					isSpecular = true;

				short errorCount = 0;
				if (lp->GetSkyrimShaderType() == BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP) {
					if ((lp->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) != SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING) {
						Log::Error("ShaderType is 'Environment', but ShaderFlags1 does not include 'Environment Mapping'.");
						errorCount++;
					}
					if ((lp->GetShaderFlags2_sk() & SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) == SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP) {
						Log::Error("ShaderType is 'Environment', but ShaderFlags2 includes 'Glow Map'.");
						errorCount++;
					}
					if ((lp->GetEnvironmentMapScale() == 0)) {
						Log::Error("ShaderType is 'Environment', but map scale equals 0, making it obsolete.");
						errorCount++;
					}
					if (lp->GetTextureSet()->GetTextures().size() > 5) {
						if (lp->GetTextureSet()->GetTextures()[4] == "") {
							Log::Error("ShaderType is 'Environment', but no 'Cube map' texture is present.");
							errorCount++;
						}
						if (lp->GetTextureSet()->GetTextures()[5] == "") {
							Log::Error("ShaderType is 'Environment', but no 'mask' texture is present.");
							errorCount++;
						}
					}
					else {
						Log::Error("TextureSet size is too small to include 'Environment' textures.");
					}
				}

				if (errorCount > 2) {
					lp->SetSkyrimShaderType(BSLightingShaderPropertyShaderType::ST_DEFAULT);
				}

				BSShaderTextureSetRef set = DynamicCast<BSShaderTextureSet>(lp->GetTextureSet());
				if (set->GetTextures().size() != 0) {
					stringvector textures = set->GetTextures();
					for (int i = 0; i != textures.size(); i++) {
						bool isTexture = (textures[i].substr(0, 7) != "textures"); //Possible issue: If the user has capitalized "textures"#
						if (!isTexture) {
							Log::Error("Block[%i]: TextureSet includes paths not relative to data.", i);
						}
						else {
							bool doesExist = false;
							if (textures[i] != "") {
								if ((fs::exists(games.data(Games::TES5SE) / textures[i])) || (fs::exists(games.data(Games::TES5SE) / "textures" / textures[i]))) { //this needs sorting out.
									Log::Info("Checking texture data of %s", (textures[i]).c_str());
									if (CheckDDS(games.data(Games::TES5SE) / textures[i], i, hasAlpha, isSpecular) == 1)
										shape->SetAlphaProperty(new NiAlphaProperty());
									doesExist = true;
								}
								if (!doesExist)
									Log::Error("Block[%i]: Texture: '%s' does not exist!", i, textures[i].c_str());
							}
						}
					}
				}

				shape->SetData(DynamicCast<NiGeometryData>(data));
			}
		}
	}

	obj->SetChildren(children);
}

vector<NiObjectRef> checkNif(vector<NiObjectRef> blocks, NifInfo info) {

	NiObjectRef root = GetFirstRoot(blocks);

	for (auto& block : blocks) {
		//if (block->IsSameType(BSXFlags::TYPE)) {
		//	BSXFlagsRef ref = DynamicCast<BSXFlags>(block);
		//	bsx_flags_t actual = ref->GetIntegerData();
		//	bsx_flags_t calculated = calculateSkyrimBSXFlags(blocks, info);
		//	if (actual != calculated.to_ulong()) {
		//		Log::Info("BSXFlag: value: [%d %s], estimate: [%d %s]", actual.to_ulong(), actual.to_string().c_str(), calculated.to_ulong(), calculated.to_string().c_str());
		//		ref->SetIntegerData(calculated.to_ulong());
		//	}
		//}
		if (block->IsDerivedType(NiNode::TYPE))
		{
			visitNode(DynamicCast<NiNode>(block));
		}
	}

	//to calculate the right flags, we need to rebuild the blocks
	vector<NiObjectRef> new_blocks = RebuildVisitor(root, info).blocks;

	//fix targets from nitrishapes substitution
	FixTargetsVisitor(GetFirstRoot(new_blocks), info, new_blocks);

	return move(new_blocks);
}

static bool BeginScan(string scanPath) {
	Log::Info("Begin Scan");

	NifInfo info;

	vector<fs::path> nifs;

	//findFilesn(games.data(Games::TES5SE) / "meshes/tes4", ".nif", nifs);
	//fs::path nif_in = "D:\\git\\ck-cmd\\resources\\in";
	findFilesn(scanPath, ".nif", nifs);
	bool write = true;
	fs::path nif_path = "F:\\FIXED_NIFS";

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
			fs::path out = nifs[i].parent_path() / fs::path("out") / nifs[i].filename();
			fs::path parent_out = out.parent_path();
			if (fs::exists(out))
				continue;

			if (nifs[i].string().find("\\out\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("meshes\\landscape\\lod") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\marker_") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\minotaurold") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\sky\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\menus\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\Creatures\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\amulet\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\armor\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\weapons\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\effects\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\sky\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\menus\\") != string::npos) {
				continue;
			}
			if (nifs[i].string().find("\\creatures\\") != string::npos) {
				continue;
			}
			Log::Info("Current File: %s", nifs[i].string().c_str());
			NifInfo info;
			try {
				vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
				vector<NiObjectRef> new_blocks = checkNif(blocks, info);
				Log::Info("Output File: %s", out.string().c_str());
				if (!fs::exists(parent_out))
					fs::create_directories(parent_out);
				WriteNifTree(out.string(), GetFirstRoot(new_blocks), info);
			}
			catch (const std::exception& e) {
				Log::Info("ERROR: %s", e.what());
			}
		}
		Log::Info("Done..");
	}
}

bool NifScan::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string scanPath;

	scanPath = parsedArgs["<path_to_scan>"].asString();

	bool result = BeginScan(scanPath);
	Log::Info("NifScan Ended");
	return result;
}