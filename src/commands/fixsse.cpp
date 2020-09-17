#include "stdafx.h"

#include <commands/fixsse.h>
#include <core/NifFile.h>
#include <core/HKXWrangler.h>
#include <core/FBXWrangler.h>

//collisions
#include <Physics\Collide\Shape\Misc\Transform\hkpTransformShape.h>
#include <Physics\Collide\Shape\Compound\Collection\List\hkpListShape.h>
#include <Physics\Collide\Shape\Deprecated\ConvexList\hkpConvexListShape.h>

#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h"
#include "Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h"
#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h"
#include "Physics/Collide/Util/Welding/hkpMeshWeldingUtility.h"
#include "Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h"
#include <Physics\Collide\Shape\Compound\Collection\CompressedMesh\hkpCompressedMeshShape.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h>
#include <Physics\Collide\Shape\Convex\ConvexTransform\hkpConvexTransformShape.h>

#include <Physics\Utilities\Collide\ShapeUtils\CreateShape\hkpCreateShapeUtility.h>
#include <Common\Base\Types\Geometry\hkStridedVertices.h>
#include <Common\Internal\ConvexHull\hkGeometryUtility.h>

#include <Common\GeometryUtilities\Misc\hkGeometryUtils.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesConnectivity.h>
#include <Physics\Utilities\Collide\ShapeUtils\ShapeConverter\hkpShapeConverter.h>

#include <Animation\Ragdoll\Utils\hkaRagdollUtils.h>

#include <core/EulerAngles.h>
#include <core/MathHelper.h>

#include <algorithm>

#include <VHACD.h>
#include <boundingmesh.h>
#include <core/NifFile.h>

#include <deque>
#include <map>
#include <Physics\Collide\Shape\Convex\Capsule\hkpCapsuleShape.h>
#include <Physics\Collide\Shape\Convex\Sphere\hkpSphereShape.h>
#include <Physics\Collide\Shape\Convex\Box\hkpBoxShape.h>


using namespace ckcmd;
using namespace ckcmd::fixsse;
using namespace ckcmd::Geometry;
using namespace ckcmd::NIF;

using namespace Niflib;
using namespace std;

static bool BeginScan(string scanPath);


FixSSENif::FixSSENif()
{
}

FixSSENif::~FixSSENif()
{
}

string FixSSENif::GetName() const
{
	return "fixssenif";
}

string FixSSENif::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd nifscan [-i <path_to_scan>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [-i <path_to_scan>] [-o <overwrite>] [-f <force_recollision>] [<vanilla_texture_path>] \r\n";

	const char help[] =
		R"(Scan Skyrim Legendary Editions meshes and apply fixes for SSE compatibility.
		
		Arguments:
			<path_to_scan> path to models you want to check for errors)
			<path_to_out> output path)
			<overwrite> overwrite nif instead of using the /out/ path
			<force_recollision> overwrite nif instead of using the /out/ path
			<vanilla_texture_path> search for textures in vanilla lot too)";

	return usage + help;
}

string FixSSENif::GetHelpShort() const
{
	return "ck-cmd.exe fixssenif -i path_to_scan -o overwrite? -f force_recollision? -vanilla_texture_path";
}

static inline hkTransform TOHKTRANSFORM(const Niflib::Matrix33& r, const Niflib::Vector4 t, const float scale = 1.0) {
	hkTransform out;
	out(0, 0) = r[0][0]; out(0, 1) = r[0][1]; out(0, 2) = r[0][2]; out(0, 3) = t[0] * scale;
	out(1, 0) = r[1][0]; out(1, 1) = r[1][1]; out(1, 2) = r[1][2]; out(1, 3) = t[1] * scale;
	out(2, 0) = r[2][0]; out(2, 1) = r[2][1]; out(2, 2) = r[2][2]; out(2, 3) = t[2] * scale;
	out(3, 0) = 0.0f;	 out(3, 1) = 0.0f;	  out(3, 2) = 0.0f;	   out(3, 3) = 1.0f;
	return out;
}

//! A bone and Triangle set
typedef struct
{
	list<int> bones;
	vector<Triangle> triangles;
} Partition;

//! Rotate a Triangle
inline void qRotate(Triangle& t)
{
	if (t[1] < t[0] && t[1] < t[2]) {
		t = { t[1], t[2], t[0] };
	}
	else if (t[2] < t[0]) {
		t = { t[2], t[0], t[1] };
	}
}

namespace std
{
	template<>
	struct less<Triangle> : public binary_function<Triangle, Triangle, bool>
	{
		bool operator()(const Triangle& s1, const Triangle& s2) const {
			int d = 0;
			if (d == 0) d = (s1[0] - s2[0]);
			if (d == 0) d = (s1[1] - s2[1]);
			if (d == 0) d = (s1[2] - s2[2]);
			return d < 0;
		}
	};
}

static list<int> mergeBones(list<int> a, list<int> b)
{
	for (const auto c : b) {
		if (find(a.begin(), a.end(), c) == a.end()) {
			a.push_back(c);
		}
	}
	return a;
}

static bool containsBones(list<int> a, list<int> b)
{
	for (const auto c : b) {
		if (find(a.begin(), a.end(), c) == a.end())
			return false;
	}
	return true;
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

class FixSSETargetsVisitor : public RecursiveFieldVisitor<FixSSETargetsVisitor> {
	vector<NiObjectRef>& blocks;
public:


	FixSSETargetsVisitor(NiObject* root, const NifInfo& info, vector<NiObjectRef>& blocks) :
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

NiTriShapeRef destrip(NiTriStripsRef& stripsRef)
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
	vector<Triangle> triangles = triangulate(stripsData->GetPoints());
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
		TriGeometryContext g(vertices, COM, faces, uvs, normals);
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

inline void visit_object(NiNodeRef obj) {
	vector<Ref<NiAVObject>> children = obj->GetChildren();
	vector<Ref<NiProperty>> properties = obj->GetProperties();
	vector<Ref<NiAVObject>>::iterator eraser = children.begin();
	while (eraser != children.end())
	{
		if (*eraser == NULL) {
			eraser = children.erase(eraser);
		}
		else
			eraser++;
	}
	int index = 0;
	for (NiAVObjectRef& block : children)
	{
		if (block->IsSameType(NiTriStrips::TYPE)) {
			NiTriStripsRef stripsRef = DynamicCast<NiTriStrips>(block);
			NiTriShapeRef shape = destrip(stripsRef);
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
		}
	}

	obj->SetChildren(children);
}

void findTextureOrTryToCorrect(BSShaderTextureSetRef texture_set, const fs::path& texture_folder_path, const fs::path& vanilla_path)
{
	vector<string> textures = texture_set->GetTextures();
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].empty())
			continue;

		//vurt, what are you doing?
		if (textures[i].substr(1, textures[i].size() - 1) == "NOR")
		{
			textures[i].clear();
			continue;
		}

		string this_tex = textures[i];
		fs::path texture_path = texture_folder_path / this_tex;
		if (fs::exists(texture_path))
			continue;
		texture_path = texture_folder_path / "textures" / this_tex;
		if (fs::exists(texture_path))
		{
			textures[i] = (fs::path("textures") / this_tex).string().c_str();
			continue;
		}
		texture_path = vanilla_path / this_tex;
		if (fs::exists(texture_path))
			continue;
		texture_path = vanilla_path / "textures" / this_tex;
		if (fs::exists(texture_path))
		{
			textures[i] = (fs::path("textures") / this_tex).string().c_str();
			continue;
		}
	}
	texture_set->SetTextures(textures);
}

