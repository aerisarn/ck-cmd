#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/log.h>

#include <commands/ConvertNif.h>
#include <core/games.h>
#include <core/bsa.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;

using namespace Niflib;
using namespace std;

NiTriShapeRef convert_strip(NiTriStripsRef stripsRef)
{
	NiTriShapeRef shapeRef = new NiTriShape();
	shapeRef->SetName(stripsRef->GetName());
	shapeRef->SetExtraDataList(stripsRef->GetExtraDataList());
	shapeRef->SetTranslation(stripsRef->GetTranslation());
	shapeRef->SetRotation(stripsRef->GetRotation());
	shapeRef->SetScale(stripsRef->GetScale());
	shapeRef->SetFlags(524302);
	shapeRef->SetData(stripsRef->GetData());
	shapeRef->SetShaderProperty(stripsRef->GetShaderProperty());
	return shapeRef;
}
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
		Log::Info("NiNode visited");

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
				if (stripsRef->GetData()->IsSameType(NiTriShapeData::TYPE)) {
					children[index] = convert_strip(stripsRef);
				}
			}
			
			index++;
		}
		obj.SetChildren(children);
	}

	template<>
	inline void visit_object(NiTriStrips& obj) {

		bool hasAlpha = false;
		bool hasStencil = false;

		//get the NiTriStripsData
		NiTriStripsDataRef stripsData = DynamicCast<NiTriStripsData>(obj.GetData());

		//Copy over existing NiTriStripData to NiTriShapeData
		NiTriShapeDataRef shapeData = new  NiTriShapeData();

		//create new properties for textures.
		BSLightingShaderPropertyRef lightingProperty = new BSLightingShaderProperty();
		BSShaderTextureSetRef textureSet = new BSShaderTextureSet();

		//oblivions materials; for later.
		NiMaterialPropertyRef material = new NiMaterialProperty();
		NiTexturingPropertyRef texturing = new NiTexturingProperty();

		//Vertices
		shapeData->SetHasVertices(stripsData->GetHasVertices());
		shapeData->SetVertices(stripsData->GetVertices());

		//BS Vector flags. Oblivion doesn't have this, just settings it to UV and tangents.
		//Might need to work out Unk64, Unk128 and Unk256.
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(4097));

		//UVs
		shapeData->SetUvSets(stripsData->GetUvSets());

		//center & radius
		shapeData->SetCenter(stripsData->GetCenter());
		shapeData->SetRadius(stripsData->GetRadius());

		//Vertex Colours
		shapeData->SetHasVertexColors(stripsData->GetHasVertexColors());
		shapeData->SetVertexColors(stripsData->GetVertexColors());

		//Flags
		shapeData->SetConsistencyFlags(stripsData->GetConsistencyFlags());

		//triangles
		vector<Triangle> triangles = triangulate(stripsData->GetPoints());
		shapeData->SetNumTriangles(triangles.size());
		shapeData->SetNumTrianglePoints(triangles.size() * 3);
		shapeData->SetHasTriangles(1);
		shapeData->SetTriangles(triangles);

		//Tangents, Bitangents and Normals
		vector<Vector3> vertices = shapeData->GetVertices();
		Vector3 COM;
		if (vertices.size() != 0)
			COM = (COM / 2) + (centeroid(vertices) / 2);
		vector<Triangle> faces = shapeData->GetTriangles();
		vector<Vector3> normals = shapeData->GetNormals();
		if (vertices.size() != 0 && faces.size() != 0 && shapeData->GetUvSets().size() != 0) {
			vector<TexCoord> uvs = shapeData->GetUvSets()[0];
			//Tangent Space
			/*TriGeometryContext g(vertices, COM, faces, uvs, normals);
			shapeData->SetHasNormals(1);
			shapeData->SetNormals(g.normals);
			shapeData->SetTangents(g.tangents);
			shapeData->SetBitangents(g.bitangents);*/
		}

		//get properties to begin texture
		vector<Ref<NiProperty>> properties = obj.GetProperties();

		for (NiPropertyRef property : properties) {
			if (property->IsSameType(NiMaterialProperty::TYPE)) {
				material = DynamicCast<NiMaterialProperty>(property);

				lightingProperty->SetShaderType(BSShaderType::SHADER_DEFAULT);
				lightingProperty->SetName(material->GetName());
				lightingProperty->SetEmissiveColor(material->GetEmissiveColor());
				lightingProperty->SetSpecularColor(Color3(0, 0, 0));
				lightingProperty->SetEmissiveMultiple(1);
				lightingProperty->SetGlossiness(material->GetGlossiness());
				lightingProperty->SetAlpha(material->GetAlpha());
			}
			if (property->IsSameType(NiTexturingProperty::TYPE)) {
				//setup paths
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
			if (property->IsSameType(NiAlphaProperty::TYPE)) {
				hasAlpha = true;
			}
			if (property->IsSameType(NiStencilProperty::TYPE)) {
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() + SkyrimShaderPropertyFlags2::SLSF2_DOUBLE_SIDED));
			}

			lightingProperty->SetTextureSet(textureSet);
			obj.SetShaderProperty(DynamicCast<BSShaderProperty>(lightingProperty));
			obj.SetExtraDataList(vector<Ref<NiExtraData>> {});
			obj.SetProperties(vector<Ref<NiProperty>> {});
			obj.SetData(DynamicCast<NiGeometryData>(shapeData));
		}
	}

	template<class T>
	inline void visit_object(bhkCollisionObject& obj) {
		Log::Info("Detected a bhkCollisionObject");
		obj.SetFlags((static_cast<bhkCOFlags>(129)));
	}
};

//void findFiles(path startingDir, string extension, vector<path>& results) {
//	if (!exists(startingDir) || !is_directory(startingDir)) return;
//	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(startingDir))
//	{
//		if (is_directory(dirEntry.path()))
//			continue;
//
//		std::string entry_extension = dirEntry.path().extension().string();
//		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
//		if (entry_extension == extension) {
//			results.push_back(dirEntry.path().string());
//		}
//	}
//}

struct membuf : std::streambuf {
	membuf(char const* base, size_t size) {
		char* p(const_cast<char*>(base));
		this->setg(p, p, p + size);
	}
};
struct imemstream : virtual membuf, std::istream {
	imemstream(char const* base, size_t size)
		: membuf(base, size)
		, std::istream(static_cast<std::streambuf*>(this)) {
	}
};

bool BeginConversion() {
	NifInfo info;

	//findFiles(nif_in, ".nif", nifs);

	Games& games = Games::Instance();
	const Games::GamesPathMapT& installations = games.getGames();

	bool firstb = true;

	for (const auto& bsa : games.bsas(Games::TES4)) {
		std::cout << "Checking: " << bsa.filename() << std::endl;
		BSAFile bsa_file(bsa);
		bool first = true;
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


			root = convert_root(root);
			ConverterVisitor fimpl(info);
			root->accept(fimpl, info);

			fs::path out_path = nif_out / nif;
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


static bool ExecuteCmd(hkxcmdLine &cmdLine) {
	BeginConversion();
	return true;
}

REGISTER_COMMAND(NifConvert, HelpString, ExecuteCmd);