#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/ConvertNif.h>
#include <core/games.h>
#include <core/bsa.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;

using namespace Niflib;
using namespace std;

//#include "ConvertNif.h"
//#include <core/hkxcmd.h>
//#include <core/log.h>

BSFadeNodeRef convert_root(NiObjectRef root)
{
	NiNodeRef rootRef = DynamicCast<NiNode>(root);
	BSFadeNodeRef fadeNode = new BSFadeNode();
	fadeNode->SetName(rootRef->GetName());
	fadeNode->SetExtraDataList(rootRef->GetExtraDataList());
	fadeNode->SetFlags(524302);
	fadeNode->SetController(rootRef->GetController());
	fadeNode->SetCollisionObject(rootRef->GetCollisionObject());
	fadeNode->SetChildren(rootRef->GetChildren());
	return fadeNode;
}
vector<Triangle> triangulate(vector<unsigned short> strip)
{
	vector<Triangle> tris;
	unsigned short a, b = strip[0], c = strip[1];
	bool flip = false;

	for (int s = 2; s < strip.size(); s++) {
		a = b;
		b = c;
		c = strip[s];

		if (a != b && b != c && c != a) {
			if (!flip)
				tris.push_back(Triangle(a, b, c));
			else
				tris.push_back(Triangle(a, c, b));
		}

		flip = !flip;
	}

	return tris;
}
vector<Triangle> triangulate(vector<vector<unsigned short>> strips)
{
	vector<Triangle> tris;
	for (const vector<unsigned short>& strip : strips)
	{
		vector<Triangle> these_tris = triangulate(strip);
		tris.insert(tris.end(), these_tris.begin(), these_tris.end());
	}
	return tris;
}
Vector3 centeroid(const vector<Vector3>& in) {
	Vector3 centeroid = Vector3(0.0, 0.0, 0.0);
	for (Vector3 vertex : in) {
		centeroid += vertex;
	}
	centeroid = Vector3(centeroid.x / in.size(), centeroid.y / in.size(), centeroid.z / in.size()).Normalized();
	return centeroid;
}

NiTriShapeRef convert_strip(NiTriStripsRef& stripsRef)
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

	//Then do the data..
	bool hasAlpha = false;

	NiTriStripsDataRef stripsData = DynamicCast<NiTriStripsData>(stripsRef->GetData());
	NiTriShapeDataRef shapeData = new  NiTriShapeData();

	shapeData->SetHasVertices(stripsData->GetHasVertices());
	shapeData->SetVertices(stripsData->GetVertices());
	shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(4097));
	shapeData->SetUvSets(stripsData->GetUvSets());
	shapeData->SetCenter(stripsData->GetCenter());
	shapeData->SetRadius(stripsData->GetRadius());
	shapeData->SetHasVertexColors(stripsData->GetHasVertexColors());
	shapeData->SetVertexColors(stripsData->GetVertexColors());
	shapeData->SetConsistencyFlags(stripsData->GetConsistencyFlags());
	vector<Triangle> triangles = triangulate(stripsData->GetPoints());
	shapeData->SetNumTriangles(triangles.size());
	shapeData->SetNumTrianglePoints(triangles.size() * 3);
	shapeData->SetHasTriangles(1);
	shapeData->SetTriangles(triangles);

	////Tangents, Bitangents and Normals
	//vector<Vector3> vertices = shapeData->GetVertices();
	//Vector3 COM;
	//if (vertices.size() != 0)
	//	COM = (COM / 2) + (centeroid(vertices) / 2);
	//vector<Triangle> faces = shapeData->GetTriangles();
	//vector<Vector3> normals = shapeData->GetNormals();
	//if (vertices.size() != 0 && faces.size() != 0 && shapeData->GetUvSets().size() != 0) {
	//	vector<TexCoord> uvs = shapeData->GetUvSets()[0];
	//	//Tangent Space
	//	//TriGeometryContext g(vertices, COM, faces, uvs, normals);
	//	//shapeData->SetHasNormals(1);
	//	//shapeData->SetNormals(normals);
	//	//shapeData->SetTangents(g.tangents);
	//	//shapeData->SetBitangents(g.bitangents);
	//}

	shapeRef->SetData(DynamicCast<NiGeometryData>(shapeData));

	return shapeRef;
}

class ConverterVisitor : public RecursiveFieldVisitor<ConverterVisitor> {
public:
	ConverterVisitor(const NifInfo& info) :
		RecursiveFieldVisitor(*this, info)
	{}
	template<class T>
	inline void visit_object(T& obj) {}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<class T>
	inline void visit_field(T& obj) {}

	template<>
	inline void visit_object(NiNode& obj) {
		vector<Ref<NiAVObject>> children = obj.GetChildren();
		int index = 0;
		for (NiAVObjectRef& block : children)
		{
			if (block == NULL) {
				children.erase(children.begin() + index);
				continue;
			}
			if (block->IsSameType(NiTriStrips::TYPE)) {
				NiTriStripsRef stripsRef = DynamicCast<NiTriStrips>(block);
				if (stripsRef->IsSameType(NiTriStrips::TYPE)) {
					NiTriShapeRef shape = convert_strip(stripsRef);
					children[index] = shape;
				}
			}

			index++;
		}
		obj.SetChildren(children);
	}