inline void scanBSProperties(NiTriShapeRef shape, const fs::path& texture_folder_path, const fs::path& vanilla_path)
{
	//check texture sets
	string shape_name = shape->GetName();
	if (shape_name.find("EditorMarker") != string::npos)
		return;

	BSLightingShaderPropertyRef property = DynamicCast<BSLightingShaderProperty>(shape->GetShaderProperty());
	if (shape->GetShaderProperty() == NULL)
	{
		Log::Info("Missing any kind of shader property on trishape %s", shape_name.c_str());
		return;
	}
	if (property == NULL)
	{
		//BSEffect
		return;
	}

	if (shape->GetData() == NULL)
	{
		Log::Info("Missing Geometry Data trishape %s", shape_name.c_str());
		return;
	}

	if (property->GetTextureSet() == NULL || property->GetTextureSet()->GetTextures().empty())
	{
		Log::Info("Missing Texture Set on trishape %s", shape_name.c_str());
		return;
	}

	NiAlphaPropertyRef alpha = shape->GetAlphaProperty();
	BSShaderTextureSetRef textures = property->GetTextureSet();
	BSLightingShaderPropertyShaderType shader_type = property->GetSkyrimShaderType();

	findTextureOrTryToCorrect(textures, texture_folder_path, vanilla_path);

	bool hasVCFlag = shape->GetData()->GetHasVertexColors();
	bool hasVCArray = shape->GetData()->GetVertexColors().size() == shape->GetData()->GetVertices().size();

	if (hasVCFlag != hasVCArray)
	{
		Log::Info("Shape %s: shape VC inconsistant!", shape_name.c_str());
		if (hasVCFlag && shape->GetData()->GetVertexColors().size() != shape->GetData()->GetVertices().size())
		{
			//resizing vcs
			Log::Info("Shape %s: resizing vc!", shape_name.c_str());
			vector<Color4>& vcs = shape->GetData()->GetVertexColors();
			vcs.resize(shape->GetData()->GetVertexColors().size());
			shape->GetData()->SetVertexColors(vcs);
		}
	}
	else {
		if (hasVCFlag)
			property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() | SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
		else
			property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
	}

	//Check VS all black
	bool allBlack = true;
	vector<Color4>& vcs = shape->GetData()->GetVertexColors();
	if (vcs.size() > 0)
	{
		for (const auto& vc : vcs)
		{
			if (vc.r != 0.0 || vc.g != 0.0 && vc.b != 0.0)
			{
				allBlack = false;
				break;
			}
		}
		if (allBlack)
		{
			Log::Info("Shape %s: has all black Vertex Colors, turning to full white!", shape_name.c_str());
			for (auto& vc : vcs)
			{
				vc = Color4(1.0, 1.0, 1.0);
			}
			shape->GetData()->SetVertexColors(vcs);
		}
	}

	//Check first two texture slots:
	//Emissive
	fs::path texture_slot_0 = texture_folder_path / textures->GetTextures()[0];
	if (!fs::exists(texture_slot_0) && !fs::exists(vanilla_path / textures->GetTextures()[0]))
		Log::Warn("Shape %s: Unable to find Emissive texture: %s (not even in vanilla path %s)", shape_name.c_str(), texture_slot_0.string().c_str(), vanilla_path.string().c_str());
	//Normal
	fs::path texture_slot_1 = texture_folder_path / textures->GetTextures()[1];
	if (!fs::exists(texture_slot_1) && !fs::exists(vanilla_path / textures->GetTextures()[1]))
	{
		if (!textures->GetTextures()[1].empty())
		{
			Log::Warn("Shape %s: Unable to find Normal texture: %s (not even in vanilla path %s)", shape_name.c_str(), texture_slot_1.string().c_str(), vanilla_path.string().c_str());
		}
		else {
			Log::Warn("Empty Normal texture, setting default");
			vector<string> texture_s = textures->GetTextures();
			texture_s[1] = "textures\\default_n.dds";
			textures->SetTextures(texture_s);
		}
	}


	if (shader_type == BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP /*|| BSLightingShaderPropertyShaderType::ST_EYE_ENVMAP*/) {
		//Environment
		fs::path texture_slot_4 = texture_folder_path / textures->GetTextures()[4];
		fs::path texture_slot_5 = texture_folder_path / textures->GetTextures()[5];
		if (!fs::exists(texture_slot_4) && !fs::exists(texture_slot_5) && !fs::exists(vanilla_path / textures->GetTextures()[4]) && !fs::exists(texture_folder_path / textures->GetTextures()[5]))
			Log::Error("Shape %s: ShaderType is 'Environment', but no 'Environment' or 'Cubemap' texture is present at %s or %s ", shape_name.c_str(), texture_slot_4.string().c_str(), texture_slot_5.string().c_str());

		if (property->GetEnvironmentMapScale() == 0) {
			Log::Error("Shape %s: ShaderType is 'Environment', but map scale equals 0, making it obsolete. Setting to 1", shape_name.c_str());
			property->SetEnvironmentMapScale(1.0);
		}

		property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)(property->GetShaderFlags1_sk() | SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING));
		property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP));
	}
	if (shader_type == BSLightingShaderPropertyShaderType::ST_GLOW_SHADER) {
		// Glow Map
		fs::path texture_slot_2 = texture_folder_path / textures->GetTextures()[2];
		if (!fs::exists(texture_slot_2) && !fs::exists(vanilla_path / textures->GetTextures()[2]))
			Log::Error("Shape %s: ShaderType is 'Glow', but no 'Glow' texture is present %s.", shape_name.c_str(), texture_slot_2.string().c_str());

		property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)(property->GetShaderFlags1_sk() & ~SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING));
		property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)(property->GetShaderFlags1_sk() | SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE));
		property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() | SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP));
	}

	if ((property->GetShaderFlags2_sk() & SLSF2_TREE_ANIM) > 0)
	{
		if (shape->GetData()->GetVertexColors().empty())
		{
			Log::Error("Shape %s: SLSF2_TREE_ANIM is set, creating full white vertex colors", shape_name.c_str());
			vector<Color4> vcs(shape->GetData()->GetVertices().size());
			for (auto& vc : vcs)
			{
				vc = Color4(1.0, 1.0, 1.0);
			}
			shape->GetData()->SetVertexColors(vcs);
		}
	}

}

extern void ScanNif(vector<NiObjectRef>& blocks, NifInfo info);

//typedef bitset<12> bsx_flags_t;
//namespace ckcmd {
//	namespace nifscan
//	{
//		extern bsx_flags_t calculateSkyrimBSXFlags(const vector<NiObjectRef>& blocks, const NifInfo& info);
//	}
//}

void check(bhkCollisionObjectRef co)
{
	if (co->GetBody() == NULL)
	{
		Log::Error("Collision Object without Rigid Body. FATAL");
		return;
	}
	bhkRigidBodyRef rb = DynamicCast<bhkRigidBody>(co->GetBody());
	if (rb == NULL)
	{
		return;
	}
	SkyrimLayer layer = rb->GetHavokFilter().layer_sk;
	if (layer == SKYL_CLUTTER)
	{
		co->SetFlags((bhkCOFlags)(BHKCO_ACTIVE | BHKCO_SET_LOCAL | BHKCO_SYNC_ON_UPDATE));
	}
	else if (layer == SKYL_ANIMSTATIC)
	{
		co->SetFlags((bhkCOFlags)(BHKCO_ACTIVE | BHKCO_SET_LOCAL));
	}
	else if (layer == SKYL_STATIC)
	{
		co->SetFlags((bhkCOFlags)(BHKCO_ACTIVE));
	}
}

void check(bhkRigidBodyRef rb)
{
	rb->SetBroadPhaseType(BROAD_PHASE_ENTITY);
	hkWorldObjCinfoProperty cinfo;
	cinfo.data = 0;
	cinfo.size = 0;
	cinfo.capacityAndFlags = 2147483648;
	rb->SetCinfoProperty(cinfo);
	HavokFilter original = rb->GetHavokFilter();
	//check layer
	if (string(NifFile::layer_name(original.layer_sk)) == string("SKYL_UNIDENTIFIED"))
	{
		Log::Error("Faulty Havok Layer. Setting it to STATIC");
		original.layer_sk = SKYL_STATIC;
	}
	rb->SetHavokFilter(original);
	rb->SetHavokFilterCopy(original);
	rb->SetCollisionResponse(RESPONSE_SIMPLE_CONTACT);
	rb->SetCollisionResponse2(RESPONSE_SIMPLE_CONTACT);
	rb->SetProcessContactCallbackDelay(65535);
	rb->SetProcessContactCallbackDelay2(65535);
	if (rb->GetHavokFilter().layer_sk == SkyrimLayer::SKYL_CLUTTER) {
		rb->SetMotionSystem(MO_SYS_DYNAMIC);
		rb->SetSolverDeactivation(SOLVER_DEACTIVATION_LOW);
		rb->SetQualityType(MO_QUAL_MOVING);
	}
	if (rb->GetMotionSystem() == MO_SYS_KEYFRAMED)
		rb->SetMotionSystem(MO_SYS_BOX_INERTIA);
	if (rb->GetQualityType() == MO_QUAL_KEYFRAMED || rb->GetQualityType() == MO_QUAL_KEYFRAMED_REPORT)
		rb->SetQualityType(MO_QUAL_FIXED);
}

void check(bhkMoppBvTreeShapeRef mopp)
{
	if (mopp->GetMoppData().empty())
	{
		Log::Error("Empty MOPP: TODO: Recalculate");
	}
	if (mopp->GetShape() == NULL)
	{
		Log::Error("Empty MOPP SHAPE: TODO: Recalculate");
	}
}


struct KeySetter {};

template<>
struct Accessor<KeySetter> {
	Accessor(bhkListShapeRef list, const vector<unsigned int>& keys)
	{
		list->unknownInts = keys;
	}
};

static inline Niflib::Vector4 TOVECTOR4(const hkVector4& v) {
	return Niflib::Vector4(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
}

static inline InertiaMatrix TOINERTIAMATRIX(const hkMatrix3& q, const float scale = 1.0f, bool inverse = false) {

	hkVector4 c0 = q.getColumn(0);
	hkVector4 c1 = q.getColumn(1);
	hkVector4 c2 = q.getColumn(2);

	return InertiaMatrix(
		c0.getSimdAt(0), c1.getSimdAt(0), c2.getSimdAt(0), 0.0f,
		c0.getSimdAt(1), c1.getSimdAt(1), c2.getSimdAt(1), 0.0f,
		c0.getSimdAt(2), c1.getSimdAt(2), c2.getSimdAt(2), 0.0f
	);
}

static inline Matrix44 TOMATRIX44(const hkTransform& q, const float scale = 1.0f, bool inverse = false) {

	hkVector4 c0 = q.getColumn(0);
	hkVector4 c1 = q.getColumn(1);
	hkVector4 c2 = q.getColumn(2);
	hkVector4 c3 = q.getColumn(3);

	return Matrix44(
		c0.getSimdAt(0), c1.getSimdAt(0), c2.getSimdAt(0), (float)c3.getSimdAt(0) * scale,
		c0.getSimdAt(1), c1.getSimdAt(1), c2.getSimdAt(1), (float)c3.getSimdAt(1) * scale,
		c0.getSimdAt(2), c1.getSimdAt(2), c2.getSimdAt(2), (float)c3.getSimdAt(2) * scale,
		c0.getSimdAt(3), c1.getSimdAt(3), c2.getSimdAt(3), c3.getSimdAt(3)
	);
}

static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v) {
	return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
}

bhkCMSDMaterial consume_material_from_shape(hkpShape* shape)
{
	hkpNamedMeshMaterial* material = (hkpNamedMeshMaterial*)shape->getUserData();
	string name = material->m_name;
	bhkCMSDMaterial m; m.material = NifFile::material_value(name);
	m.filter.layer_sk = (SkyrimLayer)material->m_filterInfo;
	delete material;
	return m;
}

class BCMSPacker {};