	template<>
	inline void visit_object(NiTriShape& obj) {
		bool hasAlpha = false;

		BSLightingShaderPropertyRef lightingProperty = new BSLightingShaderProperty();
		BSShaderTextureSetRef textureSet = new BSShaderTextureSet();
		NiMaterialPropertyRef material = new NiMaterialProperty();
		NiTexturingPropertyRef texturing = new NiTexturingProperty();
		vector<Ref<NiProperty>> properties = obj.GetProperties();

		for (NiPropertyRef property : properties)
		{
			if (property->IsSameType(NiMaterialProperty::TYPE)) {
				material = DynamicCast<NiMaterialProperty>(property);
				lightingProperty->SetShaderType(BSShaderType::SHADER_DEFAULT);
				lightingProperty->SetEmissiveColor(material->GetEmissiveColor());
				lightingProperty->SetSpecularColor(material->GetSpecularColor());
				lightingProperty->SetEmissiveMultiple(1);
				lightingProperty->SetGlossiness(material->GetGlossiness());
				lightingProperty->SetAlpha(material->GetAlpha());
			}
			if (property->IsSameType(NiTexturingProperty::TYPE)) {
				texturing = DynamicCast<NiTexturingProperty>(property);
				string textureName;
				textureName += texturing->GetBaseTexture().source->GetFileName();
				textureName.insert(9, "tes4\\");
				string textureNormal = textureName;
				textureNormal.erase(textureNormal.end() - 4, textureNormal.end());
				textureNormal += "_n.dds";

				//setup textureSet (TODO)
				std::vector<std::string> textures(9);
				textures[0] = textureName;
				textures[1] = textureNormal;

				//finally set them.
				textureSet->SetTextures(textures);
			}
			if (property->IsSameType(NiStencilProperty::TYPE)) {
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() + SkyrimShaderPropertyFlags2::SLSF2_DOUBLE_SIDED));
			}
			if (property->IsSameType(NiAlphaProperty::TYPE)) {
				obj.SetAlphaProperty(new NiAlphaProperty());
			}
		}
		lightingProperty->SetTextureSet(textureSet);
		obj.SetShaderProperty(DynamicCast<BSShaderProperty>(lightingProperty));
		obj.SetExtraDataList(vector<Ref<NiExtraData>> {});
		obj.SetProperties(vector<Ref<NiProperty>> {});
	}

	template<>
	inline void visit_object(NiControllerSequence& obj)
	{
		vector<ControlledBlock> blocks = obj.GetControlledBlocks();
		vector<ControlledBlock> nblocks;

		//for some reason, oblivion's NIF blocks have empty NiTransforms, time to remove.
		for (int i = 0; i != blocks.size(); i++) {
			if (DynamicCast<NiTransformInterpolator>(blocks[i].interpolator)->GetData() == NULL)
				continue;

			blocks[i].nodeName = blocks[i].stringPalette->GetPalette().palette.substr(blocks[i].nodeNameOffset);
			blocks[i].controllerType = blocks[i].stringPalette->GetPalette().palette.substr(blocks[i].controllerTypeOffset);

			//set to default... if above doesn't work
			if (blocks[i].controllerType == "")
				blocks[i].controllerType = "NiTransformController";

			nblocks.push_back(blocks[i]);
		}
		obj.SetControlledBlocks(nblocks);
	}

	template<>
	inline void visit_object(NiTextKeyExtraData& obj)
	{	
		vector<Key<IndexString>> textKeys = obj.GetTextKeys();

		for (int i = 0; i != textKeys.size(); i++) {
			if (std::strstr(textKeys[i].data.c_str(), "Sound:")) {
				textKeys[i].data.insert(7, "TES4");
			}
		}

		obj.SetTextKeys(textKeys);
			
	}
};

void findFiles(fs::path startingDir, string extension, vector<fs::path>& results) {
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


bool BeginConversion() {
	char fullName[MAX_PATH], exeName[MAX_PATH];
	GetModuleFileName(NULL, fullName, MAX_PATH);
	_splitpath(fullName, NULL, NULL, exeName, NULL);

	NifInfo info;
	vector<fs::path> nifs;

	findFiles(nif_in, ".nif", nifs);

	if (nifs.empty()) {
		Log::Info("No NIFs found.. trying BSAs"); 
		
		Games& games = Games::Instance();
		const Games::GamesPathMapT& installations = games.getGames();

		for (const auto& bsa : games.bsas(Games::TES4)) {
			std::cout << "Checking: " << bsa.filename() << std::endl;
			BSAFile bsa_file(bsa);
			for (const auto& nif : bsa_file.assets(".*\.nif")) {
				Log::Info("Current File: %s", nif.c_str());

				vector<uint8_t> data(bsa_file.extract(nif));
				std::string sdata((char*)data.data(), data.size());
				std::istringstream iss(sdata);

				vector<NiObjectRef> blocks = ReadNifList(iss, &info);
				NiObjectRef root = GetFirstRoot(blocks);

				info.userVersion = 12;
				info.userVersion2 = 83;
				info.version = Niflib::VER_20_2_0_7;

				ConverterVisitor fimpl(info);
				root->accept(fimpl, info);

				fs::path out_path = nif_out / nif;
				fs::create_directories(out_path.parent_path());
				WriteNifTree(out_path.string(), root, info);
			}
		}	
	}
	else {

		for (size_t i = 0; i < nifs.size(); i++) {
			Log::Info("Current File: %s", nifs[i].string().c_str());

			vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
			NiObjectRef root = GetFirstRoot(blocks);

			info.userVersion = 12;
			info.userVersion2 = 83;
			info.version = Niflib::VER_20_2_0_7;

			ConverterVisitor fimpl(info);
			root->accept(fimpl, info);

			fs::path out_path = nif_out / nifs[i].filename();
			fs::create_directories(out_path.parent_path());
			WriteNifTree(out_path.string(), root, info);

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

static bool ExecuteCmd(hkxcmdLine &cmdLine) {
	BeginConversion();
	return true;
}

REGISTER_COMMAND(ConvertNif, HelpString, ExecuteCmd);