template<>
class Accessor<BCMSPacker> {
public:
	Accessor(hkpCompressedMeshShape* pCompMesh, bhkCompressedMeshShapeDataRef pData)
	{
		short                                   chunkIdxNif(0);

		pData->SetBoundsMin(Vector4(pCompMesh->m_bounds.m_min(0), pCompMesh->m_bounds.m_min(1), pCompMesh->m_bounds.m_min(2), pCompMesh->m_bounds.m_min(3)));
		pData->SetBoundsMax(Vector4(pCompMesh->m_bounds.m_max(0), pCompMesh->m_bounds.m_max(1), pCompMesh->m_bounds.m_max(2), pCompMesh->m_bounds.m_max(3)));

		pData->SetBitsPerIndex(pCompMesh->m_bitsPerIndex);
		pData->SetBitsPerWIndex(pCompMesh->m_bitsPerWIndex);
		pData->SetMaskIndex(pCompMesh->m_indexMask);
		pData->SetMaskWIndex(pCompMesh->m_wIndexMask);
		pData->SetWeldingType(0); //seems to be fixed for skyrim pData->SetWeldingType(pCompMesh->m_weldingType);
		pData->SetMaterialType(1); //seems to be fixed for skyrim pData->SetMaterialType(pCompMesh->m_materialType);
		pData->SetError(pCompMesh->m_error);

		//  resize and copy bigVerts
		vector<Vector4 > tVec4Vec(pCompMesh->m_bigVertices.getSize());
		for (unsigned int idx(0); idx < pCompMesh->m_bigVertices.getSize(); ++idx)
		{
			tVec4Vec[idx].x = pCompMesh->m_bigVertices[idx](0);
			tVec4Vec[idx].y = pCompMesh->m_bigVertices[idx](1);
			tVec4Vec[idx].z = pCompMesh->m_bigVertices[idx](2);
			tVec4Vec[idx].w = pCompMesh->m_bigVertices[idx](3);
		}
		pData->SetBigVerts(tVec4Vec);

		//  resize and copy bigTris
		vector<bhkCMSDBigTris > tBTriVec(pCompMesh->m_bigTriangles.getSize());
		for (unsigned int idx(0); idx < pCompMesh->m_bigTriangles.getSize(); ++idx)
		{
			tBTriVec[idx].triangle1 = pCompMesh->m_bigTriangles[idx].m_a;
			tBTriVec[idx].triangle2 = pCompMesh->m_bigTriangles[idx].m_b;
			tBTriVec[idx].triangle3 = pCompMesh->m_bigTriangles[idx].m_c;
			tBTriVec[idx].material = pCompMesh->m_bigTriangles[idx].m_material;
			tBTriVec[idx].weldingInfo = pCompMesh->m_bigTriangles[idx].m_weldingInfo;
		}
		pData->SetBigTris(tBTriVec);

		//  resize and copy transform data
		vector<bhkCMSDTransform > tTranVec(pCompMesh->m_transforms.getSize());
		for (unsigned int idx(0); idx < pCompMesh->m_transforms.getSize(); ++idx)
		{
			tTranVec[idx].translation.x = pCompMesh->m_transforms[idx].m_translation(0);
			tTranVec[idx].translation.y = pCompMesh->m_transforms[idx].m_translation(1);
			tTranVec[idx].translation.z = pCompMesh->m_transforms[idx].m_translation(2);
			tTranVec[idx].translation.w = pCompMesh->m_transforms[idx].m_translation(3);
			tTranVec[idx].rotation.x = pCompMesh->m_transforms[idx].m_rotation(0);
			tTranVec[idx].rotation.y = pCompMesh->m_transforms[idx].m_rotation(1);
			tTranVec[idx].rotation.z = pCompMesh->m_transforms[idx].m_rotation(2);
			tTranVec[idx].rotation.w = pCompMesh->m_transforms[idx].m_rotation(3);
		}
		pData->chunkTransforms = tTranVec;

		vector<bhkCMSDMaterial > tMtrlVec(pCompMesh->m_materials.getSize());

		//hkpNamedMeshMaterial* material_array = pCompMesh->m_meshMaterials
		for (unsigned int idx(0); idx < pCompMesh->m_materials.getSize(); ++idx)
		{
			bhkCMSDMaterial& material = tMtrlVec[idx];
			hkpNamedMeshMaterial& hk_material = pCompMesh->m_namedMaterials[idx];
			material.material = NifFile::material_value(hk_material.m_name.cString());
			HavokFilter filter; filter.layer_sk = (SkyrimLayer)hk_material.m_filterInfo;
			material.filter = filter;
		}

		//  set material list
		pData->chunkMaterials = tMtrlVec;

		vector<bhkCMSDChunk> chunkListNif(pCompMesh->m_chunks.getSize());

		//  for each chunk
		for (hkArray<hkpCompressedMeshShape::Chunk>::iterator pCIterHvk = pCompMesh->m_chunks.begin(); pCIterHvk != pCompMesh->m_chunks.end(); pCIterHvk++)
		{
			//  get nif chunk
			bhkCMSDChunk& chunkNif = chunkListNif[chunkIdxNif];

			//  set offset => translation
			chunkNif.translation.x = pCIterHvk->m_offset(0);
			chunkNif.translation.y = pCIterHvk->m_offset(1);
			chunkNif.translation.z = pCIterHvk->m_offset(2);
			chunkNif.translation.w = pCIterHvk->m_offset(3);

			//  force flags to fixed values
			chunkNif.materialIndex = pCIterHvk->m_materialInfo;
			chunkNif.reference = 65535;
			chunkNif.transformIndex = pCIterHvk->m_transformIndex;

			//  vertices
			chunkNif.numVertices = pCIterHvk->m_vertices.getSize();
			chunkNif.vertices.resize(chunkNif.numVertices);
			for (unsigned int i(0); i < chunkNif.numVertices; ++i)
			{
				chunkNif.vertices[i] = pCIterHvk->m_vertices[i];
			}

			//  indices
			chunkNif.numIndices = pCIterHvk->m_indices.getSize();
			chunkNif.indices.resize(chunkNif.numIndices);
			for (unsigned int i(0); i < chunkNif.numIndices; ++i)
			{
				chunkNif.indices[i] = pCIterHvk->m_indices[i];
			}

			//  strips
			chunkNif.numStrips = pCIterHvk->m_stripLengths.getSize();
			chunkNif.strips.resize(chunkNif.numStrips);
			for (unsigned int i(0); i < chunkNif.numStrips; ++i)
			{
				chunkNif.strips[i] = pCIterHvk->m_stripLengths[i];
			}

			chunkNif.weldingInfo.resize(pCIterHvk->m_weldingInfo.getSize());
			for (int k = 0; k < pCIterHvk->m_weldingInfo.getSize(); k++) {
				chunkNif.weldingInfo[k] = pCIterHvk->m_weldingInfo[k];
			}

			++chunkIdxNif;

		}

		//  set modified chunk list to compressed mesh shape data
		pData->chunks = chunkListNif;
		//----  Merge  ----  END
	}
};

bhkShapeRef convert_from_hk(const hkpShape* shape, bhkCMSDMaterial& aggregate_layer)
{
	if (shape == NULL)
	{
		throw runtime_error("Trying to convert unexistant shape! Abort");
		return NULL;
	}

	//Containers
	/// hkpListShape type.
	if (HK_SHAPE_LIST == shape->getType())
	{
		bhkListShapeRef list = new bhkListShape();
		hkpListShape* hk_list = (hkpListShape*)&*shape;
		size_t num_shapes = hk_list->getNumChildShapes();

		vector<unsigned int > keys;
		vector<bhkShapeRef> shapes;
		bhkCMSDMaterial last_layer;
		for (int i = 0; i < num_shapes; i++)
		{
			bhkCMSDMaterial shape_layer;
			shapes.push_back(convert_from_hk(hk_list->getChildShapeInl(i), shape_layer));
			if (i > 0 &&
				(last_layer.filter.layer_sk != shape_layer.filter.layer_sk ||
					last_layer.material != shape_layer.material)
				)
			{
				Log::Warn("Multiple Collision layers or materials detected on a list!");
				continue;
			}
			last_layer = shape_layer;
		}
		hkpShapeKey key = hk_list->getFirstKey();
		while (key != HK_INVALID_SHAPE_KEY)
		{
			keys.push_back(key);
			key = hk_list->getNextKey(key);
		}
		list->SetSubShapes(shapes);
		Accessor<KeySetter>(list, keys);
		aggregate_layer = last_layer;
		HavokMaterial temp; temp.material_sk = aggregate_layer.material;
		list->SetMaterial(temp);
		return StaticCast<bhkShape>(list);
	}
	/// hkpConvexTransformShape type.
	if (HK_SHAPE_CONVEX_TRANSFORM == shape->getType())
	{
		bhkConvexTransformShapeRef convex_transform = new bhkConvexTransformShape();
		hkpConvexTransformShape* hk_transform = (hkpConvexTransformShape*)&*shape;
		bhkCMSDMaterial material;
		convex_transform->SetShape(convert_from_hk(hk_transform->getChildShape(), material));
		Matrix44 mat = TOMATRIX44(hk_transform->getTransform());
		convex_transform->SetTransform(mat);
		HavokMaterial temp; temp.material_sk = material.material;
		convex_transform->SetMaterial(temp);
		convex_transform->SetRadius(hk_transform->getChildShape()->getRadius());
		aggregate_layer = material;
		return StaticCast<bhkShape>(convex_transform);
	}
	/// hkpTransformShape type.
	if (HK_SHAPE_TRANSFORM == shape->getType())
	{
		bhkTransformShapeRef transform = new bhkTransformShape();
		hkpTransformShape* hk_transform = (hkpTransformShape*)&*shape;
		bhkCMSDMaterial material;
		transform->SetShape(convert_from_hk(hk_transform->getChildShape(), material));
		Matrix44 mat = TOMATRIX44(hk_transform->getTransform());
		transform->SetTransform(mat);
		HavokMaterial temp; temp.material_sk = material.material;
		transform->SetMaterial(temp);
		aggregate_layer = material;
		return StaticCast<bhkShape>(transform);
	}
	/// hkpMoppBvTreeShape type.
	if (HK_SHAPE_MOPP == shape->getType())
	{
		bhkMoppBvTreeShapeRef pMoppShape = new bhkMoppBvTreeShape();
		hkpMoppBvTreeShape* pMoppBvTree = (hkpMoppBvTreeShape*)&*shape;
		bhkCMSDMaterial material;
		pMoppShape->SetShape(convert_from_hk(pMoppBvTree->getShapeCollection(), material));
		pMoppShape->SetOrigin(Vector3(pMoppBvTree->getMoppCode()->m_info.m_offset(0), pMoppBvTree->getMoppCode()->m_info.m_offset(1), pMoppBvTree->getMoppCode()->m_info.m_offset(2)));
		pMoppShape->SetScale(pMoppBvTree->getMoppCode()->m_info.getScale());
		pMoppShape->SetBuildType(MoppDataBuildType((Niflib::byte) pMoppBvTree->getMoppCode()->m_buildType));
		pMoppShape->SetMoppData(vector<Niflib::byte>(pMoppBvTree->m_moppData, pMoppBvTree->m_moppData + pMoppBvTree->m_moppDataSize));
		aggregate_layer = material;
		return StaticCast<bhkShape>(pMoppShape);
	}

	//Shapes
	/// hkpSphereShape type.
	if (HK_SHAPE_SPHERE == shape->getType())
	{
		bhkSphereShapeRef sphere = new bhkSphereShape();
		hkpSphereShape* hk_sphere = (hkpSphereShape*)&*shape;
		sphere->SetRadius(hk_sphere->getRadius());
		aggregate_layer = consume_material_from_shape(hk_sphere);
		HavokMaterial temp; temp.material_sk = aggregate_layer.material;
		sphere->SetMaterial(temp);
		hkVector4 centre; hk_sphere->getCentre(centre);
		return StaticCast<bhkShape>(sphere);
	}
	/// hkpBoxShape type.
	if (HK_SHAPE_BOX == shape->getType())
	{
		bhkBoxShapeRef box = new bhkBoxShape();
		hkpBoxShape* hk_box = (hkpBoxShape*)&*shape;
		box->SetDimensions(TOVECTOR3(hk_box->getHalfExtents()));
		box->SetRadius(hk_box->getRadius());
		aggregate_layer = consume_material_from_shape(hk_box);
		HavokMaterial temp; temp.material_sk = aggregate_layer.material;
		box->SetMaterial(temp);
		hkVector4 centre; hk_box->getCentre(centre);
		return StaticCast<bhkShape>(box);
	}
	/// hkpCapsuleShape type.
	if (HK_SHAPE_CAPSULE == shape->getType())
	{
		bhkCapsuleShapeRef capsule = new bhkCapsuleShape();
		hkpCapsuleShape* hk_capsule = (hkpCapsuleShape*)&*shape;
		//Inverted?
		capsule->SetFirstPoint(TOVECTOR3(hk_capsule->getVertices()[1]));
		capsule->SetSecondPoint(TOVECTOR3(hk_capsule->getVertices()[0]));
		capsule->SetRadius(hk_capsule->getRadius());
		capsule->SetRadius1(hk_capsule->getRadius());
		capsule->SetRadius2(hk_capsule->getRadius());
		aggregate_layer = consume_material_from_shape(hk_capsule);
		HavokMaterial temp; temp.material_sk = aggregate_layer.material;
		capsule->SetMaterial(temp);
		hkVector4 centre; hk_capsule->getCentre(centre);
		return StaticCast<bhkShape>(capsule);
	}
	/// hkpConvexVerticesShape type.
	if (HK_SHAPE_CONVEX_VERTICES == shape->getType())
	{
		bhkConvexVerticesShapeRef convex = new bhkConvexVerticesShape();
		hkpConvexVerticesShape* hk_convex = (hkpConvexVerticesShape*)&*shape;
		hkArray<hkVector4> hk_vertices;
		vector<Vector4> vertices;
		hk_convex->getOriginalVertices(hk_vertices);
		for (const auto& v : hk_vertices)
		{
			vertices.push_back(TOVECTOR4(v));
		}
		convex->SetVertices(vertices);
		const hkArray<hkVector4>& hk_planes = hk_convex->getPlaneEquations();
		vector<Vector4> planes;
		for (const auto& n : hk_planes)
		{
			planes.push_back(TOVECTOR4(n));
		}
		convex->SetNormals(planes);
		aggregate_layer = consume_material_from_shape(hk_convex);
		HavokMaterial temp; temp.material_sk = aggregate_layer.material;
		convex->SetMaterial(temp);
		convex->SetRadius(hk_convex->getRadius());
		hkVector4 centre; hk_convex->getCentre(centre);
		return StaticCast<bhkShape>(convex);
	}
	/// hkpCompressedMeshShape type.
	if (HK_SHAPE_COMPRESSED_MESH == shape->getType())
	{
		bhkCompressedMeshShapeRef mesh = new bhkCompressedMeshShape();
		bhkCompressedMeshShapeDataRef data = new bhkCompressedMeshShapeData();
		hkpCompressedMeshShape* hk_mesh = (hkpCompressedMeshShape*)&*shape;
		Accessor<BCMSPacker> go(hk_mesh, data);
		mesh->SetRadius(hk_mesh->m_radius);
		mesh->SetRadiusCopy(hk_mesh->m_radius);
		mesh->SetData(data);
		hkAabb out;
		hk_mesh->getAabb(hkTransform::getIdentity(), 0.01, out);
		hkVector4 centre; out.getCenter(centre);
		return StaticCast<bhkShape>(mesh);
	}

	throw runtime_error("Trying to convert unsupported shape type! Abort");
	return NULL;
}



void extract_collision_shape(FbxNode* shape_root, vector<pair<FbxAMatrix, FbxMesh*>>& meshes)
{
	string name = shape_root->GetName();
	//Containers
	if (ends_with(name, "_transform"))
	{
		extract_collision_shape(shape_root->GetChild(0), meshes);
		for (pair<FbxAMatrix, FbxMesh*>& entry : meshes) {
			entry.first = shape_root->EvaluateLocalTransform() * entry.first;
		}
	}
	if (ends_with(name, "_list"))
	{
		size_t num_children = shape_root->GetChildCount();
		for (int i = 0; i < num_children; i++)
		{
			extract_collision_shape(shape_root->GetChild(i), meshes);
		}
	}
	else if (ends_with(name, "_convex_list"))
	{
		size_t num_children = shape_root->GetChildCount();
		for (int i = 0; i < num_children; i++)
		{
			extract_collision_shape(shape_root->GetChild(i), meshes);
		}
	}
	else if (ends_with(name, "_mopp"))
	{
		extract_collision_shape(shape_root->GetChild(0), meshes);
	}
	//shapes
	else if (ends_with(name, "_sphere"))
	{
		meshes.push_back({ FbxAMatrix(), shape_root->GetMesh() });
	}
	else if (ends_with(name, "_box"))
	{
		meshes.push_back({ FbxAMatrix(), shape_root->GetMesh() });
	}
	if (ends_with(name, "_capsule"))
	{
		meshes.push_back({ FbxAMatrix(), shape_root->GetMesh() });
	}
	if (ends_with(name, "_convex"))
	{
		meshes.push_back({ FbxAMatrix(), shape_root->GetMesh() });
	}
	if (ends_with(name, "_mesh"))
	{
		meshes.push_back({ FbxAMatrix(), shape_root->GetMesh() });
	}
}


NiCollisionObjectRef build_physics(set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes, bhkRigidBodyRef body)
{
	float bhkScaleFactorInverse = 0.01428f; // 1 skyrim unit = 0,01428m
	NiCollisionObjectRef return_collision = NULL;
	vector<hkpNamedMeshMaterial> materials;
	bhkCollisionObjectRef collision = new bhkCollisionObject();
	if (body == NULL)
	{
		body = new bhkRigidBodyT();
		FbxAMatrix transform; //local
		FbxVector4 T = transform.GetT();
		FbxQuaternion Q = transform.GetQ();
		Niflib::hkQuaternion q;
		body->SetTranslation({ (float)T[0] * bhkScaleFactorInverse,(float)T[1] * bhkScaleFactorInverse, (float)T[2] * bhkScaleFactorInverse });
		body->SetRotation(q);
	}
	bhkCMSDMaterial body_layer;
	size_t depth = 0;
	hkRefPtr<hkpRigidBody> hk_body = HKX::HKXWrapper::build_body(NULL, geometry_meshes);
	if (hk_body == NULL)
		return NULL;
	hkpRigidBodyCinfo body_cinfo; hk_body->getCinfo(body_cinfo);
	body->SetShape(convert_from_hk(body_cinfo.m_shape, body_layer));
	body->SetCenter(TOVECTOR4(body_cinfo.m_centerOfMass));
	body->SetInertiaTensor(TOINERTIAMATRIX(body_cinfo.m_inertiaTensor));
	body->SetMass(hk_body->getMass());

	if (body_layer.filter.layer_sk == SKYL_ANIMSTATIC || body_layer.filter.layer_sk == SKYL_BIPED)
	{
		body->SetMotionSystem(MO_SYS_BOX_INERTIA);
		body->SetSolverDeactivation(SOLVER_DEACTIVATION_LOW);
		body->SetQualityType(MO_QUAL_FIXED);
		collision->SetFlags((bhkCOFlags)(collision->GetFlags() | BHKCO_SET_LOCAL | BHKCO_SYNC_ON_UPDATE));
	}
	else if (body_layer.filter.layer_sk == SKYL_CLUTTER)
	{
		body->SetMotionSystem(MO_SYS_DYNAMIC);
		body->SetSolverDeactivation(SOLVER_DEACTIVATION_LOW);
		body->SetQualityType(MO_QUAL_MOVING);
		collision->SetFlags((bhkCOFlags)(collision->GetFlags() | BHKCO_SYNC_ON_UPDATE));
	}
	//Static
	else {
		body->SetMotionSystem(MO_SYS_BOX_STABILIZED);
		body->SetSolverDeactivation(SOLVER_DEACTIVATION_OFF);
		body->SetQualityType(MO_QUAL_INVALID);
		collision->SetFlags((bhkCOFlags)(collision->GetFlags() | BHKCO_SET_LOCAL));
	}
	body->SetHavokFilter(body_layer.filter);
	body->SetHavokFilterCopy(body->GetHavokFilter());

	collision->SetBody(StaticCast<bhkWorldObject>(body));
	return_collision = StaticCast<NiCollisionObject>(collision);
	return return_collision;
}

bool check_collisions(vector<NiObjectRef>& blocks)
{
	bool hasCollision = false;
	for (auto& block : blocks)
	{
		if (block->IsDerivedType(bhkRigidBody::TYPE))
		{
			check(DynamicCast<bhkRigidBody>(block));
			hasCollision = true;
		}
		if (block->IsDerivedType(bhkCollisionObject::TYPE))
		{
			check(DynamicCast<bhkCollisionObject>(block));
			hasCollision = true;
		}
		if (block->IsDerivedType(bhkMoppBvTreeShape::TYPE))
		{
			check(DynamicCast<bhkMoppBvTreeShape>(block));
			hasCollision = true;
		}
	}
	return hasCollision;
}

inline FbxAMatrix to_havok_matrix(const hkTransform& m)
{
	FbxAMatrix out;
	out[0][0] = m(0, 0); out[0][1] = m(0, 1); out[0][2] = m(0, 2); out[0][3] = m(3, 0);
	out[1][0] = m(1, 0); out[1][1] = m(1, 1); out[1][2] = m(1, 2); out[1][3] = m(3, 1);
	out[2][0] = m(2, 0); out[2][1] = m(2, 1); out[2][2] = m(2, 2); out[2][3] = m(3, 2);
	out[3][0] = m(0, 3); out[3][1] = m(1, 3); out[3][2] = m(2, 3); out[3][3] = m(3, 3);
	return out;
}

void check_physics(NiNodeRef collision_parent, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes, bool force = false)
{
	if (!collision_parent) return;
	//redo
	FbxManager* sdkManager = FbxManager::Create();
	FbxScene* scene = FbxScene::Create(sdkManager, "");
	set<pair<FbxAMatrix, FbxMesh*>> conv_geometry_meshes;
	vector<FbxSurfaceMaterial*> materials;

	if (collision_parent->GetCollisionObject() != NULL) {
		auto collision_object = DynamicCast<bhkCollisionObject>(collision_parent->GetCollisionObject());
		if (collision_object)
		{
			//extract old collision geometry
			FBX::FBXWrangler wrap;
			NifInfo info;
			info.userVersion = 12;
			info.userVersion2 = 83;
			info.version = Niflib::VER_20_2_0_7;
			FbxNode* collision = FbxNode::Create(scene, "Collision");
			wrap.convert(collision_object, collision, info);
			//match materials!
			vector<pair<FbxAMatrix, FbxMesh*>> meshes;
			//root -> _rb -> _shape 
			if (collision && collision->GetChild(0) && collision->GetChild(0)->GetChild(0))
			{
				extract_collision_shape(collision->GetChild(0)->GetChild(0), meshes);
			}


			for (auto& row : meshes)
			{

				FbxNode* parent = row.second->GetNode();
				string parent_name = parent->GetName();
				if (parent_name.find("_rb") == string::npos)
					parent = collision;
				if (parent)
				{
					int materials_size = parent->GetMaterialCount();				
					for (int i = 0; i < materials_size; i++)
					{
						auto fbx_material = parent->GetMaterial(i);
						if (find(materials.begin(), materials.end(), fbx_material)==materials.end())
							materials.push_back(parent->GetMaterial(i));
					}
					if (materials_size == 0) materials_size = 1;
				}
			}


		}
	}

	for (const auto& geometry_group : geometry_meshes) {
		if (!geometry_group.second->GetData()) continue;

		FbxAMatrix matrix = to_havok_matrix(geometry_group.first);
		FbxMesh* m = FbxMesh::Create(scene, "");
		FbxNode* materialParent = FbxNode::Create(sdkManager, "node");
		materialParent->AddNodeAttribute(m);

		const auto& vertices = geometry_group.second->GetData()->GetVertices();
		m->InitControlPoints(vertices.size());
		FbxVector4* points = m->GetControlPoints();

		for (int i = 0; i < m->GetControlPointsCount(); i++) {
			points[i] = FbxVector4(vertices[i].x, vertices[i].y, vertices[i].z);
		}

		const auto& tris = DynamicCast<NiTriShapeData>(geometry_group.second->GetData())->GetTriangles();
		for (auto& t : tris) {
			m->BeginPolygon(-1);
			m->AddPolygon(t.v1);
			m->AddPolygon(t.v2);
			m->AddPolygon(t.v3);
			m->EndPolygon();
		}
		
		vector<int> indices;

		for (auto collision_material : materials) 
			indices.push_back(materialParent->AddMaterial(collision_material));

		m->InitMaterialIndices(FbxLayerElement::EMappingMode::eAllSame);
		FbxLayerElementMaterial* layerElement = m->GetElementMaterial();
		FbxLayerElementArrayTemplate<int>& iarray = layerElement->GetIndexArray();
		iarray.SetAt(0, 1);


		conv_geometry_meshes.insert({ matrix, m });
	}
	auto co = DynamicCast<bhkCollisionObject>(collision_parent->GetCollisionObject());
	bhkRigidBodyRef rb = NULL;
	if (co && co->GetBody())
	{
		rb = DynamicCast<bhkRigidBody>(co->GetBody());
	}
	collision_parent->SetCollisionObject(build_physics(conv_geometry_meshes, rb));
}


typedef map<NiNodeRef, vector<pair<hkTransform, NiTriShapeRef>>> bodies_meshes_map_t;

void rebuild_collisions(NiObjectRef root, vector<NiObjectRef>& blocks, bool force = false) {
	bodies_meshes_map_t bodies_meshes_map;
	std::deque<NiObjectRef> visit_stack;
	set<NiNodeRef> animated_nodes;

	//build a list of animated nodes, if any
	for (const auto& block : blocks)
	{
		if (block->IsDerivedType(NiControllerSequence::TYPE))
		{
			NiControllerSequenceRef seq = DynamicCast<NiControllerSequence>(block);
			for (const auto& cb : seq->GetControlledBlocks()) {
				if (cb.controller->GetTarget()->IsDerivedType(NiNode::TYPE))
				{
					animated_nodes.insert(DynamicCast<NiNode>(cb.controller->GetTarget()));
				}
			}
		}
	}

	std::function<void(NiObjectRef, std::deque<NiObjectRef>&)> findShapesToBeCollisioned = [&](NiObjectRef current_node, std::deque<NiObjectRef>& stack) {
		//recurse until we find a shape, avoiding rb as their mesh will be taken into account later
		stack.push_front(current_node);
		if (current_node->IsDerivedType(NiNode::TYPE))
		{
			vector<NiAVObjectRef> children = DynamicCast<NiNode>(current_node)->GetChildren();
			for (int i = 0; i < children.size(); i++) {
				findShapesToBeCollisioned(StaticCast<NiObject>(children[i]), stack);
			}
		}
		if (current_node->IsDerivedType(NiTriShape::TYPE))
		{
			//I'm a mesh, find my nearest parent RB, if any
			NiNodeRef parent_collisioner = NULL;
			hkTransform transform_from_rigid_body; transform_from_rigid_body.setIdentity();
			for (const auto& element : stack)
			{
				NiObjectRef parent = element;
				if (parent->IsDerivedType(NiNode::TYPE))
				{
					NiNodeRef parent_node = DynamicCast<NiNode>(parent);
					hkTransform this_transform = TOHKTRANSFORM(parent_node->GetRotation(), parent_node->GetTranslation(), parent_node->GetScale());
					transform_from_rigid_body.setMul(this_transform, transform_from_rigid_body);

					if (parent_node->GetCollisionObject() != NULL ||
						animated_nodes.find(parent_node) != animated_nodes.end())
					{
						parent_collisioner = parent_node;
						break;
					}
				}
			}
			bodies_meshes_map_t::iterator it = bodies_meshes_map.find(parent_collisioner);
			if (it == bodies_meshes_map.end())
				bodies_meshes_map[parent_collisioner] = vector<pair<hkTransform, NiTriShapeRef>>();
			bodies_meshes_map[parent_collisioner].push_back({ transform_from_rigid_body, DynamicCast<NiTriShape>(current_node) });
		}
		stack.pop_front();
	};

	findShapesToBeCollisioned(root, visit_stack);
	Log::Info("Found shapes/collisions relation");

	//todo: avoid build collisions for vfx, skins


	for (auto& entry : bodies_meshes_map)
	{
		if (force && !entry.first && bodies_meshes_map.size() == 1)
			check_physics(DynamicCast<NiNode>(root), entry.second, force);
		else
			check_physics(entry.first, entry.second, force);
	}
}


vector<NiObjectRef> fixssenif(vector<NiObjectRef> blocks, NifInfo info, const fs::path& texture_path, const fs::path& vanilla_texture_path, bool forceCollision) {

	NiObjectRef root = GetFirstRoot(blocks);

	for (auto& block : blocks) {
		if (block->IsDerivedType(NiNode::TYPE))
		{
			visit_object(DynamicCast<NiNode>(block));
		}
	}

	//to calculate the right flags, we need to rebuild the blocks
	vector<NiObjectRef> new_blocks = RebuildVisitor(root, info).blocks;

	//fix targets from nitrishapes substitution
	FixSSETargetsVisitor(GetFirstRoot(new_blocks), info, new_blocks);

	BSXFlagsRef bsx_flags = NULL;

	for (int i = 0; i < new_blocks.size(); i++) {
		auto& block = new_blocks[i];
		if (block->IsDerivedType(BSXFlags::TYPE))
			bsx_flags = DynamicCast<BSXFlags>(block);
		if (block->IsDerivedType(NiTriShape::TYPE))
			scanBSProperties(DynamicCast<NiTriShape>(block), texture_path, vanilla_texture_path);
		if (block->IsDerivedType(NiTriShapeData::TYPE))
		{
			NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(block);
			vector<Vector3> vertices = data->GetVertices();
			Vector3 COM;
			if (vertices.size() != 0)
				COM = (COM / 2) + (ckcmd::Geometry::centeroid(vertices) / 2);
			vector<Triangle> faces = data->GetTriangles();
			vector<Vector3> normals = data->GetNormals();
			if (normals.size() != vertices.size() && (faces.empty() || data->GetUvSets().empty()))
			{
				Log::Error("Model has no normals or no faces or no UV. Won't be able to calculate tangent space");
				//normals are needed for sse, let's just fill the void here
				normals.resize(vertices.size());
				for (int i = 0; i < vertices.size(); i++)
				{
					normals[i] = COM - vertices[i];
					normals[i] = normals[i].Normalized();
				}
				data->SetHasNormals(true);
				data->SetNormals(normals);
			}
			if (vertices.size() != 0 && faces.size() != 0 && data->GetUvSets().size() != 0) {
				vector<TexCoord> uvs = data->GetUvSets()[0];

				//Seems that tangent space calculation wants the uv flipped
				for (auto& uv : uvs)
				{
					float u = uv.u;
					uv.u = uv.v;
					uv.v = u;
				}

				TriGeometryContext g(vertices, COM, faces, uvs, normals);
				data->SetHasNormals(1);
				data->SetNormals(g.normals);
				data->SetTangents(g.tangents);
				data->SetBitangents(g.bitangents);
				data->SetBsVectorFlags(static_cast<BSVectorFlags>(data->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
			}
		}
		if (new_blocks[i]->IsDerivedType(NiTimeController::TYPE)) {
			NiTimeControllerRef ref = DynamicCast<NiTimeController>(new_blocks[i]);
			if (ref->GetTarget() == NULL) {
				Log::Error("Block[%i]: Controller has no target. This will increase the chances of a crash.", i);
			}
		}

		if (new_blocks[i]->IsDerivedType(NiControllerSequence::TYPE)) {
			NiSequenceRef ref = DynamicCast<NiSequence>(new_blocks[i]);

			if (ref->GetControlledBlocks().size() != 0) {
				vector<ControlledBlock> blocks = ref->GetControlledBlocks();

				for (int y = 0; y != blocks.size(); y++) {
					if (blocks[y].controllerType == "") {
						Log::Error("Block[%i]: ControlledBlock number %i, has a blank controller type.", i, y);
					}
				}
			}
		}
	}

	if (bsx_flags != NULL)
	{
		bsx_flags_t actual = calculateSkyrimBSXFlags(new_blocks, info);
		bsx_flags->SetIntegerData(actual.to_ulong());
	}

	set<NiObjectRef> roots = FindRoots(new_blocks);
	if (roots.size() != 1)
		throw runtime_error("Model has multiple roots!");

	if (check_collisions(new_blocks) || forceCollision)
		rebuild_collisions(GetFirstRoot(new_blocks), new_blocks, forceCollision);

	//to calculate the right flags, we need to rebuild the blocks
	new_blocks = RebuildVisitor(root, info).blocks;

	if (bsx_flags != NULL)
	{
		bsx_flags_t actual = calculateSkyrimBSXFlags(new_blocks, info);
		bsx_flags->SetIntegerData(actual.to_ulong());
	}

	return move(new_blocks);
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
	hkMemoryRouter* pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}


bool FixSSENif::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string scanPath;
	string vanilla_texture_path = "";
	bool doOverwrite = false;
	bool force_recollision = false;

	if (parsedArgs.find("<overwrite>") != parsedArgs.end() && parsedArgs["<overwrite>"].asString() == "true")
		doOverwrite = true;
	if (parsedArgs.find("<force_recollision>") != parsedArgs.end() && parsedArgs["<force_recollision>"].asString() == "true")
		force_recollision = true;
	if (parsedArgs.find("<vanilla_texture_path>") != parsedArgs.end() && parsedArgs.find("<vanilla_texture_path>") != parsedArgs.end() &&
		parsedArgs["<vanilla_texture_path>"].isString())
		vanilla_texture_path = parsedArgs["<vanilla_texture_path>"].asString();

	scanPath = parsedArgs["<path_to_scan>"].asString();
	Log::Info("Scan Path: %s", scanPath.c_str());
	Log::Info("Vanilla texture Path: %s", vanilla_texture_path.c_str());
	if (fs::exists(scanPath) && fs::is_directory(scanPath)) {
		InitializeHavok();
		vector<fs::path> nifs; find_files(scanPath, ".nif", nifs);
		fs::path texture_path = fs::path(scanPath).parent_path();
		for (size_t i = 0; i < nifs.size(); i++) {
			Log::Info("Current File: %s", nifs[i].string().c_str());
			NifInfo info;
			try {
				vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
				vector<NiObjectRef> new_blocks = fixssenif(blocks, info, texture_path, vanilla_texture_path, force_recollision);
				fs::path out;
				if (!doOverwrite) {
					out = fs::path(scanPath).parent_path() / fs::path("out") / relative_to(nifs[i], scanPath);
					fs::path parent_out = out.parent_path();
					if (!fs::exists(parent_out))
						fs::create_directories(parent_out);
				}
				else {
					out = nifs[i];
				}
				WriteNifTree(out.string(), GetFirstRoot(new_blocks), info);
				blocks = ReadNifList(out.string().c_str(), &info);
				for (const auto& block : blocks)
				{
					if (block->IsDerivedType(NiTriStrips::TYPE) || block->IsDerivedType(NiTriStripsData::TYPE)) {
						throw std::runtime_error("Round trip error, model not triangulated!");
					}
				}
			}
			catch (const std::exception& e) {
				Log::Info("ERROR: %s", e.what());
				fs::path out = fs::path(scanPath).parent_path() / fs::path("error") / relative_to(nifs[i], scanPath);
				fs::path parent_out = out.parent_path();
				if (!fs::exists(parent_out))
					fs::create_directories(parent_out);
				fs::copy(nifs[i], out, fs::copy_options::overwrite_existing);
			}
			catch (...) {
				Log::Info("SERIOUS ERROR");
				fs::path out = fs::path(scanPath).parent_path() / fs::path("error") / relative_to(nifs[i], scanPath);
				fs::path parent_out = out.parent_path();
				if (!fs::exists(parent_out))
					fs::create_directories(parent_out);
				fs::copy(nifs[i], out, fs::copy_options::overwrite_existing);
			}
		}
		Log::Info("Done..");
		CloseHavok();
	}
	return true;
}