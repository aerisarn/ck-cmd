/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius
See the included LICENSE file
*/

#include <core/FBXWrangler.h>
#include <core/EulerAngles.h>
#include <core/MathHelper.h>
#include <commands/Geometry.h>
#include <commands/NifScan.h>
#include <core/log.h>

#include <Physics\Utilities\Collide\ShapeUtils\CreateShape\hkpCreateShapeUtility.h>
#include <Common\GeometryUtilities\Misc\hkGeometryUtils.h>
#include <Physics\Utilities\Collide\ShapeUtils\ShapeConverter\hkpShapeConverter.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Common\Base\Types\Geometry\hkGeometry.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h>
#include <Common\Base\Types\Geometry\hkStridedVertices.h>
#include <Physics\Collide\Shape\Convex\Sphere\hkpSphereShape.h>

//// Physics
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>

#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\StiffSpring\hkpStiffSpringConstraintData.h>

#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Physics\Dynamics\World\hkpPhysicsSystem.h>
#include <Physics\Utilities\Serialize\hkpPhysicsData.h>

#include <Physics\Collide\Shape\Misc\Transform\hkpTransformShape.h>
#include <Physics\Collide\Shape\Compound\Collection\List\hkpListShape.h>
#include <Physics\Collide\Shape\Deprecated\ConvexList\hkpConvexListShape.h>
#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h"
#include <Physics\Collide\Shape\Compound\Collection\CompressedMesh\hkpCompressedMeshShape.h>
#include <Physics\Collide\Shape\Convex\ConvexTransform\hkpConvexTransformShape.h>

#include <Common\Internal\ConvexHull\hkGeometryUtility.h>
#include <Common\GeometryUtilities\Misc\hkGeometryUtils.h>

#include <algorithm>

#include <VHACD.h>
#include <boundingmesh.h>

#include <commands/fixsse.h>

using namespace ckcmd::FBX;
using namespace  ckcmd::Geometry;
using namespace ckcmd::nifscan;
using namespace ckcmd::HKX;

static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v, const float scale = 1.0f) {
	return Niflib::Vector3((float)v.getSimdAt(0) * scale, (float)v.getSimdAt(1) * scale, (float)v.getSimdAt(2) * scale);
}

static inline Niflib::Vector4 TOVECTOR4(const hkVector4& v, const float scale = 1.0f) {
	return Niflib::Vector4((float)v.getSimdAt(0) * scale, (float)v.getSimdAt(1) * scale, (float)v.getSimdAt(2) * scale, (float)v.getSimdAt(3));
}

static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v) {
	return hkVector4(v.x, v.y, v.z, v.w);
}

static inline Niflib::Quaternion TOQUAT(const ::hkQuaternion& q, bool inverse = false) {
	Niflib::Quaternion qt(q.m_vec.getSimdAt(3), q.m_vec.getSimdAt(0), q.m_vec.getSimdAt(1), q.m_vec.getSimdAt(2));
	return inverse ? qt.Inverse() : qt;
}

static inline ::hkQuaternion TOQUAT(const Niflib::Quaternion& q, bool inverse = false) {
	hkVector4 v(q.x, q.y, q.z, q.w);
	v.normalize4();
	::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
	if (inverse) qt.setInverse(qt);
	return qt;
}

static inline ::hkQuaternion TOQUAT(const Niflib::hkQuaternion& q, bool inverse = false) {
	hkVector4 v(q.x, q.y, q.z, q.w);
	v.normalize4();
	::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
	if (inverse) qt.setInverse(qt);
	return qt;
}

static inline hkMatrix3 TOMATRIX3(const Niflib::InertiaMatrix& q, bool inverse = false) {
	hkMatrix3 m3;
	m3.setCols(TOVECTOR4(q.rows[0]), TOVECTOR4(q.rows[1]), TOVECTOR4(q.rows[2]));
	if (inverse) m3.invert(0.001);
}

static inline Vector4 HKMATRIXROW(const hkTransform& q, const unsigned int row) {
	return Vector4(q(row, 0), q(row, 1), q(row, 2), q(row, 3));
}

static inline FbxVector4 TOFBXVECTOR4(const hkVector4& v)
{
	return FbxVector4(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
}

static inline FbxVector4 TOFBXVECTOR3(const Vector4& v)
{
	return FbxVector4(v[0], v[1], v[2]);
}

static inline FbxQuaternion TOFBXQUAT(const Quaternion& q)
{
	return FbxQuaternion(q.x, q.y, q.z, q.w);
}

static inline Matrix44 TOMATRIX44(const hkTransform& q, const float scale = 1.0f, bool inverse = false) {

	hkVector4 c0 = q.getColumn(0);
	hkVector4 c1 = q.getColumn(1);
	hkVector4 c2 = q.getColumn(2);
	hkVector4 c3 = q.getColumn(3);

	return Matrix44(
		c0.getSimdAt(0), c1.getSimdAt(0), c2.getSimdAt(0), (float)c3.getSimdAt(0)*scale,
		c0.getSimdAt(1), c1.getSimdAt(1), c2.getSimdAt(1), (float)c3.getSimdAt(1)*scale,
		c0.getSimdAt(2), c1.getSimdAt(2), c2.getSimdAt(2), (float)c3.getSimdAt(2)*scale,
		c0.getSimdAt(3), c1.getSimdAt(3), c2.getSimdAt(3), c3.getSimdAt(3)
	);
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

FbxAMatrix getTransform(const NiAVObjectRef av)
{
	FbxAMatrix avm;
	avm.SetTQS(
		TOFBXVECTOR3(av->GetTranslation()),
		TOFBXQUAT(av->GetRotation().AsQuaternion()),
		{ av->GetScale(), av->GetScale(), av->GetScale() }
	);
	return avm;
}

FbxNode* setMatTransform(const Matrix44& av, FbxNode* node, double bake_scale = 1.0) {
	Vector3 translation = av.GetTrans();
	//
	node->LclTranslation.Set(FbxDouble3(translation.x * bake_scale, translation.y * bake_scale, translation.z *bake_scale));
	Quaternion rotation = av.GetRotation().AsQuaternion();
	Quat QuatTest = { rotation.x, rotation.y, rotation.z, rotation.w };
	EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
	node->LclRotation.Set(FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z)));
	node->LclScaling.Set(FbxDouble3(av.GetScale(), av.GetScale(), av.GetScale()));
	return node;
}

FbxNode* setMatATransform(const FbxAMatrix& av, FbxNode* node) {
	node->LclTranslation.Set(av.GetT());
	node->LclRotation.Set(av.GetR());
	node->LclScaling.Set(av.GetS());
	return node;
}

FBXWrangler::FBXWrangler() {
	sdkManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	NewScene();
}

FBXWrangler::~FBXWrangler() {
	if (scene)
		CloseScene();

	if (sdkManager)
		sdkManager->Destroy();
}

void FBXWrangler::NewScene() {
	if (scene)
		CloseScene();

	scene = FbxScene::Create(sdkManager, "ckcmd");
	FbxAxisSystem maxSystem(FbxAxisSystem::EUpVector::eZAxis, (FbxAxisSystem::EFrontVector) - 2, FbxAxisSystem::ECoordSystem::eRightHanded);
	scene->GetGlobalSettings().SetAxisSystem(maxSystem);
	scene->GetRootNode()->LclScaling.Set(FbxDouble3(1.0, 1.0, 1.0));
}

void FBXWrangler::CloseScene() {
	if (scene)
		scene->Destroy();
	
	scene = nullptr;
	comName.clear();
}

namespace std {

	template <>
	struct hash<Triangle>
	{
		std::size_t operator()(const Triangle& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<unsigned short>()(k.v1)
				^ (hash<unsigned short>()(k.v2) << 1)) >> 1)
				^ (hash<unsigned short>()(k.v3) << 1);
		}
	};

}

template<>
class Accessor<bhkCompressedMeshShapeData> 
{
	hkGeometry& geometry;
	vector<bhkCMSDMaterial>& havok_materials;
public:

	Accessor(bhkCompressedMeshShapeRef cmesh, hkGeometry& geometry, vector<bhkCMSDMaterial>& materials) : geometry(geometry), havok_materials(materials)
	{
		bhkCompressedMeshShapeDataRef data = cmesh->GetData();

		const vector<bhkCMSDChunk>& chunks = data->chunks;
		const vector<Vector4>& bigVerts = data->GetBigVerts();

		bool multipleShapes = (chunks.size() + bigVerts.empty() ? 0 : 1) > 1;

		if (!data->GetBigVerts().empty())
		{
			const vector<bhkCMSDBigTris>& bigTris = data->GetBigTris();

			for (int i = 0; i < bigVerts.size(); i++)
				geometry.m_vertices.pushBack({ bigVerts[i][0], bigVerts[i][1], bigVerts[i][2] });//verts.push_back({ bigVerts[i][0], bigVerts[i][1], bigVerts[i][2] });

			for (int i = 0; i < bigTris.size(); i++) {
				geometry.m_triangles.pushBack(
					{ 
						(int)bigTris[i].triangle1,
						(int)bigTris[i].triangle2,
						(int)bigTris[i].triangle3,
						(int)bigTris[i].material
					}
				);
			}
		}

		int i = 0;
		havok_materials = data->chunkMaterials;
		const vector<bhkCMSDTransform>& transforms = data->chunkTransforms;
		auto n = chunks.size();
		for (const bhkCMSDChunk& chunk : chunks)
		{
			Vector3 chunkOrigin = { chunk.translation[0], chunk.translation[1], chunk.translation[2] };
			int numOffsets = chunk.numVertices;
			int numIndices = chunk.numIndices;
			int numStrips = chunk.numStrips;
			const vector<unsigned short>& offsets = chunk.vertices;
			const vector<unsigned short>& indices = chunk.indices;
			const vector<unsigned short>& strips = chunk.strips;

			int offset = 0;

			const bhkCMSDTransform& transform = transforms[chunk.transformIndex];

			hkQTransform t;
			t.setTranslation(TOVECTOR4(transform.translation));
			t.setRotation(TOQUAT(transform.rotation));
			hkMatrix4 rr; rr.set(t);


			int verOffsets = geometry.m_vertices.getSize();
			int n = 0;
			for (n = 0; n < (numOffsets / 3); n++) {
				Vector3 vec = chunkOrigin + Vector3(offsets[3 * n], offsets[3 * n + 1], offsets[3 * n + 2]) / 1000.0f;
				hkVector4 p = TOVECTOR4(vec);
				rr.transformPosition(p, p);
				geometry.m_vertices.pushBack(p);
			}

			for (auto s = 0; s < numStrips; s++) {
				for (auto f = 0; f < strips[s] - 2; f++) {
					if ((f + 1) % 2 == 0) {
						geometry.m_triangles.pushBack(
							{ 
								(int)indices[offset + f + 2] + verOffsets, 
								(int)indices[offset + f + 1] + verOffsets, 
								(int)indices[offset + f + 0] + verOffsets, 
								(int)chunk.materialIndex
							}
						);
					}
					else {
						geometry.m_triangles.pushBack(
							{
								(int)indices[offset + f + 0] + verOffsets,
								(int)indices[offset + f + 1] + verOffsets,
								(int)indices[offset + f + 2] + verOffsets,
								(int)chunk.materialIndex
							}
						);
					}
				}
				offset += strips[s];
			}

			// Non-stripped tris
			for (auto f = 0; f < (numIndices - offset); f += 3) {
				geometry.m_triangles.pushBack(
					{
						(int)indices[offset + f + 0] + verOffsets,
						(int)indices[offset + f + 1] + verOffsets,
						(int)indices[offset + f + 2] + verOffsets,
						(int)chunk.materialIndex
					}
				);
			}
		}
	}
};


//class ConstraintVisitor {
//protected:
//	vector<bhkBlendCollisionObjectRef>& nifBodies;
//	hkArray<hkpRigidBody*>& hkBodies;
//
//	hkpRigidBody* getEntity(Ref<bhkEntity> e) {
//		int index = find_if(nifBodies.begin(), nifBodies.end(), [e](bhkBlendCollisionObjectRef b) -> bool { return &(*b->GetBody()) == &*e; }) - nifBodies.begin();
//		if (index < 0 || index >= hkBodies.getSize()) throw runtime_error("Invalid entity into constraint!");
//		return hkBodies[index];
//	}
//public:
//	virtual hkpConstraintData* visit(RagdollDescriptor& constraint) = 0;
//	virtual hkpConstraintData* visit(PrismaticDescriptor& constraint) = 0;
//	virtual hkpConstraintData* visit(MalleableDescriptor& constraint) = 0;
//	virtual hkpConstraintData* visit(HingeDescriptor& constraint) = 0;
//	virtual hkpConstraintData* visit(LimitedHingeDescriptor& constraint) = 0;
//	virtual hkpConstraintData* visit(BallAndSocketDescriptor& constraint) = 0;
//	virtual hkpConstraintData* visit(StiffSpringDescriptor& constraint) = 0;
//
//	virtual hkpConstraintInstance* visitConstraint(bhkConstraintRef constraint) {
//		hkpConstraintData* data = NULL;
//		if (constraint)
//		{
//			if (constraint->IsSameType(bhkRagdollConstraint::TYPE))
//				data = visit(DynamicCast<bhkRagdollConstraint>(constraint)->GetRagdoll());
//			else if (constraint->IsSameType(bhkPrismaticConstraint::TYPE))
//				data = visit(DynamicCast<bhkPrismaticConstraint>(constraint)->GetPrismatic());
//			else if (constraint->IsSameType(bhkMalleableConstraint::TYPE))
//				data = visit(DynamicCast<bhkMalleableConstraint>(constraint)->GetMalleable());
//			else if (constraint->IsSameType(bhkHingeConstraint::TYPE))
//				data = visit(DynamicCast<bhkHingeConstraint>(constraint)->GetHinge());
//			else if (constraint->IsSameType(bhkLimitedHingeConstraint::TYPE))
//				data = visit(DynamicCast<bhkLimitedHingeConstraint>(constraint)->GetLimitedHinge());
//			else if (constraint->IsSameType(bhkBallAndSocketConstraint::TYPE))
//				data = visit(DynamicCast<bhkBallAndSocketConstraint>(constraint)->GetBallAndSocket());
//			else if (constraint->IsSameType(bhkStiffSpringConstraint::TYPE))
//				data = visit(DynamicCast<bhkStiffSpringConstraint>(constraint)->GetStiffSpring());
//			else
//				throw new runtime_error("Unimplemented constraint type!");
//			return new hkpConstraintInstance(getEntity(constraint->GetEntities()[0]), getEntity(constraint->GetEntities()[1]), data);
//		}
//	}
//
//	ConstraintVisitor(vector<bhkBlendCollisionObjectRef>& nbodies, hkArray<hkpRigidBody*>& hkbodies) : nifBodies(nbodies), hkBodies(hkbodies) {}
//};
//
//class FBXConstraintBuilder : public ConstraintVisitor {
//public:
//
//	virtual hkpConstraintData* visit(BallAndSocketDescriptor& constraint)
//	{
//		hkpBallAndSocketConstraintData* data = new hkpBallAndSocketConstraintData();
//		return data;
//	}
//
//	virtual hkpConstraintData* visit(StiffSpringDescriptor& constraint)
//	{
//		hkpStiffSpringConstraintData* data = new hkpStiffSpringConstraintData();
//		return data;
//	}
//
//	virtual hkpConstraintData* visit(RagdollDescriptor& descriptor) {
//		hkpRagdollConstraintData* data = new hkpRagdollConstraintData();
//		data->setInBodySpace(
//			TOVECTOR4(descriptor.pivotA * 7),
//			TOVECTOR4(descriptor.pivotB * 7),
//			TOVECTOR4(descriptor.planeA),
//			TOVECTOR4(descriptor.planeB),
//			TOVECTOR4(descriptor.twistA),
//			TOVECTOR4(descriptor.twistB)
//		);
//		return data;
//	}
//
//	virtual hkpConstraintData* visit(PrismaticDescriptor& descriptor) {
//		//hkpPrismaticConstraintData* data = new hkpPrismaticConstraintData();
//		//data->setInBodySpace(
//		//	TOVECTOR4(descriptor.pivotA * 7),
//		//	TOVECTOR4(descriptor.pivotB * 7),
//		//	);
//		//);
//
//		//return new hkpConstraintInstance(getEntity(constraint.GetEntities()[0]), getEntity(constraint.GetEntities()[1]), data);
//		return NULL;
//	}
//
//	virtual hkpConstraintData* visit(MalleableDescriptor& descriptor) {
//		switch (descriptor.type) {
//			case BALLANDSOCKET:
//				return visit(descriptor.ballAndSocket);
//			case HINGE:
//				return visit(descriptor.hinge);
//			case LIMITED_HINGE:
//				return visit(descriptor.limitedHinge);
//			case PRISMATIC:
//				return visit(descriptor.prismatic);
//			case RAGDOLL:
//				return visit(descriptor.ragdoll);
//			case STIFFSPRING:
//				return visit(descriptor.stiffSpring);
//			case MALLEABLE:
//				break;
//		}
//		
//		return NULL;
//	}
//
//	virtual hkpConstraintData* visit(HingeDescriptor& descriptor) {
//		hkpHingeConstraintData* data = new hkpHingeConstraintData();
//		data->setInBodySpace(
//			TOVECTOR4(descriptor.pivotA * 7),
//			TOVECTOR4(descriptor.pivotB * 7),
//			TOVECTOR4(descriptor.axleA),
//			TOVECTOR4(descriptor.axleB)
//		);
//		return data;
//	}
//	virtual hkpConstraintData* visit(LimitedHingeDescriptor& descriptor){
//		hkpLimitedHingeConstraintData* data = new hkpLimitedHingeConstraintData();
//		data->setInBodySpace(
//			TOVECTOR4(descriptor.pivotA * 7),
//			TOVECTOR4(descriptor.pivotB * 7),
//			TOVECTOR4(descriptor.axleA),
//			TOVECTOR4(descriptor.axleB),
//			TOVECTOR4(descriptor.perp2AxleInA1),
//			TOVECTOR4(descriptor.perp2AxleInB1)
//		);
//		return data;
//	}
//
//	ConstraintBuilder(vector<bhkBlendCollisionObjectRef>& nbodies, hkArray<hkpRigidBody*>& hkbodies) : ConstraintVisitor(nbodies,hkbodies) {}
//};

template<typename PropertyType, typename input>
void set_property(FbxSurfaceMaterial* material, const char* name, input value, PropertyType T)
{
	FbxProperty p = material->FindProperty(name);
	if (!p.IsValid())
	{
		p = FbxProperty::Create(material, T, name);
		p.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
	}
	p.Set(value);
};

bool valid = false;

template<typename Output>
Output get_property(FbxSurfaceMaterial* material, const char* name)
{
	FbxProperty p = material->FindProperty(name);
	if (p.IsValid())
	{
		valid = true;
		return p.Get<Output>();
	}
	return Output();
};

enum gl_blend_modes {
	GL_ONE = 0,
	GL_ZERO = 1,
	GL_SRC_COLOR = 2,
	GL_ONE_MINUS_SRC_COLOR = 3,
	GL_DST_COLOR = 4,
	GL_ONE_MINUS_DST_COLOR = 5,
	GL_SRC_ALPHA = 6,
	GL_ONE_MINUS_SRC_ALPHA = 7,
	GL_DST_ALPHA = 8,
	GL_ONE_MINUS_DST_ALPHA = 9,
	GL_SRC_ALPHA_SATURATE = 10
};

enum gl_test_modes {
	GL_ALWAYS = 0,
	GL_LESS = 1,
	GL_EQUAL = 2,
	GL_LEQUAL = 3,
	GL_GREATER = 4,
	GL_NOTEQUAL = 5,
	GL_GEQUAL = 6,
	GL_NEVER = 7
};

union alpha_flags_modes
{
	struct {
		// Bit 0 : color blending enable
		unsigned color_blending_enable : 1;
		// Bits 1-4 : source blend mode
		unsigned source_blend_mode : 4;
		// Bits 5-8 : destination blend mode
		unsigned destination_blend_mode : 4;
		// Bit 9 : alpha test enable
		unsigned alpha_test_enable : 1;
		// Bit 10-12 : alpha test mode
		unsigned alpha_test_mode : 3;
		// Bit 13 : no sorter flag ( disables triangle sorting )
		unsigned no_sorter_flag : 1;
	} bits;
	unsigned int value;
};

class AlphaFlagsHandler
{
	const char* gl_blend_modes_to_string(gl_blend_modes mode)
	{
		switch (mode)
		{
		case GL_ONE: return "ONE";
		case GL_ZERO: return "ZERO";
		case GL_SRC_COLOR: return "SRC_COLOR";
		case GL_ONE_MINUS_SRC_COLOR: return "ONE_MINUS_SRC_COLOR";
		case GL_DST_COLOR: return "DST_COLOR";
		case GL_ONE_MINUS_DST_COLOR: return "ONE_MINUS_DST_COLOR";
		case GL_SRC_ALPHA: return "SRC_ALPHA";
		case GL_ONE_MINUS_SRC_ALPHA: return "ONE_MINUS_SRC_ALPHA";
		case GL_DST_ALPHA: return "DST_ALPHA";
		case GL_ONE_MINUS_DST_ALPHA: return "ONE_MINUS_DST_ALPHA";
		case GL_SRC_ALPHA_SATURATE: return "SRC_ALPHA_SATURATE";
		default: return "ONE";
		}
		return "ONE";
	}

	gl_blend_modes gl_blend_modes_to_value(const string& mode)
	{
		if (mode == "GL_ONE") return GL_ONE;
		if (mode == "ZERO") return GL_ZERO;
		if (mode == "SRC_COLOR") return GL_SRC_COLOR;
		if (mode == "ONE_MINUS_SRC_COLOR") return GL_ONE_MINUS_SRC_COLOR;
		if (mode == "DST_COLOR") return GL_DST_COLOR;
		if (mode == "ONE_MINUS_DST_COLOR") return GL_ONE_MINUS_DST_COLOR;
		if (mode == "SRC_ALPHA") return GL_SRC_ALPHA;
		if (mode == "ONE_MINUS_SRC_ALPHA") return GL_ONE_MINUS_SRC_ALPHA;
		if (mode == "DST_ALPHA") return GL_DST_ALPHA;
		if (mode == "ONE_MINUS_DST_ALPHA") return GL_ONE_MINUS_DST_ALPHA;
		if (mode == "SRC_ALPHA_SATURATE") return GL_SRC_ALPHA_SATURATE;
		return GL_ONE;
	}

	const char* gl_test_modes_to_string(gl_test_modes mode)
	{
		switch (mode)
		{
		case GL_ALWAYS: return "ALWAYS";
		case GL_LESS: return "LESS";
		case GL_EQUAL: return "EQUAL";
		case GL_LEQUAL: return "LEQUAL";
		case GL_GREATER: return "GREATER";
		case GL_NOTEQUAL: return "NOTEQUAL";
		case GL_GEQUAL: return "GEQUAL";
		case GL_NEVER: return "NEVER";
		default: return "ALWAYS";
		}
		return "ALWAYS";
	}

	gl_test_modes gl_test_modes_to_value(const string& mode)
	{
		if (mode == "ALWAYS") return GL_ALWAYS;
		if (mode == "LESS") return GL_LESS;
		if (mode == "EQUAL") return GL_EQUAL;
		if (mode == "LEQUAL") return GL_LEQUAL;
		if (mode == "GREATER") return GL_GREATER;
		if (mode == "NOTEQUAL") return GL_NOTEQUAL;
		if (mode == "GEQUAL") return GL_GEQUAL;
		if (mode == "NEVER") return GL_NEVER;
		return GL_ALWAYS;
	}

	alpha_flags_modes modes;
	byte threshold;

public:


	AlphaFlagsHandler(NiAlphaPropertyRef alpha)
	{
		modes.value = alpha->GetFlags();
		threshold = alpha->GetThreshold();
	}

	void add_to_node(FbxSurfaceMaterial* material)
	{
		set_property(material, "color_blending_enable", FbxBool(modes.bits.color_blending_enable), FbxBoolDT);
		set_property(material, "source_blend_mode", FbxString(gl_blend_modes_to_string((gl_blend_modes)modes.bits.source_blend_mode)), FbxStringDT);
		set_property(material, "destination_blend_mode", FbxString(gl_blend_modes_to_string((gl_blend_modes)modes.bits.destination_blend_mode)), FbxStringDT);
		set_property(material, "alpha_test_enable", FbxBool(modes.bits.alpha_test_enable), FbxBoolDT);
		set_property(material, "alpha_test_mode", FbxString(gl_test_modes_to_string((gl_test_modes)modes.bits.alpha_test_mode)), FbxStringDT);
		set_property(material, "no_sorter_flag", FbxBool(modes.bits.no_sorter_flag), FbxBoolDT);
		set_property(material, "alpha_test_threshold", threshold, FbxShortDT); //blender
	}

	AlphaFlagsHandler(FbxSurfaceMaterial* material)
	{
		modes.value = 0;
		modes.bits.color_blending_enable = get_property<FbxBool>(material, "color_blending_enable");
		modes.bits.source_blend_mode = gl_blend_modes_to_value(get_property<FbxString>(material, "source_blend_mode").Buffer());
		modes.bits.destination_blend_mode = gl_blend_modes_to_value(get_property<FbxString>(material, "destination_blend_mode").Buffer());
		modes.bits.alpha_test_enable = get_property<FbxBool>(material, "alpha_test_enable");
		modes.bits.alpha_test_mode = gl_test_modes_to_value(get_property<FbxString>(material, "alpha_test_mode").Buffer());
		modes.bits.no_sorter_flag = get_property<FbxBool>(material, "no_sorter_flag");
		threshold = (byte)get_property<FbxShort>(material, "alpha_test_threshold");
	}

	NiAlphaPropertyRef to_property()
	{
		if (valid && modes.value != 0)
		{
			NiAlphaPropertyRef alpha = new NiAlphaProperty();
			alpha->SetFlags(modes.value);
			alpha->SetThreshold(threshold);
			return alpha;
		}
		return NULL;
	}

};


class FBXBuilderVisitor : public RecursiveFieldVisitor<FBXBuilderVisitor> {
	const NifInfo& this_info;
	const string& texturePath;
	FbxScene& scene;
	deque<FbxNode*> build_stack;
	set<void*>& alreadyVisitedNodes;
	map<NiSkinInstance*, NiTriBasedGeom*> skins;
	map<NiSkinInstance*, FbxNode*> fbx_meshes_skin_parent;
	map<bhkRigidBodyRef, FbxNode*> bodies;
	set<NiControllerManager*>& managers;
	NifFile& nif_file;
	FbxAnimStack* lAnimStack = NULL;
	string root_name;
	double bhkScaleFactor = 1.0;

	FbxFileTexture* create_texture(const char* texture_type, const string& texture_path, const FbxFileTexture::ETextureUse use = FbxTexture::eStandard)
	{
		FbxFileTexture* lTexture = FbxFileTexture::Create(&scene, texture_type);
		// Set texture properties.
		fs::path out_path = fs::path(texturePath) / texture_path;
		lTexture->SetFileName(out_path.string().c_str()); // Resource file is in current directory.		
		lTexture->SetTextureUse(use);
		lTexture->SetMappingType(FbxTexture::eUV);
		lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
		lTexture->SetSwapUV(false);
		lTexture->SetTranslation(0.0, 0.0);
		lTexture->SetScale(1.0, 1.0);
		lTexture->SetRotation(0.0, 0.0);
		return lTexture;
	}

	FbxSurfaceMaterial* create_material(const string& name, BSLightingShaderPropertyRef material_property, NiAlphaPropertyRef alpha)
	{
		if (material_property != NULL)
		{
			alreadyVisitedNodes.insert(material_property);

			string lShadingName = "Phong";
			string m_name = name + "_material";
			sanitizeString(m_name);
			FbxDouble3 lDiffuse(1.0, 1.0, 1.0);
			FbxDouble3 lBlack(0.0, 0.0, 0.0);
			FbxSurfacePhong* gMaterial = FbxSurfacePhong::Create(scene.GetFbxManager(), m_name.c_str());
			//The following properties are used by the Phong shader to calculate the color for each pixel in the material :

			// Generate primary and secondary colors.
			Color3 nif_emissive_color = material_property->GetEmissiveColor();
			gMaterial->Emissive = { nif_emissive_color.r, nif_emissive_color.g, nif_emissive_color.b };
			gMaterial->EmissiveFactor = material_property->GetEmissiveMultiple();

			
			//Specular
			Color3 nif_specular_color = material_property->GetSpecularColor();
			gMaterial->Specular = { nif_specular_color.r, nif_specular_color.g, nif_specular_color.b };
			gMaterial->SpecularFactor.Set(material_property->GetSpecularStrength());

			//Diffuse
			gMaterial->Diffuse = lDiffuse;

			//Ambient
			gMaterial->Ambient = lBlack;
			gMaterial->AmbientFactor.Set(1.);

			gMaterial->Shininess = material_property->GetGlossiness();
			gMaterial->ShadingModel.Set(lShadingName.c_str());

			//Environment
			FbxProperty shader_type = gMaterial->FindProperty("shader_type");
			if (!shader_type.IsValid())
			{
				shader_type = FbxProperty::Create(gMaterial, FbxStringDT, "shader_type");
				shader_type.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
			}
			shader_type.Set(FbxString(NifFile::shader_type_name(material_property->GetSkyrimShaderType())));

			FbxProperty environment_map_scale = gMaterial->FindProperty("environment_map_scale");
			if (!environment_map_scale.IsValid())
			{
				environment_map_scale = FbxProperty::Create(gMaterial, FbxDoubleDT, "environment_map_scale");
				environment_map_scale.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
			}
			environment_map_scale.Set(FbxDouble(material_property->GetEnvironmentMapScale()));

			if (material_property->GetTextureSet() != NULL)
			{
				alreadyVisitedNodes.insert(material_property->GetTextureSet());
				vector<string>& texture_set = material_property->GetTextureSet()->GetTextures();
				if (!texture_set.empty())
				{
					if (!texture_set[0].empty())
					{
						FbxFileTexture* diffuse = create_texture(FbxSurfaceMaterial::sDiffuse, texture_set[0]);
						diffuse->SetTranslation(material_property->GetUvOffset().u, material_property->GetUvOffset().v);
						diffuse->SetScale(material_property->GetUvScale().u, material_property->GetUvScale().v);
						switch (material_property->GetTextureClampMode())
						{
						case CLAMP_S_CLAMP_T: 
							/*!< Clamp in both directions. */
							diffuse->SetWrapMode(FbxTexture::EWrapMode::eClamp, FbxTexture::EWrapMode::eClamp);
							break;
						case CLAMP_S_WRAP_T:
							/*!< Clamp in the S(U) direction but wrap in the T(V) direction. */
							diffuse->SetWrapMode(FbxTexture::EWrapMode::eClamp, FbxTexture::EWrapMode::eRepeat);
							break;
						case WRAP_S_CLAMP_T:
							/*!< Wrap in the S(U) direction but clamp in the T(V) direction. */
							diffuse->SetWrapMode(FbxTexture::EWrapMode::eRepeat, FbxTexture::EWrapMode::eClamp);
							break;
						case WRAP_S_WRAP_T:
							/*!< Wrap in both directions. */
							diffuse->SetWrapMode(FbxTexture::EWrapMode::eRepeat, FbxTexture::EWrapMode::eRepeat);
							break;
						default:
							break;
						};

						diffuse->Alpha = material_property->GetAlpha();

						if (diffuse && gMaterial)
						{
							gMaterial->Diffuse.ConnectSrcObject(diffuse);
							if (alpha != NULL)
							{
								alreadyVisitedNodes.insert(&*alpha);
								//TODO: also find a way in fbx to make them look right in 3d suites
								AlphaFlagsHandler(alpha).add_to_node(gMaterial);
							}
						}
					}
					if (!texture_set[1].empty())
					{
						FbxFileTexture* normal = create_texture(FbxSurfaceMaterial::sNormalMap, texture_set[1] );
						if (normal && gMaterial)
							gMaterial->NormalMap.ConnectSrcObject(normal);
					}
					for (int i = 2; i < 9; i++)
					{
						if (!texture_set[i].empty())
						{
							string slot_name = "slot" + to_string(i+1);
							FbxFileTexture* additional_texture = create_texture(slot_name.c_str(), texture_set[i+1]);
							FbxProperty texture_slot = gMaterial->FindProperty(slot_name.c_str());
							if (!texture_slot.IsValid())
							{
								texture_slot = FbxProperty::Create(gMaterial, FbxStringDT, slot_name.c_str());
								texture_slot.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
							}
							texture_slot.Set(FbxString(additional_texture->GetFileName()));
						}
					}
				}
			}

			//shader flags save
			set_property(gMaterial, "shader_flags_1", FbxInt(material_property->GetShaderFlags1_sk()), FbxIntDT);
			set_property(gMaterial, "shader_flags_2", FbxInt(material_property->GetShaderFlags2_sk()), FbxIntDT);


			return gMaterial;
		}
		return NULL;
	}

	template<typename shape_type, const unsigned int>
	FbxSurfaceMaterial* extract_Material(shape_type& shape)
	{
		return NULL;
	}

	template<>
	FbxSurfaceMaterial* extract_Material<NiTriStrips, VER_20_2_0_7>(NiTriStrips& shape)
	{
		return create_material(shape.GetName(), DynamicCast<BSLightingShaderProperty>(shape.GetShaderProperty()), shape.GetAlphaProperty());
	}

	template<>
	FbxSurfaceMaterial* extract_Material<NiTriShape, VER_20_2_0_7>(NiTriShape& shape)
	{
		return create_material(shape.GetName(), DynamicCast<BSLightingShaderProperty>(shape.GetShaderProperty()), shape.GetAlphaProperty());
	}

	FbxNode* AddGeometry(FbxNode* parent, NiTriStrips& node) {
		string shapeName = node.GetName();
		sanitizeString(shapeName);

		if (node.GetData() == NULL) return parent;

		const vector<Vector3>& verts = node.GetData()->GetVertices();
		const vector<Vector3>& norms = node.GetData()->GetNormals();

		vector<Triangle>& tris = vector<Triangle>(0);
		vector<TexCoord>& uvs = vector<TexCoord>(0);
		vector<Color4>& vcs = vector<Color4>(0);

		if (node.GetData()->IsSameType(NiTriStripsData::TYPE)) {
			NiTriStripsDataRef ref = DynamicCast<NiTriStripsData>(node.GetData());
			tris = triangulate(ref->GetPoints());
			if (!ref->GetUvSets().empty())
				uvs = ref->GetUvSets()[0];
			if (!ref->GetVertexColors().empty())
				vcs = ref->GetVertexColors();
		}

		alreadyVisitedNodes.insert(node.GetData());

		//defer skin
		if (node.GetSkinInstance() != NULL)
		{
			fbx_meshes_skin_parent[node.GetSkinInstance()] = parent;
			alreadyVisitedNodes.insert(node.GetSkinInstance());
			if (node.GetSkinInstance()->GetData() != NULL)
				alreadyVisitedNodes.insert(node.GetSkinInstance()->GetData());
			if (node.GetSkinInstance()->GetSkinPartition() != NULL)
				alreadyVisitedNodes.insert(node.GetSkinInstance()->GetSkinPartition());
			skins[node.GetSkinInstance()] = &node;
		}


		FbxSurfaceMaterial* material = extract_Material<NiTriStrips, VER_20_2_0_7>(node);
		//int material_index = -1;
		//if (material != NULL)
		//	material_index = parent->AddMaterial(material);

		return AddGeometry(parent, shapeName, verts, norms, tris, uvs, vcs, material, getTransform(&node));
	}

	FbxNode* AddGeometry(FbxNode* parent, NiTriShape& node) {

		string shapeName = node.GetName();
		sanitizeString(shapeName);

		if (node.GetData() == NULL) return parent;

		const vector<Vector3>& verts = node.GetData()->GetVertices();
		const vector<Vector3>& norms = node.GetData()->GetNormals();

		vector<Triangle>& tris = vector<Triangle>(0);
		vector<TexCoord>& uvs = vector<TexCoord>(0);
		vector<Color4>& vcs = vector<Color4>(0);

		if (node.GetData()->IsSameType(NiTriShapeData::TYPE)) {
			NiTriShapeDataRef ref = DynamicCast<NiTriShapeData>(node.GetData());
			tris = ref->GetTriangles();
			if (!ref->GetUvSets().empty())
				uvs = ref->GetUvSets()[0];
			if (!ref->GetVertexColors().empty())
				vcs = ref->GetVertexColors();
		}

		alreadyVisitedNodes.insert(node.GetData());

		//defer skin
		if (node.GetSkinInstance() != NULL)
		{
			fbx_meshes_skin_parent[node.GetSkinInstance()] = parent;
			alreadyVisitedNodes.insert(node.GetSkinInstance());
			if (node.GetSkinInstance()->GetData() != NULL)
				alreadyVisitedNodes.insert(node.GetSkinInstance()->GetData());
			if (node.GetSkinInstance()->GetSkinPartition() != NULL)
				alreadyVisitedNodes.insert(node.GetSkinInstance()->GetSkinPartition());
			skins[node.GetSkinInstance()] = &node;
		}

		if (verts.empty())
			return FbxNode::Create(&scene, shapeName.c_str());

		FbxSurfaceMaterial* material = extract_Material<NiTriShape, VER_20_2_0_7>(node);
		//int material_index = -1;
		//if (material != NULL)
		//	material_index = parent->AddMaterial(material);

		return AddGeometry(parent, shapeName, verts, norms, tris, uvs, vcs, material, getTransform(&node));
	}

	FbxNode* AddGeometry(FbxNode* parent, const string& shapeName, 
							const vector<Vector3>& verts,
							const vector<Vector3>& norms,
							const vector<Triangle>& tris,
							vector<TexCoord>& uvs, 
							vector<Color4>& vcs, 
							FbxSurfaceMaterial* material,
							FbxAMatrix& geometry_transform) {

		FbxMesh* m = FbxMesh::Create(&scene, shapeName.c_str());

		FbxGeometryElementNormal* normElement = nullptr;
		if (!norms.empty()) {
			normElement = m->CreateElementNormal();
			normElement->SetMappingMode(FbxLayerElement::eByControlPoint);
			normElement->SetReferenceMode(FbxLayerElement::eDirect);
		}

		FbxGeometryElementUV* uvElement = nullptr;
		if (!uvs.empty()) {
			std::string uvName = shapeName + "UV";
			uvElement = m->CreateElementUV(uvName.c_str());
			uvElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
			uvElement->SetReferenceMode(FbxGeometryElement::eDirect);
		}

		FbxGeometryElementVertexColor* vcElement = nullptr;
		if (!vcs.empty()) {
			vcElement = m->CreateElementVertexColor();
			vcElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
			vcElement->SetReferenceMode(FbxGeometryElement::eDirect);
		}

		m->InitControlPoints(verts.size());
		FbxVector4* points = m->GetControlPoints();

		for (int i = 0; i < m->GetControlPointsCount(); i++) {
			points[i] = geometry_transform.MultT(FbxVector4(verts[i].x, verts[i].y, verts[i].z));

			if (normElement)
				normElement->GetDirectArray().Add(FbxVector4(norms[i].x, norms[i].y, norms[i].z));
			if (uvElement)
				uvElement->GetDirectArray().Add(FbxVector2(uvs[i].u, 1 - uvs[i].v));
			if (vcElement)
				vcElement->GetDirectArray().Add(FbxColor(vcs[i].r, vcs[i].g, vcs[i].b, vcs[i].a));
		}

		FbxNode* local_parent = NULL;
		if (parent == parent->GetScene()->GetRootNode())
		{
			//seems like FBX doesn't like meshes added to root
			string dummy_name = shapeName + "_support";
			local_parent = FbxNode::Create(&scene, dummy_name.c_str());
			parent->AddChild(local_parent);
		}
		else {
			local_parent = parent;
		}

		int material_index = -1;  

		if (material != NULL)
		{
			material_index = local_parent->AddMaterial(material);
			m->InitMaterialIndices(FbxLayerElement::EMappingMode::eAllSame);
			int count = m->GetElementMaterialCount();
			FbxLayerElementMaterial* layerElement = m->GetElementMaterial();
			FbxLayerElementArrayTemplate<int>& iarray = layerElement->GetIndexArray();
			iarray.SetAt(0, material_index);
		}
		if (!tris.empty()) {
			for (auto &t : tris) {
				m->BeginPolygon(-1);
				m->AddPolygon(t.v1);
				m->AddPolygon(t.v2);
				m->AddPolygon(t.v3);
				m->EndPolygon();
			}
		}

		//if (parent == parent->GetScene()->GetRootNode())
		//{
		//	//seems like FBX doesn't like meshes added to root
		//	string dummy_name = shapeName + "_support";
		//	FbxNode* dummy = FbxNode::Create(&scene, dummy_name.c_str());
		//	dummy->AddNodeAttribute(m);
		//	parent->AddChild(dummy);
		//}
		//else {
		//	parent->AddNodeAttribute(m);
		//}
		local_parent->AddNodeAttribute(m);
		return parent;
	}

	FbxNode* setNullTransform(FbxNode* node) {
		node->LclTranslation.Set(FbxDouble3(0.0,0.0,0.0));
		node->LclRotation.Set(FbxVector4(0.0,0.0,0.0));
		node->LclScaling.Set(FbxDouble3(1.0,1.0,1.0));

		return node;
	}

	FbxNode* setTransform(NiAVObject* av, FbxNode* node) {
		Vector3 translation = av->GetTranslation();
		//
		node->LclTranslation.Set(FbxDouble3(translation.x, translation.y, translation.z));

		Quaternion rotation = av->GetRotation().AsQuaternion();
		Quat QuatTest = { rotation.x, rotation.y, rotation.z, rotation.w };
		EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
		node->LclRotation.Set(FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z)));
		node->LclScaling.Set(FbxDouble3(av->GetScale(), av->GetScale(), av->GetScale()));

		return node;
	}

	template<typename T>
	FbxNode* getBuiltNode(T& obj) {
		NiObject* node = (NiObject*)&obj;
		if (node->IsDerivedType(NiAVObject::TYPE))
			return getBuiltNode(string(DynamicCast<NiAVObject>(node)->GetName().c_str()));
		return NULL;
	}

	template<typename T>
	FbxNode* getBuiltNode(Ref<T>& obj) {
		NiObjectRef node = (NiObjectRef)obj;
		if (node->IsDerivedType(NiAVObject::TYPE))
			return getBuiltNode(string(DynamicCast<NiAVObject>(node)->GetName().c_str()));
		return NULL;
	}


	template<>
	inline FbxNode* getBuiltNode(NiNode*& node) {
		return getBuiltNode(string(node->GetName().c_str()));
	}

	template<>
	inline FbxNode* getBuiltNode(string& name) {
		string sanitized = name; sanitizeString(sanitized);
		if (name == root_name || sanitized == root_name)
			return scene.GetRootNode();
		FbxNode* result = scene.FindNodeByName(name.c_str());
		if (result == NULL)
			result = scene.FindNodeByName(sanitized.c_str());
		if (result == NULL)
			result = scene.FindNodeByName((sanitized + "_support").c_str());
		return result;
	}

	template<>
	inline FbxNode* getBuiltNode(const string& name) {
		string sanitized = name; sanitizeString(sanitized);
		if (name == root_name || sanitized == root_name)
			return scene.GetRootNode();
		FbxNode* result = scene.FindNodeByName(name.c_str());
		if (result == NULL)
			result = scene.FindNodeByName(sanitized.c_str());
		if (result == NULL)
			result = scene.FindNodeByName((sanitized + "_support").c_str());
		return result;
	}

	void processSkins() {
		if (skins.empty())
			return;
		for (pair<NiSkinInstance*, NiTriBasedGeom*> skin : skins) {
			NiTriBasedGeomRef mesh = skin.second;
			string shapeName = mesh->GetName();
			sanitizeString(shapeName);
			NiSkinDataRef data = skin.first->GetData();
			NiSkinPartitionRef part = skin.first->GetSkinPartition();
			vector<BoneData>& bonelistdata = data->GetBoneList();
			std::string shapeSkin = shapeName + "_skin";


			vector<NiNode * > data_bonelist = skin.first->GetBones();

			int partIndex = 0;
			for (const auto& part_data : part->GetSkinPartitionBlocks())
			{
				FbxSkin* fbx_skin = FbxSkin::Create(&scene, shapeSkin.c_str());
				map<unsigned short, FbxCluster*> clusters;
				//create clusters
				for (unsigned short bone_index : part_data.bones) {
					NiNode* bone = skin.first->GetBones()[bone_index];
					FbxNode* jointNode = getBuiltNode(bone);
					if (jointNode) {
						std::string boneSkin = bone->GetName() + "_cluster";
						FbxCluster* aCluster = FbxCluster::Create(&scene, boneSkin.c_str());
						aCluster->SetLink(jointNode);
						aCluster->SetLinkMode(FbxCluster::eTotalOne);

						Vector3 translation = data->GetSkinTransform().translation;
						Quaternion rotation = data->GetSkinTransform().rotation.AsQuaternion();
						float scale = data->GetSkinTransform().scale;

						FbxAMatrix global_transform;
						global_transform.SetT(FbxDouble3(translation.x, translation.y, translation.z));
						global_transform.SetQ({ rotation.x, rotation.y, rotation.z, rotation.w });
						global_transform.SetS(FbxDouble3(scale, scale, scale));

						FbxAMatrix joint_transform = jointNode->EvaluateGlobalTransform();

						aCluster->SetTransformLinkMatrix(joint_transform);
						aCluster->SetTransformMatrix(global_transform.Inverse());

						clusters[bone_index] = aCluster;
					}
				}

				for (int v = 0; v < part_data.vertexMap.size(); v++)
				{
					for (int b = 0; b < part_data.boneIndices[v].size(); b++)
					{
						unsigned short bone_index = part_data.bones[part_data.boneIndices[v][b]];
						FbxCluster* cluster = clusters[bone_index];
						unsigned short vertex_index = part_data.vertexMap[v];
						float vertex_weight = part_data.vertexWeights[v][b];
						cluster->AddControlPointIndex(vertex_index, vertex_weight);
					}
				}

				for (const auto& cluster : clusters)
				{
					fbx_skin->AddCluster(cluster.second);
				}
				FbxNode* skin_parent = fbx_meshes_skin_parent[skin.first]; scene.FindNodeByName(mesh->GetName().c_str());
				if (NULL != skin_parent)
				{
					string dummy_name = shapeName + "_support";
					//search for support node
					auto skin_support = skin_parent->FindChild(dummy_name.c_str(), false);
					if (skin_support != NULL)
						skin_parent = skin_support;
					for (int i = 0; i < skin_parent->GetNodeAttributeCount(); i++)
					{
						if (FbxNodeAttribute::eMesh == skin_parent->GetNodeAttributeByIndex(i)->GetAttributeType())
						{
							FbxMesh* m_i = (FbxMesh*)skin_parent->GetNodeAttributeByIndex(i);
							string m_i_name = m_i->GetName();
							if (m_i_name == shapeName)
							{
								m_i->AddDeformer(fbx_skin);
								break;
							}
						}
					}
				}
			}
		}
	}

	string getPalettedString(NiStringPaletteRef nipalette, unsigned int offset) {
		StringPalette s_palette = nipalette->GetPalette();
		const char* c_palette = s_palette.palette.c_str();
		for (size_t i = offset; i < s_palette.length; i++)
		{
			if (c_palette[i] == 0)
				return string(&c_palette[offset], &c_palette[i]);
		}
		return "";
	}

	template<typename interpolatorT>
	void addTrack(interpolatorT& interpolator, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		//		throw runtime_error("addTrack: Unsupported interpolator type!");
	}


	// For reference
	//enum KeyType {
	//	LINEAR_KEY = 1, /*!< Use linear interpolation. */
	//	QUADRATIC_KEY = 2, /*!< Use quadratic interpolation.  Forward and back tangents will be stored. */
	//	TBC_KEY = 3, /*!< Use Tension Bias Continuity interpolation.  Tension, bias, and continuity will be stored. */
	//	XYZ_ROTATION_KEY = 4, /*!< For use only with rotation data.  Separate X, Y, and Z keys will be stored instead of using quaternions. */
	//	CONST_KEY = 5, /*!< Step function. Used for visibility keys in NiBoolData. */
	//};


	template<typename T>
	bool getFieldIsValid(T* object, unsigned int field) {
		vector<unsigned int> valid_translations_fields = object->GetValidFieldsIndices(this_info);
		return find(valid_translations_fields.begin(), valid_translations_fields.end(), field)
			!= valid_translations_fields.end();
	}

	void addKeys(KeyGroup<Vector3>& translations, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		if (translations.numKeys > 0) {
			FbxAnimCurve* lCurve_Trans_X = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			FbxAnimCurve* lCurve_Trans_Y = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			FbxAnimCurve* lCurve_Trans_Z = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			FbxAnimCurveDef::EInterpolationType translation_interpolation_type = FbxAnimCurveDef::eInterpolationLinear;

			if (getFieldIsValid(&translations, KeyGroup<Vector3>::FIELDS::interpolation)) {
				switch (translations.interpolation) {
				case CONST_KEY:
					translation_interpolation_type = FbxAnimCurveDef::eInterpolationConstant;
					break;
				case LINEAR_KEY:
					translation_interpolation_type = FbxAnimCurveDef::eInterpolationLinear;
					break;
				case QUADRATIC_KEY:
					translation_interpolation_type = FbxAnimCurveDef::eInterpolationCubic;
					break;
				}
			}

			for (const Key<Vector3>& key : translations.keys) {

				FbxTime lTime;
				//key.time
				lTime.SetSecondDouble(key.time);

				lCurve_Trans_X->KeyModifyBegin();
				int lKeyIndex = lCurve_Trans_X->KeyAdd(lTime);
				lCurve_Trans_X->KeySetValue(lKeyIndex, key.data.x);
				lCurve_Trans_X->KeySetInterpolation(lKeyIndex, translation_interpolation_type);
				lCurve_Trans_X->KeyModifyEnd();

				lCurve_Trans_Y->KeyModifyBegin();
				lKeyIndex = lCurve_Trans_Y->KeyAdd(lTime);
				lCurve_Trans_Y->KeySetValue(lKeyIndex, key.data.y);
				lCurve_Trans_Y->KeySetInterpolation(lKeyIndex, translation_interpolation_type);
				lCurve_Trans_Y->KeyModifyEnd();

				lCurve_Trans_Z->KeyModifyBegin();
				lKeyIndex = lCurve_Trans_Z->KeyAdd(lTime);
				lCurve_Trans_Z->KeySetValue(lKeyIndex, key.data.z);
				lCurve_Trans_Z->KeySetInterpolation(lKeyIndex, translation_interpolation_type);
				lCurve_Trans_Z->KeyModifyEnd();
			}
		}
	}
	void addKeys(const Niflib::array<3, KeyGroup<float > >& rotations, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		if (!rotations.empty()) {
			FbxAnimCurve* lCurve_Rot_X = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			FbxAnimCurve* lCurve_Rot_Y = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			FbxAnimCurve* lCurve_Rot_Z = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			Niflib::array<3, FbxAnimCurve* > curves;
			curves[0] = lCurve_Rot_X;
			curves[1] = lCurve_Rot_Y;
			curves[2] = lCurve_Rot_Z;

			for (int i = 0; i < 3; i++) {
				for (const Key<float>& key : rotations.at(i).keys) {

					FbxTime lTime;
					//key.time
					lTime.SetSecondDouble(key.time);

					curves[i]->KeyModifyBegin();
					int lKeyIndex = curves[i]->KeyAdd(lTime);
					curves[i]->KeySetValue(lKeyIndex, float(rad2deg(key.data)));
					curves[i]->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
					curves[i]->KeyModifyEnd();
				}
			}
		}
	}
	void addKeys(const vector<Key<Quaternion > >& rotations, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		if (!rotations.empty()) {
			FbxAnimCurve* lCurve_Rot_X = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			if (lCurve_Rot_X == NULL)
				lCurve_Rot_X = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			FbxAnimCurve* lCurve_Rot_Y = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			if (lCurve_Rot_Y == NULL)
				lCurve_Rot_Y = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			FbxAnimCurve* lCurve_Rot_Z = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			if (lCurve_Rot_Z == NULL)
				lCurve_Rot_Z = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			for (const Key<Quaternion>& key : rotations) {
				FbxTime lTime;
				//key.time
				lTime.SetSecondDouble(key.time);

				FbxQuaternion fbx_quat(key.data.x, key.data.y, key.data.z, key.data.w);
				FbxVector4 xyz = fbx_quat.DecomposeSphericalXYZ();

				lCurve_Rot_X->KeyModifyBegin();
				int lKeyIndex = lCurve_Rot_X->KeyAdd(lTime);
				lCurve_Rot_X->KeySetValue(lKeyIndex, xyz[0]);
				lCurve_Rot_X->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve_Rot_X->KeyModifyEnd();

				lCurve_Rot_Y->KeyModifyBegin();
				lKeyIndex = lCurve_Rot_Y->KeyAdd(lTime);
				lCurve_Rot_Y->KeySetValue(lKeyIndex, xyz[1]);
				lCurve_Rot_Y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve_Rot_Y->KeyModifyEnd();

				lCurve_Rot_Z->KeyModifyBegin();
				lKeyIndex = lCurve_Rot_Z->KeyAdd(lTime);
				lCurve_Rot_Z->KeySetValue(lKeyIndex, xyz[2]);
				lCurve_Rot_Z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve_Rot_Z->KeyModifyEnd();
			}
		}
	}
	void addKeys(const KeyGroup<float >& scales, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		if (!scales.keys.empty()) {
			FbxAnimCurve* lCurve_Scale_X = node->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			FbxAnimCurve* lCurve_Scale_Y = node->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			FbxAnimCurve* lCurve_Scale_Z = node->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			Niflib::array<3, FbxAnimCurve* > curves;
			curves[0] = lCurve_Scale_X;
			curves[1] = lCurve_Scale_Y;
			curves[2] = lCurve_Scale_Z;

			FbxAnimCurveDef::EInterpolationType translation_interpolation_type = FbxAnimCurveDef::eInterpolationLinear;

			if (getFieldIsValid(&scales, KeyGroup<Vector3>::FIELDS::interpolation)) {
				switch (scales.interpolation) {
				case CONST_KEY:
					translation_interpolation_type = FbxAnimCurveDef::eInterpolationConstant;
					break;
				case LINEAR_KEY:
					translation_interpolation_type = FbxAnimCurveDef::eInterpolationLinear;
					break;
				case QUADRATIC_KEY:
					translation_interpolation_type = FbxAnimCurveDef::eInterpolationCubic;
					break;
				}
			}

			for (const Key<float>& key : scales.keys) {
				FbxTime lTime;
				//key.time
				lTime.SetSecondDouble(key.time);

				for (int i = 0; i < 3; i++) {
					curves[i]->KeyModifyBegin();
					int lKeyIndex = curves[i]->KeyAdd(lTime);
					curves[i]->KeySetValue(lKeyIndex, float(key.data));
					curves[i]->KeySetInterpolation(lKeyIndex, translation_interpolation_type);
					if (translation_interpolation_type == FbxAnimCurveDef::eInterpolationCubic)
					{
						curves[i]->KeySetTangentMode(lKeyIndex, FbxAnimCurveDef::ETangentMode::eTangentBreak);
					}
					curves[i]->KeyModifyEnd();
				}
			}
		}
	}
	template<>
	void addTrack(NiTransformInterpolator& interpolator, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		//here we have an initial transform to be applied and a set of keys
		FbxAMatrix local = node->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
		NiQuatTransform interp_local = interpolator.GetTransform();
		//TODO: check these;
		NiTransformDataRef data = interpolator.GetData();

		if (data != NULL) {

			//translations:
			addKeys(data->GetTranslations(), node, lAnimLayer);

			//rotations:
			if (getFieldIsValid(&*data, NiKeyframeData::FIELDS::rotationType)) {
				if (data->GetRotationType() == XYZ_ROTATION_KEY) {
					//single float groups with tangents
					addKeys(data->GetXyzRotations(), node, lAnimLayer);
				}
				else {
					//threat as quaternion?
					addKeys(data->GetQuaternionKeys(), node, lAnimLayer);
				}
			}
			else {
				addKeys(data->GetQuaternionKeys(), node, lAnimLayer);
			}
			addKeys(data->GetScales(), node, lAnimLayer);
		}

	}

	void exportKFSequence(NiControllerSequenceRef ref) {
		lAnimStack = FbxAnimStack::Create(&scene, ref->GetName().c_str());
		FbxAnimLayer* layer = FbxAnimLayer::Create(&scene, "Default");
		for (ControlledBlock block : ref->GetControlledBlocks()) {
			//find the node
			string controlledBlockID;
			if (ref->GetStringPalette() != NULL) {
				controlledBlockID = getPalettedString(ref->GetStringPalette(), block.nodeNameOffset);
			}
			else {
				//default palette
				controlledBlockID = block.nodeName;
			}
			FbxNode* animatedNode = getBuiltNode(controlledBlockID/*, block.priority*/);

			if (animatedNode == NULL)
				throw runtime_error("exportKFSequence: Referenced node not found by name:" + controlledBlockID);
			//TODO: use palette here too
			//NiInterpolatorRef interpolator = block.interpolator;
			if (block.interpolator != NULL) {
				if (block.interpolator->IsDerivedType(NiTransformInterpolator::TYPE))
					addTrack(*DynamicCast<NiTransformInterpolator>(block.interpolator), animatedNode, layer);
				else
					addTrack(*block.interpolator, animatedNode, layer);
			}

		}
		lAnimStack->AddMember(layer);
	}

	void buildManagers() {
		//Usually the root node for animations sequences
		for (NiControllerManager* obj : managers) {
			//Next Controller can be ignored because it should be referred by individual sequences
			for (NiControllerSequenceRef ref : obj->GetControllerSequences()) {
				exportKFSequence(ref);
			}
		}
	}

public:

	FBXBuilderVisitor(NifFile& nif, FbxNode& sceneNode, FbxScene& scene, const NifInfo& info, const string& texture_path) :
		RecursiveFieldVisitor(*this, info),
		nif_file(nif),
		alreadyVisitedNodes(set<void*>()),
		this_info(info),
		scene(scene),
		managers(set<NiControllerManager*>()),
		texturePath(texture_path)
	{
		bhkScaleFactor = nif.GetBhkScaleFactor();
		NiNodeRef rootNode = DynamicCast<NiNode>(nif.GetRoot());
		if (rootNode != NULL)
		{

			build_stack.push_front(&sceneNode);
			root_name = rootNode->GetName();
			scene.GetRootNode()->SetName(root_name.c_str());
			rootNode->accept(*this, info);
			processSkins();
			buildManagers();
		}
	}


	virtual inline void start(NiObject& in, const NifInfo& info) {
	}

	virtual inline void end(NiObject& in, const NifInfo& info) {
		if (alreadyVisitedNodes.find(&in) == alreadyVisitedNodes.end()) {
			build_stack.pop_front();
			alreadyVisitedNodes.insert(&in);
		}
	}

	//Always insert into the stack to be consistant
	template<class T>
	inline void visit_object(T& obj) {
		if (alreadyVisitedNodes.find(&obj) == alreadyVisitedNodes.end()) {
			FbxNode* node = visit_new_object(obj);
			if (node != NULL)
				build_stack.push_front(node);
			else
				build_stack.push_front(build_stack.front());
		}
	}

	template<class T>
	FbxNode* visit_new_object(T& object) {
		FbxNode* parent = build_stack.front();
		FbxNode* current = getBuiltNode(object);
		if (current == NULL) {
			current = build(object, parent);
			parent->AddChild(current);
		}
		return current;
	}

	template<>
	FbxNode* visit_new_object(BSLODTriShape& obj) {
		FbxNode* parent = build_stack.front();
		AddGeometry(parent, *(NiTriShape*)&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiTriShape& obj) {
		FbxNode* parent = build_stack.front();
		AddGeometry(parent, obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiTriStrips& obj) {
		FbxNode* parent = build_stack.front();
		AddGeometry(parent, obj);
		return NULL;
	}

	//Deferred Already handled nodes
	template<>
	FbxNode* visit_new_object(NiControllerManager& obj) {
		alreadyVisitedNodes.insert(&obj);
		managers.insert(&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiMultiTargetTransformController& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiControllerSequence& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiTransformInterpolator& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiTransformData& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(NiDefaultAVObjectPalette& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	//Unuseful, will be recalculated on export
	template<>
	FbxNode* visit_new_object(BSXFlags& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	template<>
	FbxNode* visit_new_object(BSBound& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	//TODO: add them to build KF
	template<>
	FbxNode* visit_new_object(NiTextKeyExtraData& obj) {
		alreadyVisitedNodes.insert(&obj);
		return NULL;
	}

	inline int find_material_index(const vector<bhkCMSDMaterial>& materials, bhkCMSDMaterial& material)
	{
		for (int i = 0; i < materials.size(); i++)
			if (materials[i].material == material.material && materials[i].filter == material.filter)
				return i;
		return -1;
	}

	//Collisions. Fbx has no support for shapes, so we must generate a geometry out of them

	FbxNode* recursive_convert(bhkShape* shape, FbxNode* parent, HavokFilter layer)
	{
		//Containers
		hkGeometry geom;
		string parent_name = parent->GetName();
		vector<bhkCMSDMaterial> materials;
		//bhkConvexTransformShape, bhkTransformShape
		if (shape->IsDerivedType(bhkTransformShape::TYPE))
		{
			parent_name += "_transform";
			bhkTransformShapeRef transform_block = DynamicCast<bhkTransformShape>(shape);
			transform_block->GetTransform();
			FbxNode* transform_node = FbxNode::Create(&scene, parent_name.c_str());
			parent->AddChild(transform_node);
			//setMatTransform(transform_block->GetTransform(), transform_node);
			recursive_convert(transform_block->GetShape(), transform_node, layer);
		}
		//bhkListShape, /bhkConvexListShape
		else if (shape->IsDerivedType(bhkListShape::TYPE))
		{
			parent_name += "_list";
			bhkListShapeRef list_block = DynamicCast<bhkListShape>(shape);
			FbxNode* list_node = FbxNode::Create(&scene, parent_name.c_str());
			parent->AddChild(list_node);
			const vector<bhkShapeRef>& shapes = list_block->GetSubShapes();
			for (int i = 0; i < shapes.size(); i++) {
				string name_index = parent_name + "_" + to_string(i);
				//FbxNode* child_shape_node = FbxNode::Create(&scene, name_index.c_str());
				//parent->AddChild(child_shape_node);
				recursive_convert(shapes[i], list_node, layer);
			}
		}
		else if (shape->IsDerivedType(bhkConvexListShape::TYPE))
		{
			parent_name += "_convex_list";
			bhkConvexListShapeRef list_block = DynamicCast<bhkConvexListShape>(shape);
			FbxNode* convex_list = FbxNode::Create(&scene, parent_name.c_str());
			parent->AddChild(convex_list);
			const vector<bhkConvexShapeRef>& shapes = list_block->GetSubShapes();
			for (int i = 0; i < shapes.size(); i++) {
				string name_index = parent_name + "_" + to_string(i);
				//FbxNode* child_shape_node = FbxNode::Create(&scene, name_index.c_str());
				//parent->AddChild(child_shape_node);
				recursive_convert(shapes[i], convex_list, layer);
			}
		}
		//bhkMoppBvTree
		else if (shape->IsDerivedType(bhkMoppBvTreeShape::TYPE))
		{
			parent_name += "_mopp";
			bhkMoppBvTreeShapeRef moppbv = DynamicCast<bhkMoppBvTreeShape>(shape);
			if (moppbv->GetShape() != NULL)
			{
				alreadyVisitedNodes.insert(moppbv);
				FbxNode* mopp_node = FbxNode::Create(&scene, parent_name.c_str());
				parent->AddChild(mopp_node);
				recursive_convert(moppbv->GetShape(), mopp_node, layer);				
			}
		}
		//shapes
		//bhkSphereShape
		else if (shape->IsDerivedType(bhkSphereShape::TYPE))
		{
			parent_name += "_sphere";
			bhkSphereShapeRef sphere = DynamicCast<bhkSphereShape>(shape);
			hkpSphereShape hkSphere(sphere->GetRadius()/*/10.0*/);
			hkpShapeConverter::append(&geom, hkpShapeConverter::toSingleGeometry(&hkSphere));
			bhkCMSDMaterial material;
			material.material = sphere->GetMaterial().material_sk;
			material.filter = layer;
			int index = find_material_index(materials, material);
			if (index == -1)
			{
				index = materials.size();
				materials.push_back(material);
			}
			for (int i = 0; i < geom.m_triangles.getSize(); i++)
			{
				geom.m_triangles[i].m_material = index;
			}

		}
		//bhkBoxShape
		else if (shape->IsDerivedType(bhkBoxShape::TYPE))
		{
			parent_name += "_box";
			bhkBoxShapeRef box = DynamicCast<bhkBoxShape>(shape);
			hkpBoxShape hkBox(TOVECTOR4(box->GetDimensions()), box->GetRadius());
			hkpShapeConverter::append(&geom, hkpShapeConverter::toSingleGeometry(&hkBox));
			bhkCMSDMaterial material;
			material.material = box->GetMaterial().material_sk;
			material.filter = layer;
			int index = find_material_index(materials, material);
			if (index == -1)
			{
				index = materials.size();
				materials.push_back(material);
			}
			for (int i = 0; i < geom.m_triangles.getSize(); i++)
			{
				geom.m_triangles[i].m_material = index;
			}

		}
		//bhkCapsuleShape
		else if (shape->IsDerivedType(bhkCapsuleShape::TYPE))
		{
			parent_name += "_capsule";
			bhkCapsuleShapeRef capsule = DynamicCast<bhkCapsuleShape>(shape);
			hkpCapsuleShape hkCapsule(TOVECTOR4(capsule->GetFirstPoint()), TOVECTOR4(capsule->GetSecondPoint()), capsule->GetRadius());
			hkpShapeConverter::append(&geom, hkpShapeConverter::toSingleGeometry(&hkCapsule));
			bhkCMSDMaterial material;
			material.material = capsule->GetMaterial().material_sk;
			material.filter = layer;
			int index = find_material_index(materials, material);
			if (index == -1)
			{
				index = materials.size();
				materials.push_back(material);
			}
			for (int i = 0; i < geom.m_triangles.getSize(); i++)
			{
				geom.m_triangles[i].m_material = index;
			}
		}
		//bhkConvexVerticesShape
		else if (shape->IsDerivedType(bhkConvexVerticesShape::TYPE))
		{
			parent_name += "_convex";
			bhkConvexVerticesShapeRef convex = DynamicCast<bhkConvexVerticesShape>(shape);
			hkArray<hkVector4> vertices;
			for (const auto& vert : convex->GetVertices())
				vertices.pushBack(TOVECTOR4(vert));
			hkStridedVertices strided_vertices(vertices);
			hkArray<hkVector4> planeEquationsOut;
			hkGeometryUtility::createConvexGeometry(strided_vertices, geom, planeEquationsOut);
			bhkCMSDMaterial material;
			material.material = convex->GetMaterial().material_sk;
			material.filter = layer;
			int index = find_material_index(materials, material);
			if (index == -1)
			{
				index = materials.size();
				materials.push_back(material);
			}
			for (int i = 0; i < geom.m_triangles.getSize(); i++)
			{
				geom.m_triangles[i].m_material = index;
			}
		}
		//bhkCompressedMeshShape
		else if (shape->IsDerivedType(bhkCompressedMeshShape::TYPE))
		{
			parent_name += "_mesh";
			bhkCompressedMeshShapeRef cmesh = DynamicCast<bhkCompressedMeshShape>(shape);
			alreadyVisitedNodes.insert(cmesh);
			alreadyVisitedNodes.insert(cmesh->GetData());
			Accessor<bhkCompressedMeshShapeData> converter(DynamicCast<bhkCompressedMeshShape>(cmesh), geom, materials);
		}
		//create collision materials
		vector<FbxSurfaceMaterial*> to_be_added;
		for (const auto& material : materials)
		{
			string lMaterialName = NifFile::material_name(material.material);
			string collision_layer_name = NifFile::layer_name(material.filter.layer_sk);
			size_t materials_count = scene.GetMaterialCount();
			bool material_found = false;
			FbxSurfaceMaterial* m = NULL;
			for (int mm = 0; mm < materials_count; mm++)
			{
				m = scene.GetMaterial(mm);
				string m_name = m->GetName();
				FbxProperty layer = m->FindProperty("CollisionLayer");
				FbxString m_layer_v = layer.Get<FbxString>();
				string m_layer = m_layer_v.Buffer();
				if (m_name == lMaterialName && collision_layer_name == m_layer)
				{
					material_found = true;
					break;
				}
			}

			if (!material_found)
			{
				string lShadingName = "Phong";
				FbxDouble3 white = FbxDouble3(1, 1, 1);
				std::array<double, 3> m_color = NifFile::material_color(material.material);
				FbxDouble3 lcolor(m_color[0], m_color[1], m_color[2]);
				FbxDouble3 lDiffuseColor(0.75, 0.75, 0.0);
				FbxSurfacePhong* gMaterial = FbxSurfacePhong::Create(scene.GetFbxManager(), lMaterialName.c_str());
				//The following properties are used by the Phong shader to calculate the color for each pixel in the material :

				// Generate primary and secondary colors.
				gMaterial->Emissive = lcolor;
				gMaterial->TransparentColor = { 1.0,1.0,1.0 };
				gMaterial->TransparencyFactor = 0.9;
				gMaterial->Ambient = lcolor;
				gMaterial->Diffuse = lcolor;
				gMaterial->Shininess = 0.5;

				gMaterial->ShadingModel.Set(lShadingName.c_str());
				FbxProperty layer = FbxProperty::Create(gMaterial, FbxStringDT, "CollisionLayer");
				layer.Set(FbxString(collision_layer_name.c_str()));
				layer.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
				m = gMaterial;
			}
			to_be_added.push_back(m);
		}

		//pack
		if (geom.m_vertices.getSize() > 0)
		{
			FbxNode* shape_node = FbxNode::Create(&scene, parent_name.c_str());
			for (const auto& mm : to_be_added)
				shape_node->AddMaterial(mm);
			parent_name += "_geometry";
			FbxMesh* m = FbxMesh::Create(&scene, parent_name.c_str());
			m->InitControlPoints(geom.m_vertices.getSize());
			FbxVector4* points = m->GetControlPoints();

			for (int i = 0; i < m->GetControlPointsCount(); i++) {
				points[i] = TOFBXVECTOR4(geom.m_vertices[i])*bhkScaleFactor;
			}

			
			FbxGeometryElementMaterial* lMaterialElement = m->CreateElementMaterial();
			lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
			lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

			for (int i = 0; i < geom.m_triangles.getSize(); i++)
			{
				m->BeginPolygon(geom.m_triangles[i].m_material);
				m->AddPolygon(geom.m_triangles[i].m_a);
				m->AddPolygon(geom.m_triangles[i].m_b);
				m->AddPolygon(geom.m_triangles[i].m_c);
				m->EndPolygon();
			}
			
			shape_node->SetNodeAttribute(m);
			parent->AddChild(shape_node);
		}
		alreadyVisitedNodes.insert(shape);
		return parent;
	}

	class FbxConstraintBuilder {
		double bhkScaleFactor;

	protected:

		//TODO: pre 2010/660 import format!
		FbxNode* visit(RagdollDescriptor& descriptor, FbxNode* parent, FbxNode* child)
		{
			Vector4 row1 = descriptor.twistA;
			Vector4 row2 = descriptor.planeA;
			Vector4 row3 = descriptor.motorA;
			Vector4 row4 = descriptor.pivotA * bhkScaleFactor;
			row4.w = 1;

			Matrix44 matA = Matrix44(
				row1[0], row1[1], row1[2], row4[0],
				row2[0], row2[1], row2[2], row4[1],
				row3[0], row3[1], row3[2], row4[2],
				0.0f, 0.0f, 0.0f, row4[3]
			);

			Vector4 row1b = descriptor.twistB;
			Vector4 row2b = descriptor.planeB;
			Vector4 row3b = descriptor.motorB;
			Vector4 row4b = descriptor.pivotB * bhkScaleFactor;
			row4b.w = 1;

			Matrix44 matB = Matrix44(
				row1b[0], row1b[1], row1b[2], row4b[0],
				row2b[0], row2b[1], row2b[2], row4b[1],
				row3b[0], row3b[1], row3b[2], row4b[2],
				0.0f, 0.0f, 0.0f, row4b[3]
			);

			FbxNode* constraint_node = FbxNode::Create(parent->GetScene(), string(string(parent->GetName()) + "_con_" + string(child->GetName()) + "_attach_point").c_str());
			parent->AddChild(setMatTransform(matB, constraint_node));


			Quaternion rotation = matA.GetRotation().AsQuaternion();
			Quat QuatTest = { rotation.x, rotation.y, rotation.z, rotation.w };
			EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
			FbxVector4 fbx_rotation = FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z));

			FbxConstraintParent * fbx_constraint = FbxConstraintParent::Create(constraint_node, string(string(parent->GetName()) + "_con_" + string(child->GetName())).c_str());
			fbx_constraint->SetConstrainedObject(child);
			fbx_constraint->AddConstraintSource(constraint_node);
			fbx_constraint->SetRotationOffset(constraint_node, fbx_rotation);
			fbx_constraint->SetTranslationOffset(constraint_node, TOFBXVECTOR3(matA.GetTrans()));

			fbx_constraint->AffectRotationX = false;
			fbx_constraint->AffectRotationY = false;
			fbx_constraint->AffectRotationZ = false;

			return constraint_node;
		}
		FbxNode* visit(PrismaticDescriptor& descriptor, FbxNode* parent, FbxNode* child) 
		{
			return parent;
		}
		FbxNode* visit(MalleableDescriptor& descriptor, FbxNode* parent, FbxNode* child)
		{
			switch (descriptor.type) {
				case BALLANDSOCKET:
					return visit(descriptor.ballAndSocket, parent, child);
				case HINGE:
					return visit(descriptor.hinge, parent, child);
				case LIMITED_HINGE:
					return visit(descriptor.limitedHinge, parent, child);
				case PRISMATIC:
					return visit(descriptor.prismatic, parent, child);
				case RAGDOLL:
					return visit(descriptor.ragdoll, parent, child);
				case STIFFSPRING:
					return visit(descriptor.stiffSpring, parent, child);
				case MALLEABLE:
					throw runtime_error("Recursive Malleable Constraint detected!");
			}
			throw runtime_error("Unknown Malleable Constraint detected!");
			return parent;
		}
		FbxNode* visit(HingeDescriptor& descriptor, FbxNode* parent, FbxNode* child) 
		{
			Vector4 row1 = descriptor.axleA;
			Vector4 row2 = descriptor.perp2AxleInA1;
			Vector4 row3 = descriptor.perp2AxleInA2;
			Vector4 row4 = descriptor.pivotA * bhkScaleFactor;
			row4.w = 1;

			Matrix44 matA = Matrix44(
				row1[0], row1[1], row1[2], row4[0],
				row2[0], row2[1], row2[2], row4[1],
				row3[0], row3[1], row3[2], row4[2],
				0.0f, 0.0f, 0.0f, row4[3]
			);

			Vector4 row1b = descriptor.axleB;
			Vector4 row2b = descriptor.perp2AxleInB1;
			Vector4 row3b = descriptor.perp2AxleInB2;
			Vector4 row4b = descriptor.pivotB * bhkScaleFactor;
			row4b.w = 1;

			Matrix44 matB = Matrix44(
				row1b[0], row1b[1], row1b[2], row4b[0],
				row2b[0], row2b[1], row2b[2], row4b[1],
				row3b[0], row3b[1], row3b[2], row4b[2],
				0.0f, 0.0f, 0.0f, row4b[3]
			);

			FbxNode* constraint_node = FbxNode::Create(parent->GetScene(), string(string(parent->GetName()) + "_con_" + string(child->GetName()) + "_attach_point").c_str());
			parent->AddChild(setMatTransform(matB, constraint_node));


			Quaternion rotation = matA.GetRotation().AsQuaternion();
			Quat QuatTest = { rotation.x, rotation.y, rotation.z, rotation.w };
			EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
			FbxVector4 fbx_rotation = FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z));

			FbxConstraintParent * fbx_constraint = FbxConstraintParent::Create(constraint_node, string(string(parent->GetName()) + "_con_" + string(child->GetName())).c_str());
			fbx_constraint->SetConstrainedObject(child);
			fbx_constraint->AddConstraintSource(constraint_node);
			//fbx_constraint->SetRotationOffset(constraint_node, fbx_rotation);
			//fbx_constraint->SetTranslationOffset(constraint_node, TOFBXVECTOR3(matA.GetTrans()));

			fbx_constraint->AffectRotationX = false;

			return NULL;
		}
		FbxNode* visit(LimitedHingeDescriptor& descriptor, FbxNode* parent, FbxNode* child) 
		{
			Vector4 row1 = descriptor.axleA;
			Vector4 row2 = descriptor.perp2AxleInA1;
			Vector4 row3 = descriptor.perp2AxleInA2;
			Vector4 row4 = descriptor.pivotA * bhkScaleFactor;
			row4.w = 1;

			Matrix44 matA = Matrix44(
				row1[0], row1[1], row1[2], row4[0],
				row2[0], row2[1], row2[2], row4[1],
				row3[0], row3[1], row3[2], row4[2],
				0.0f, 0.0f, 0.0f, row4[3]
			);

			Vector4 row1b = descriptor.axleB;
			Vector4 row2b = descriptor.perp2AxleInB1;
			Vector4 row3b = descriptor.perp2AxleInB2;
			Vector4 row4b = descriptor.pivotB * bhkScaleFactor;
			row4b.w = 1;

			Matrix44 matB = Matrix44(
				row1b[0], row1b[1], row1b[2], row4b[0],
				row2b[0], row2b[1], row2b[2], row4b[1],
				row3b[0], row3b[1], row3b[2], row4b[2],
				0.0f, 0.0f, 0.0f, row4b[3]
			);
	
			FbxNode* constraint_node = FbxNode::Create(parent->GetScene(), string(string(parent->GetName()) + "_con_" + string(child->GetName())+ "_attach_point").c_str());
			parent->AddChild(setMatTransform(matB, constraint_node));


			Quaternion rotation = matA.GetRotation().AsQuaternion();
			Quat QuatTest = { rotation.x, rotation.y, rotation.z, rotation.w };
			EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
			FbxVector4 fbx_rotation = FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z));

			FbxConstraintParent * fbx_constraint = FbxConstraintParent::Create(constraint_node, string(string(parent->GetName()) + "_con_" + string(child->GetName())).c_str());
			fbx_constraint->SetConstrainedObject(child);
			fbx_constraint->AddConstraintSource(constraint_node);
			fbx_constraint->SetRotationOffset(constraint_node, fbx_rotation);
			fbx_constraint->SetTranslationOffset(constraint_node, TOFBXVECTOR3(matA.GetTrans()));


			int index = fabs(descriptor.axleB.x) > fabs(descriptor.axleB.y) ?
				fabs(descriptor.axleB.x) > fabs(descriptor.axleB.z) ? 0 : 2 :
				fabs(descriptor.axleB.y) > fabs(descriptor.axleB.z) ? 1 : 2;

			//if (index == 1)
			//	printf("lol");

			//if (index == 0)
				fbx_constraint->AffectRotationX = false;
			//else if (index == 1)
				fbx_constraint->AffectRotationY = false;
			//else
				//fbx_constraint->AffectRotationZ = false;

			//fbx_constraint->AffectRotationX = false;
			//fbx_constraint->AffectRotationY = false;
			//fbx_constraint->AffectRotationZ = false;

			return NULL;
		}
		FbxNode* visit(BallAndSocketDescriptor& descriptor, FbxNode* parent, FbxNode* child) 
		{
			return parent;
		}
		FbxNode* visit(StiffSpringDescriptor& descriptor, FbxNode* parent, FbxNode* child) 
		{
			return parent;
		}

		void visitConstraint(FbxNode* holder, map<bhkRigidBodyRef, FbxNode*>& bodies, bhkRigidBodyRef parent, bhkConstraintRef constraint) {
			if (constraint)
			{
				for (const auto& entity : constraint->GetEntities())
				{
					if (entity->IsDerivedType(bhkRigidBody::TYPE))
					{
						bhkRigidBodyRef rbentity = DynamicCast<bhkRigidBody>(entity);
						if (rbentity != parent)
						{
							if (bodies.find(rbentity) == bodies.end())
								throw runtime_error("Wrong Nif Hierarchy, entity referred before being built!");
							FbxNode* parent = bodies[rbentity];


							//Constraints need an animation stack?
							FbxScene* scene = holder->GetScene();
							size_t stacks = scene->GetSrcObjectCount<FbxAnimStack>();
							if (stacks == 0)
							{
								FbxAnimStack* lAnimStack = FbxAnimStack::Create(scene, "Take 001");
								FbxAnimLayer* layer = FbxAnimLayer::Create(scene, "Default");
							}

							FbxNode* constraint_position = NULL;

							if (constraint->IsSameType(bhkRagdollConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkRagdollConstraint>(constraint)->GetRagdoll(), parent, holder);
							else if (constraint->IsSameType(bhkPrismaticConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkPrismaticConstraint>(constraint)->GetPrismatic(), parent, holder);
							else if (constraint->IsSameType(bhkMalleableConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkMalleableConstraint>(constraint)->GetMalleable(), parent, holder);
							else if (constraint->IsSameType(bhkHingeConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkHingeConstraint>(constraint)->GetHinge(), parent, holder);
							else if (constraint->IsSameType(bhkLimitedHingeConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkLimitedHingeConstraint>(constraint)->GetLimitedHinge(), parent, holder);
							else if (constraint->IsSameType(bhkBallAndSocketConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkBallAndSocketConstraint>(constraint)->GetBallAndSocket(), parent, holder);
							else if (constraint->IsSameType(bhkStiffSpringConstraint::TYPE))
								constraint_position = visit(DynamicCast<bhkStiffSpringConstraint>(constraint)->GetStiffSpring(), parent, holder);
							else
								throw new runtime_error("Unimplemented constraint type!");

							//FbxConstraintParent * fbx_constraint = FbxConstraintParent::Create(constraint_position, string(string(parent->GetName()) + "_con_" + string(holder->GetName())).c_str());
							//fbx_constraint->SetConstrainedObject(holder);
							//fbx_constraint->AddConstraintSource(constraint_position);
							//fbx_constraint->AffectRotationX = false;
							//fbx_constraint->AffectRotationY = false;
							//fbx_constraint->AffectRotationZ = false;
						}
					}
				}	
			}
		}

	public:

		FbxConstraintBuilder(FbxNode* holder, map<bhkRigidBodyRef, FbxNode*>& bodies, bhkRigidBodyRef parent, bhkConstraintRef constraint, double bhkScaleFactor) : bhkScaleFactor(bhkScaleFactor)
		{
			visitConstraint(holder, bodies, parent, constraint);
		}
		
	};

	inline FbxNode* visit_rigid_body(bhkRigidBodyRef obj, const string& collision_name, bool absolute = false) {
		FbxNode* parent = build_stack.front();
		string name = collision_name;
		name += "_rb";
		FbxNode* rb_node = FbxNode::Create(&scene, name.c_str());
		bodies[obj] = rb_node;

		//This is funny: Rigid Bodies transform is an actual world matrix even if 
		//it's parented into a NiNode. The value is actually ignored without a T derived class
		//So while the creature is not in a ragdoll state, the rigid bodies are driven by the ninode;
		//When the ragdoll kicks in, the transformations are matched
		//Havok engine would originally separate the skeletal animation and the ragdoll,
		//but that will require an additional linking between the ragdoll and the parent bone
		//to avoid this, we are properly parenting the rigid body and calculate a relative matrix on import
		//the T condition is left for reference

		/*if (obj->IsSameType(bhkRigidBodyT::TYPE))
		{*/
			Vector4 translation = obj->GetTranslation();
			rb_node->LclTranslation.Set(FbxDouble3(translation.x*bhkScaleFactor, translation.y*bhkScaleFactor, translation.z*bhkScaleFactor));
			Niflib::hkQuaternion rotation = obj->GetRotation();
			Quat QuatTest = { rotation.x, rotation.y, rotation.z, rotation.w };
			EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
			rb_node->LclRotation.Set(FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z)));
		//}
		recursive_convert(obj->GetShape(), rb_node, obj->GetHavokFilter());

		//find the relative transform between nodes
		FbxAMatrix world_parent = parent->EvaluateGlobalTransform();
		FbxAMatrix world_child = rb_node->EvaluateGlobalTransform();

		FbxAMatrix rel = world_parent.Inverse() * world_child;

		if (absolute)
			parent->AddChild(setMatATransform(rel, rb_node));
		else
			parent->AddChild(rb_node);

		//Constraints
		for (const auto& constraint : obj->GetConstraints())
		{
			if (constraint->IsDerivedType(bhkConstraint::TYPE)) {
				FbxConstraintBuilder(rb_node, bodies, obj, DynamicCast<bhkConstraint>(constraint), bhkScaleFactor);
				alreadyVisitedNodes.insert(constraint);
			}
			else {
				throw runtime_error("Unknown constraint hierarchy!");
			}
		}

		return rb_node;
	}

	inline FbxNode* visit_phantom(bhkSimpleShapePhantomRef obj, const string& collision_name) {
		FbxNode* parent = build_stack.front();
		string name = collision_name;
		name += "_sp";
		FbxNode* rb_node = FbxNode::Create(&scene, name.c_str());
		rb_node->SetShadingMode(FbxNode::EShadingMode::eWireFrame);

		Matrix44 transform = obj->GetTransform();
		//Following nifskope, this is 
		//setMatTransform(transform, rb_node, bhkScaleFactor);
		recursive_convert(obj->GetShape(), rb_node, obj->GetHavokFilter());

		//hmmmm... seems to be relative and not absolute as in the case of rigid bodies
		//find the relative transform between nodes
		//FbxAMatrix world_parent = parent->EvaluateGlobalTransform();
		//FbxAMatrix world_child = rb_node->EvaluateGlobalTransform();

		//FbxAMatrix rel = world_parent.Inverse() * world_child;

		parent->AddChild(/*setMatATransform(rel, */rb_node/*)*/);

		return rb_node;
	}

	template<>
	inline FbxNode* visit_new_object(bhkCollisionObject& obj) {
		FbxNode* parent = build_stack.front();
		string name = obj.GetTarget()->GetName();
		sanitizeString(name);
		if (obj.GetBody() && obj.GetBody()->IsDerivedType(bhkRigidBody::TYPE))
		{
			alreadyVisitedNodes.insert(obj.GetBody());
			return visit_rigid_body(DynamicCast<bhkRigidBody>(obj.GetBody()), name);
		}
		return NULL;
	}

	template<>
	inline FbxNode* visit_new_object(bhkBlendCollisionObject& obj) {
		FbxNode* parent = build_stack.front();
		string name = obj.GetTarget()->GetName();
		sanitizeString(name);
		if (obj.GetBody() && obj.GetBody()->IsDerivedType(bhkRigidBody::TYPE))
		{
			alreadyVisitedNodes.insert(obj.GetBody());
			return visit_rigid_body(DynamicCast<bhkRigidBody>(obj.GetBody()), name, true);
		}
		return NULL;
	}

	template<>
	inline FbxNode* visit_new_object(bhkSPCollisionObject& obj) {
		FbxNode* parent = build_stack.front();
		string name = obj.GetTarget()->GetName();
		sanitizeString(name);
		if (obj.GetBody() && obj.GetBody()->IsDerivedType(bhkSimpleShapePhantom::TYPE))
		{
			alreadyVisitedNodes.insert(obj.GetBody());
			return visit_phantom(DynamicCast<bhkSimpleShapePhantom>(obj.GetBody()), name);
		}
		return NULL;
	}

	template<class T>
	inline void visit_compound(T& obj) {}

	template<class T>
	inline void visit_field(T& obj) {}

	template<class T>
	FbxNode* build(T& obj, FbxNode* parent) {
		NiObject* pobj = (NiObject*)&obj;
		//Geometry is actually an attribute and not really a node
		if (pobj->IsDerivedType(NiAVObject::TYPE)) {
			NiAVObjectRef av = DynamicCast<NiAVObject>(pobj);
			string name = av->GetName().c_str(); sanitizeString(name);
			FbxNode* node = FbxNode::Create(&scene, name.c_str());
			return setTransform(av, node);
		}
		return setNullTransform(
			FbxNode::Create(&scene, pobj->GetInternalType().GetTypeName().c_str())
		);
	}
};

void FBXWrangler::AddNif(NifFile& nif) {
	FBXBuilderVisitor(nif, *scene->GetRootNode(), *scene, nif.GetInfo(), texture_path);
}

bool FBXWrangler::ExportScene(const std::string& fileName) {
	FbxExporter* iExporter = FbxExporter::Create(sdkManager, "");
	if (!iExporter->Initialize(fileName.c_str(), -1, sdkManager->GetIOSettings())) {
		iExporter->Destroy();
		return false;
	}

	// Export options determine what kind of data is to be imported.
	// The default (except for the option eEXPORT_TEXTURE_AS_EMBEDDED)
	// is true, but here we set the options explicitly.
	FbxIOSettings* ios = sdkManager->GetIOSettings();
	ios->SetBoolProp(EXP_FBX_MATERIAL, true);
	ios->SetBoolProp(EXP_FBX_TEXTURE, true);
	ios->SetBoolProp(EXP_FBX_EMBEDDED, false);
	ios->SetBoolProp(EXP_FBX_SHAPE, true);
	ios->SetBoolProp(EXP_FBX_GOBO, true);
	ios->SetBoolProp(EXP_FBX_CONSTRAINT, true);
	ios->SetBoolProp(EXP_FBX_ANIMATION, true);
	ios->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

	iExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE, FbxSceneRenamer::eNone);

	sdkManager->CreateMissingBindPoses(scene);

	//FbxAxisSystem axis(FbxAxisSystem::eMax);
	//axis.ConvertScene(scene);

	bool status = iExporter->Export(scene);
	iExporter->Destroy();

	return status;
}

bool FBXWrangler::ImportScene(const std::string& fileName, const FBXImportOptions& options) {
	FbxIOSettings* ios = sdkManager->GetIOSettings();
	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	ios->SetBoolProp(IMP_FBX_LINK, false);
	ios->SetBoolProp(IMP_FBX_SHAPE, true);
	ios->SetBoolProp(IMP_FBX_GOBO, true);
	ios->SetBoolProp(IMP_FBX_ANIMATION, true);
	ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

	FbxImporter* iImporter = FbxImporter::Create(sdkManager, "");
	if (!iImporter->Initialize(fileName.c_str(), -1, ios)) {
		iImporter->Destroy();
		return false;
	}

	if (scene)
		CloseScene();

	scene = FbxScene::Create(sdkManager, fileName.c_str());

	bool status = iImporter->Import(scene);

	FbxAxisSystem maxSystem(FbxAxisSystem::EUpVector::eZAxis, (FbxAxisSystem::EFrontVector) - 2, FbxAxisSystem::ECoordSystem::eRightHanded);
	maxSystem.ConvertScene(scene);

	if (!status) {
		FbxStatus ist = iImporter->GetStatus();
		iImporter->Destroy();
		return false;
	}
	iImporter->Destroy();

	if (scene->GetSceneInfo() != NULL)
	{
		FbxProperty p = scene->GetSceneInfo()->GetFirstProperty();
		while (p.IsValid())
		{
			string p_name = p.GetNameAsCStr();
			if (p_name == "ApplicationName")
				exporter_name = p.Get<FbxString>().Buffer();
			if (p_name == "ApplicationVersion")
				exporter_version = p.Get<FbxString>().Buffer();
			p = scene->GetSceneInfo()->GetNextProperty(p);
		}
	}

	return LoadMeshes(options);
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

bool hasNoTransform(FbxNode* node) {
	FbxDouble3 trans = node->LclTranslation.Get();
	FbxDouble3 euler_angles = node->LclRotation.Get();
	FbxDouble3 scaling = node->LclScaling.Get();
	
	return abs(trans[0]) < 1e-5 &&
		abs(trans[1]) < 1e-5 &&
		abs(trans[2]) < 1e-5 &&
		abs(euler_angles[0]) < 1e-5 &&
		abs(euler_angles[1]) < 1e-5 &&
		abs(euler_angles[2]) < 1e-5 &&
		abs(scaling[0]) > 1 - 1e-5 && abs(scaling[0]) < 1 + 1e-5 &&
		abs(scaling[1]) > 1 - 1e-5 && abs(scaling[1]) < 1 + 1e-5 &&
		abs(scaling[2]) > 1 - 1e-5 && abs(scaling[2]) < 1 + 1e-5;
}



FbxAMatrix getNodeTransform(FbxNode* pNode, bool absolute = false) {
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (pNode->GetNodeAttribute())
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
	if (absolute)
		localMatrix = pNode->EvaluateGlobalTransform();

	return localMatrix * matrixGeo;
}

FbxAMatrix getLocalNodeTransformFromProperties(FbxNode* pNode, FbxTime time = FBXSDK_TIME_INFINITE) {
	//T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp * S * Sp-1 * OT * OR * OS
	
	FbxAMatrix M_OUT; M_OUT.SetIdentity();
	FbxAMatrix M_TEMP; M_TEMP.SetIdentity();
	M_OUT.SetT(pNode->EvaluateLocalTranslation(time)); //T
	M_TEMP.SetT(pNode->GetRotationOffset(FbxNode::eSourcePivot));
	M_OUT = M_OUT * M_TEMP; // T * Roff
	M_TEMP.SetIdentity(); M_TEMP.SetT(pNode->GetRotationPivot(FbxNode::eSourcePivot));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp
	M_TEMP.SetIdentity(); M_TEMP.SetR(pNode->GetPreRotation(FbxNode::eSourcePivot));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre 
	M_TEMP.SetIdentity(); M_TEMP.SetR(pNode->EvaluateLocalRotation(time));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R
	M_TEMP.SetIdentity(); M_TEMP.SetR(pNode->GetPostRotation(FbxNode::eSourcePivot));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R * Rpost
	M_TEMP.SetIdentity(); M_TEMP.SetT(pNode->GetRotationPivot(FbxNode::eSourcePivot));
	M_TEMP = M_TEMP.Inverse();
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R * Rpost * Rp-1
	M_TEMP.SetIdentity(); M_TEMP.SetT(pNode->GetScalingOffset(FbxNode::eSourcePivot));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff
	M_TEMP.SetIdentity(); M_TEMP.SetT(pNode->GetScalingPivot(FbxNode::eSourcePivot));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp
	M_TEMP.SetIdentity(); M_TEMP.SetS(pNode->EvaluateLocalScaling(time));
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp * S
	M_TEMP.SetIdentity(); M_TEMP.SetT(pNode->GetScalingPivot(FbxNode::eSourcePivot));
	M_TEMP = M_TEMP.Inverse();
	M_OUT = M_OUT * M_TEMP; // T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp * S * Sp-1

	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();

	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	matrixGeo.SetT(lT);
	matrixGeo.SetR(lR);
	matrixGeo.SetS(lS);

	return M_OUT * matrixGeo;
}

FbxAMatrix getWorldTransform(FbxNode* pNode) {
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (pNode->GetNodeAttribute())
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);

	FbxNode* pParentNode = pNode->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO) * parentMatrix;
	}

	return parentMatrix * localMatrix * matrixGeo;
}

void setAvTransform(FbxNode* node, NiAVObject* av, bool rotation_only = false, bool absolute = false) {
	FbxAMatrix tr = node->EvaluateLocalTransform(FBXSDK_TIME_ZERO); //getNodeTransform(node, absolute);
	FbxDouble3 trans = tr.GetT();
	av->SetTranslation(Vector3(trans[0], trans[1], trans[2]));
	av->SetRotation(
		Matrix33(
			tr.Get(0,0), tr.Get(0,1), tr.Get(0,2),
			tr.Get(1,0), tr.Get(1,1), tr.Get(1,2),
			tr.Get(2,0), tr.Get(2,1), tr.Get(2,2)
		)
	);
	FbxDouble3 scaling = tr.GetS(); // node->LclScaling.Get();
	//if (scaling[0] == scaling[1] && scaling[1] == scaling[2])
		av->SetScale(scaling[0]);
}

NiTransform GetAvTransform(FbxAMatrix tr) {
	NiTransform out;
	FbxDouble3 trans = tr.GetT();
	out.translation = Vector3(trans[0], trans[1], trans[2]);
	out.rotation =
		Matrix33(
			tr.Get(0, 0), tr.Get(0, 1), tr.Get(0, 2),
			tr.Get(1, 0), tr.Get(1, 1), tr.Get(1, 2),
			tr.Get(2, 0), tr.Get(2, 1), tr.Get(2, 2)
		)
	;
	FbxDouble3 scaling = tr.GetS(); // node->LclScaling.Get();
	if (scaling[0] == scaling[1] && scaling[1] == scaling[2])
		out.scale = scaling[0];
	return out;
}


struct bone_weight {
	NiNode* bone;
	float weight;

	bone_weight(NiNode* bone, float weight) : bone(bone), weight(weight) {}
};

class AccessSkin {};

template<>
class Accessor<AccessSkin>
{
	public:
		Accessor(FbxMesh* m, const map<int, int>& cp, NiTriShapeRef shape, map<FbxNode*, NiObjectRef>& conversion_Map, NiNodeRef& conversion_root, bool export_skin) {
			if (m->GetDeformerCount(FbxDeformer::eSkin) > 0) {
				NiSkinInstanceRef skin;
				if (export_skin)
					skin = new BSDismemberSkinInstance();
				else
					skin = new NiSkinInstance();
				NiSkinDataRef data = new NiSkinData();
				NiSkinPartitionRef spartition = new NiSkinPartition();

				map<NiNode*, BoneData > bones_data;
				vector<SkinPartition > partitions;
				//set<NiNode*> bones;
				//vector<NiNode*> vbones;

				multimap<int, int> faces_map;

				int numTris = m->GetPolygonCount();
				for (int t = 0; t < numTris; t++) {
					if (m->GetPolygonSize(t) != 3)
						continue;

					int p1 = cp.find(m->GetPolygonVertex(t, 0))->second;
					int p2 = cp.find(m->GetPolygonVertex(t, 1))->second;
					int p3 = cp.find(m->GetPolygonVertex(t, 2))->second;

					faces_map.insert({ p1,t });
					faces_map.insert({ p2,t });
					faces_map.insert({ p3,t });

				}

				for (int iSkin = 0; iSkin < m->GetDeformerCount(FbxDeformer::eSkin); iSkin++) {
					FbxSkin* fbx_skin = (FbxSkin*)m->GetDeformer(iSkin, FbxDeformer::eSkin);
					SkinPartition partition;
					
					for (int iCluster = 0; iCluster < fbx_skin->GetClusterCount(); iCluster++) {
						FbxCluster* cluster = fbx_skin->GetCluster(iCluster);
						if (!cluster->GetLink())
							continue;

						FbxNode* bone = cluster->GetLink();
						NiNodeRef ni_bone = DynamicCast<NiNode>(conversion_Map[cluster->GetLink()]);

						//find global
						vector<NiNode * >& bones = skin->bones;
						vector<BoneData >& bone_data_list = data->boneList;
						vector<NiNode * >::iterator bone_it = find(bones.begin(), bones.end(), ni_bone);
						size_t bone_data_index = -1;
						if (bone_it != bones.end())
						{
							bone_data_index = distance(bones.begin(), bone_it);
						}
						else {
							bone_data_index = bones.size();
							bones.push_back(ni_bone);
							BoneData bd;

							FbxAMatrix joint_transform; cluster->GetTransformLinkMatrix(joint_transform);
							FbxAMatrix global_transform; global_transform = cluster->GetTransformMatrix(global_transform).Inverse();
							FbxAMatrix local_transform = (global_transform * joint_transform).Inverse();

							data->skinTransform = GetAvTransform(global_transform);
							bd.skinTransform = GetAvTransform(local_transform);
							bone_data_list.push_back(bd);
							//data->SetBoneList(bone_data_list);
						}
						//Bone Data
						BoneData& bone_data = bone_data_list[bone_data_index];

						partition.numWeightsPerVertex = 4;

						//find local values
						vector<unsigned short >& partition_bones = partition.bones;
						size_t local_bone_index = -1;
						vector<unsigned short >::iterator local_bones_it = find(partition_bones.begin(), partition_bones.end(), bone_data_index);
						if (local_bones_it == partition_bones.end())
						{
							local_bone_index = partition_bones.size();
							partition_bones.push_back(bone_data_index);
							partition.numBones++;
						}
						else {
							local_bone_index = distance(partition_bones.begin(), local_bones_it);
						}

						size_t cp_count = cluster->GetControlPointIndicesCount();
						vector< vector<float > >& local_vertices_weights = partition.vertexWeights;
						//local_vertices_weights.resize(cp_count);
						vector< vector<byte > >& local_vertices_indices = partition.boneIndices;
						//local_vertices_indices.resize(cp_count);
						vector<Triangle>& local_triangles = partition.triangles;
						for (int iPoint = 0; iPoint < cluster->GetControlPointIndicesCount(); iPoint++)
						{
							int v = cp.find(cluster->GetControlPointIndices()[iPoint])->second;
							float w = cluster->GetControlPointWeights()[iPoint];

							if (w > 0.0)
							{
								BoneVertData bvd;
								bvd.index = v;
								bvd.weight = w;
							
								bone_data.vertexWeights.push_back(bvd);
								bone_data.numVertices++;
							}

							size_t local_vertex_index = -1;
							vector<unsigned short >::iterator v_local_it = find(partition.vertexMap.begin(), partition.vertexMap.end(), v);

							if (v_local_it == partition.vertexMap.end()) {
								partition.numVertices++;
								
								local_vertex_index = partition.vertexMap.size();
								partition.vertexMap.push_back(v);
								local_vertices_weights.resize(partition.vertexMap.size());
								local_vertices_indices.resize(partition.vertexMap.size());
							}
							else {
								local_vertex_index = distance(partition.vertexMap.begin(), v_local_it);
							}

							vector<float >& local_vertex_weight_array = local_vertices_weights[local_vertex_index];
							local_vertex_weight_array.push_back(w);
							vector<byte >& local_bones_indices = local_vertices_indices[local_vertex_index];
							local_bones_indices.push_back(local_bone_index);							
						} //end points

						//triangles
						for (int iPoint = 0; iPoint < cluster->GetControlPointIndicesCount(); iPoint++)
						{
							int v = cp.find(cluster->GetControlPointIndices()[iPoint])->second;
							pair <multimap<int, int>::iterator, multimap<int, int>::iterator> this_faces = faces_map.equal_range(v);

							for (multimap<int, int>::iterator faces_it = this_faces.first; faces_it != this_faces.second; faces_it++)
							{
								int triangle_index = faces_it->second;

								int global_v1 = cp.find(m->GetPolygonVertex(triangle_index, 0))->second;
								int global_v2 = cp.find(m->GetPolygonVertex(triangle_index, 1))->second;
								int global_v3 = cp.find(m->GetPolygonVertex(triangle_index, 2))->second;

								vector<unsigned short >::iterator local_v1_it = find(partition.vertexMap.begin(), partition.vertexMap.end(), global_v1);
								vector<unsigned short >::iterator local_v2_it = find(partition.vertexMap.begin(), partition.vertexMap.end(), global_v2);
								vector<unsigned short >::iterator local_v3_it = find(partition.vertexMap.begin(), partition.vertexMap.end(), global_v3);

								if (local_v1_it != partition.vertexMap.end() && local_v2_it != partition.vertexMap.end() && local_v3_it != partition.vertexMap.end())
								{
									int local_v1 = distance(partition.vertexMap.begin(), local_v1_it);
									int local_v2 = distance(partition.vertexMap.begin(), local_v2_it);
									int local_v3 = distance(partition.vertexMap.begin(), local_v3_it);

									Triangle ni_t = { (unsigned short)local_v1,
										(unsigned short)local_v2,
										(unsigned short)local_v3 };

									vector<Triangle>::iterator tris_pos = find(local_triangles.begin(), local_triangles.end(), ni_t);
									if (tris_pos == local_triangles.end())
									{
										local_triangles.push_back(ni_t);
										partition.numTriangles++;
									}
								}
							}
						}

						for (int i_f = 0; i_f < partition.numVertices; i_f++) {

							vector<byte >& pindex = local_vertices_indices[i_f];
							if (pindex.size() > 4)
								Log::Error("Too many indices for bone");

							vector<float >& vweight = local_vertices_weights[i_f];
							if (vweight.size() > 4)
								Log::Error("Too many weights for bone");

							//find less influencing bone
							if (pindex.size() != vweight.size()) {
								Log::Error("Fatal Error: weights != indexes");
								throw std::runtime_error("Fatal Error: weights != indexes");
							}
							bool removed = false;
							while (vweight.size() > 4) {
								removed = true;
								int to_remove_bone = distance(vweight.begin(), min_element(begin(vweight), end(vweight)));
								pindex.erase(pindex.begin() + to_remove_bone);
								vweight.erase(vweight.begin() + to_remove_bone);
							}
							if (removed) {
								float sum = std::accumulate(vweight.begin(), vweight.end(), 0.0f);
								vweight[0] /= sum;
								vweight[1] /= sum;
								vweight[2] /= sum;
								vweight[3] /= sum;
							}

							pindex.resize(4);
							vweight.resize(4);
						}
					}

					if (partition.bones.size()<4)
						partition.bones.resize(4);

					partition.numWeightsPerVertex = 4;
					partition.numTriangles = partition.triangles.size();
					partition.hasFaces = partition.triangles.size() > 0;
					partition.trianglesCopy = partition.triangles;

					partition.hasVertexMap = partition.vertexMap.size() > 0;
					partition.hasVertexWeights = partition.vertexWeights.size() > 0;
					partition.hasBoneIndices = partition.boneIndices.size() > 0;

					partitions.push_back(partition);
				}

				spartition->SetSkinPartitionBlocks(partitions);
				data->SetHasVertexWeights(1);
				skin->SetData(data);
				skin->SetSkinPartition(spartition);
				skin->SetSkeletonRoot(conversion_root);

				if (export_skin)
				{
					BSDismemberSkinInstanceRef bsskin = DynamicCast<BSDismemberSkinInstance>(skin);
					bsskin->partitions.resize(bsskin->skinPartition->skinPartitionBlocks.size());
					for (int i = 0; i < bsskin->partitions.size(); i++)
					{
						bsskin->partitions[i].bodyPart = SBP_32_BODY;
						bsskin->partitions[i].partFlag = (BSPartFlag)( PF_EDITOR_VISIBLE | PF_START_NET_BONESET);
					}
				}

				shape->SetSkinInstance(skin);
			}

		}
};

void FBXWrangler::convertSkins(FbxMesh* m, NiTriShapeRef shape, const map<int, int>& cp) {
	Accessor<AccessSkin> do_it(m, cp, shape, conversion_Map, conversion_root, export_skin);
	int bones = 60;
	int weights = 4;
	if (shape->GetSkinInstance()->GetBones().size() > 60)
		remake_partitions(StaticCast<NiTriBasedGeom>(shape), bones, weights, false, false);
}

string format_texture(string tempString)
{
	size_t idx = tempString.find("textures", 0);
	if (idx != string::npos)
	{
		tempString.erase(tempString.begin(), tempString.begin() + idx);
	}
	else {
		idx = tempString.find("cube", 0);
		if (idx != string::npos)
		{
			tempString.erase(tempString.begin(), tempString.begin() + idx);
		}
	}
	fs::path p(tempString);
	p.replace_extension(".dds");
	return p.string();
}

// Get the value of a geometry element for a triangle vertex
template <typename TGeometryElement, typename TValue>
inline TValue get_vertex_element(TGeometryElement * pElement, int iPoint, int iTriangle, int iVertex, TValue defaultValue)
{
	if (!pElement || pElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eNone) return defaultValue;
	int index = 0;
	if (pElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint) index = iPoint;
	else if (pElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygon) index = iTriangle;
	else if (pElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex) index = iTriangle * 3 + iVertex;
	if (pElement->GetReferenceMode() != fbxsdk::FbxGeometryElement::eDirect) index = pElement->GetIndexArray().GetAt(index);
	return pElement->GetDirectArray().GetAt(index);
}

FbxVector4 v;
FbxVector4 v_n;
FbxVector4 v_t;
FbxVector4 v_bt;
FbxVector2 v_uv;
FbxColor color;

bool has_normal = false;
bool has_uv = false;
bool has_vc = false;

Vector3 nif_n;
Vector3 nif_t;
Vector3 nif_bt;
TexCoord nif_uv;
Color4 nif_color;

inline Vector3 toNIF(const FbxVector4& v) {
	return { (float)v.mData[0], (float)v.mData[1], (float)v.mData[2] };
}

inline TexCoord toNIF(const FbxVector2& v) {
	return { (float)v.mData[0], (float)v.mData[1]};
}

inline Color4 toNIF(const FbxColor& color) {
	return 	{ (float)color.mRed, (float)color.mGreen, (float)color.mBlue, (float)color.mAlpha };
}



NiTriShapeRef FBXWrangler::importShape(FbxNodeAttribute* node, const FBXImportOptions& options) {
	NiTriShapeRef out = new NiTriShape();
	NiTriShapeDataRef data = new NiTriShapeData();

	bool hasAlpha = false;

	FbxMesh* m = (FbxMesh*)node;
	FbxGeometryElementUV* uv = m->GetElementUV(0);
	FbxGeometryElementNormal* normal = m->GetElementNormal(0);
	FbxGeometryElementVertexColor* vc = m->GetElementVertexColor(0);

	string orig_name = m->GetName();
	out->SetName(unsanitizeString(orig_name));
	int numVerts = m->GetControlPointsCount();
	int numTris = m->GetPolygonCount();

	vector<Vector3> verts(0);
	vector<Vector3> normals(0);
	vector<Vector3> tangents(0);
	vector<Vector3> bitangents(0);
	vector<Color4 > vcs(0);

	if (normal == NULL) {
		Log::Info("Warning: cannot find normals, I'll recalculate them for %s", m->GetName());
	}
	vector<TexCoord> uvs;

	FbxAMatrix node_trans; node_trans = m->GetPivot(node_trans);
	if (uv)
	{
		auto& uv_array = uv->GetDirectArray();
		if (options.InvertU)
			for (int i = 0; i < uv_array.GetCount(); i++)
			{
				auto uvv = uv_array.GetAt(i);
				uvv[0] = 1.0f - uvv[0];
				uv_array.SetAt(i, uvv);
			}

		if (options.InvertV)
			for (int i = 0; i < uv_array.GetCount(); i++)
			{
				auto uvv = uv_array.GetAt(i);
				uvv[1] = 1.0f - uvv[1];
				uv_array.SetAt(i, uvv);
			}		
	}

	m->GenerateTangentsDataForAllUVSets();

	FbxGeometryElementTangent* tangent = m->GetElementTangent(0);
	FbxGeometryElementBinormal* bitangent = m->GetElementBinormal(0);

	bool max_wa = false;


	//string exporter_name = "";
	//string exporter_version = "";

	//Max has a wrong value into the vertex color mappings in 2016/2017/2018 
	if (exporter_name == "3ds Max" &&
		(exporter_version == "2017" || exporter_version == "2018"))
		max_wa = true;

	//for (int v = 0; v < numVerts; v++) {
	//	FbxVector4 vert = m->GetControlPointAt(v);

	//	if (uv && uv->GetMappingMode() != FbxGeometryElement::eByControlPoint &&
	//		uv->GetMappingMode() != FbxGeometryElement::eByPolygonVertex) {
	//		FbxGeometryElement::EMappingMode m = uv->GetMappingMode();
	//	}

	//	verts.emplace_back((float)vert.mData[0], (float)vert.mData[1], (float)vert.mData[2]);
	//	if (uv && uv->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
	//		int uIndex = v;
	//		if (uv->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
	//			uIndex = uv->GetIndexArray().GetAt(v);

	//		uvs.emplace_back((float)uv->GetDirectArray().GetAt(uIndex).mData[0],
	//			(float)uv->GetDirectArray().GetAt(uIndex).mData[1]);
	//	}

	//	if (normal && normal->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
	//		normals.emplace_back((float)normal->GetDirectArray().GetAt(v).mData[0],
	//			(float)normal->GetDirectArray().GetAt(v).mData[1],
	//			(float)normal->GetDirectArray().GetAt(v).mData[2]);
	//		if (tangent)
	//		{
	//			tangents.emplace_back((float)tangent->GetDirectArray().GetAt(v).mData[0],
	//				(float)tangent->GetDirectArray().GetAt(v).mData[1],
	//				(float)tangent->GetDirectArray().GetAt(v).mData[2]);
	//		}
	//		if (bitangent)
	//		{
	//			bitangents.emplace_back((float)bitangent->GetDirectArray().GetAt(v).mData[0],
	//				(float)bitangent->GetDirectArray().GetAt(v).mData[1],
	//				(float)bitangent->GetDirectArray().GetAt(v).mData[2]);
	//		}
	//	}


	//	if (vc && (vc->GetMappingMode() == FbxGeometryElement::eByControlPoint || max_wa)) {
	//		vcs.emplace_back(
	//			Color4
	//			(
	//			(float)vc->GetDirectArray().GetAt(v).mRed,
	//				(float)vc->GetDirectArray().GetAt(v).mGreen,
	//				(float)vc->GetDirectArray().GetAt(v).mBlue,
	//				(float)vc->GetDirectArray().GetAt(v).mAlpha
	//			)
	//		);
	//		if (hasAlpha == false && (float)vc->GetDirectArray().GetAt(v).mAlpha < 1.0)
	//			hasAlpha = true;
	//	}
	//}


	//verts.resize(numVerts);
	const char* uvName = nullptr;
	if (uv) {
		uvName = uv->GetName();
		//uvs.resize(numVerts);
	}

	//if (vc && vcs.empty()) {
	//	vcs.resize(numVerts);
	//}

	//if (normal && normals.empty())
	//{
	//	normals.resize(numVerts);
	//	if (uv)
	//	{
	//		tangents.resize(numVerts);
	//		bitangents.resize(numVerts);
	//	}
	//}

	vector<Triangle> tris;
	set<int> mapped;

	map<std::array<double,18>, size_t> uniques;
	map<int, int> cp;

	for (int t = 0; t < numTris; t++) {
		if (m->GetPolygonSize(t) != 3)
			continue;

		std::array<int, 3> triangle;

		for (int i = 0; i < 3; i++)
		{
			FbxVector4 v;
			FbxVector4 v_n;
			FbxVector4 v_t;
			FbxVector4 v_bt;
			FbxVector2 v_uv;
			FbxColor color;

			bool has_normal = false;
			bool has_uv = false;
			bool has_vc = false;

			Vector3 nif_v;
			Vector3 nif_n;
			Vector3 nif_t;
			Vector3 nif_bt;
			TexCoord nif_uv;
			Color4 nif_color;

			bool isUnmapped;
			int vertex_index = m->GetPolygonVertex(t, i);

			v = m->GetControlPointAt(vertex_index);
			if (normal)
			{
				v_n = get_vertex_element(normal, vertex_index, t, i, fbxsdk::FbxVector4(0, 0, 0, 0));
				if (tangent)
					v_t = get_vertex_element(tangent, vertex_index, t, i, fbxsdk::FbxVector4(0, 0, 0, 0));
				if (bitangent)
					v_bt = get_vertex_element(bitangent, vertex_index, t, i, fbxsdk::FbxVector4(0, 0, 0, 0));
				has_normal = true;
			}

			if (uv) {
				v_uv = get_vertex_element(uv, vertex_index, t, i, fbxsdk::FbxVector2(0, 0));
				nif_uv = TexCoord(v_uv.mData[0], v_uv.mData[1]);
				has_uv = true;
			}

			if (vc) {
				color = get_vertex_element(vc, vertex_index, t, i, fbxsdk::FbxColor(0, 0, 0, 0));
				//MAX workaround
				if (max_wa)
					color = vc->GetDirectArray().GetAt(vertex_index);
				if (hasAlpha == false && color.mAlpha < 1.0)
					hasAlpha = true;
				has_vc = true;
			}

			nif_v = toNIF(v);
			int nif_vertex_index = verts.size();
			//verts[vertex_index] = toNIF(v);
			triangle[i] = nif_vertex_index;

			nif_n = toNIF(v_n);
			nif_t = toNIF(v_t);
			nif_bt = toNIF(v_bt);
			nif_uv = toNIF(v_uv);
			nif_color = toNIF(color);

			//FbxVector4 v;
			//FbxVector4 v_n;
			//FbxVector4 v_t;
			//FbxVector4 v_bt;
			//FbxVector2 v_uv;
			//FbxColor color;

			std::array<double, 18> values = {
				v.mData[0], v.mData[1], v.mData[2],
				v_n.mData[0], v_n.mData[1], v_n.mData[2],
				v_t.mData[0], v_t.mData[1], v_t.mData[2],
				v_bt.mData[0], v_bt.mData[1], v_bt.mData[2],
				v_uv.mData[0], v_uv.mData[1],
				color.mRed, color.mGreen, color.mBlue, color.mAlpha
			};
			auto v_iter = uniques.find(values);
			if (v_iter == uniques.end()) {
				int index = verts.size();
				verts.push_back(nif_v);
				if (normal) {
					normals.push_back(nif_n);
					if (tangent && bitangent)
					{
						tangents.push_back(nif_t);
						bitangents.push_back(nif_bt);
					}
				}
				if (has_uv)
					uvs.push_back(nif_uv);
				if (has_vc)
					vcs.push_back(nif_color);
				uniques[values] = index;
				triangle[i] = index;
			}
			else {
				triangle[i] = v_iter->second;
			}

			cp[vertex_index] = triangle[i];

			/*if (vc && (vc->GetMappingMode() == FbxGeometryElement::eByControlPoint || max_wa)) {
				color = vc->GetDirectArray().GetAt(vertex_index);
				if (hasAlpha == false && (float)vc->GetDirectArray().GetAt(vertex_index).mAlpha < 1.0)
					hasAlpha = true;
			}
			if (vc && vc->GetMappingMode() == FbxGeometryElement::eByPolygonVertex && !max_wa)
			{
				switch (vc->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
				{
					color = vc->GetDirectArray().GetAt(t * 3 + i);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = vc->GetIndexArray().GetAt(t * 3 + i);
					color = vc->GetDirectArray().GetAt(id);
					break;
				}
				default:
				{
					break;
				}
				}
				if (hasAlpha == false && color.mAlpha < 1.0)
					hasAlpha = true;
				nif_color = Color4
				(
					(float)color.mRed,
					(float)color.mGreen,
					(float)color.mBlue,
					(float)color.mAlpha
				);
				has_vc = true;
			}
			else if (max_wa && vc)
			{
				has_vc = true;
				nif_color = vcs[vertex_index];
			}*/


			/*if (normal && normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
				switch (normal->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						v_n = normal->GetDirectArray().GetAt(t * 3 + i);
						if (tangent)
							v_t = tangent->GetDirectArray().GetAt(t * 3 + i);
						if (bitangent)
							v_bt = bitangent->GetDirectArray().GetAt(t * 3 + i);
						break;
					}
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = normal->GetIndexArray().GetAt(t * 3 + i);
						v_n = normal->GetDirectArray().GetAt(id);
						if (tangent)
							v_t = tangent->GetDirectArray().GetAt(id);
						if (bitangent)
							v_bt = bitangent->GetDirectArray().GetAt(id);
						break;
					}
					default:
					{
						break;
					}
				}
				nif_n = Vector3(v_n.mData[0], v_n.mData[1], v_n.mData[2]);
				if (tangent && bitangent)
				{
					nif_t = Vector3(v_t.mData[0], v_t.mData[1], v_t.mData[2]);
					nif_bt = Vector3(v_bt.mData[0], v_bt.mData[1], v_bt.mData[2]);
				}
				has_normal = true;
			}*/
			//if (uv && uv->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			//	m->GetPolygonVertexUV(t, i, uvName, v_uv, isUnmapped);
			//	nif_uv = TexCoord(v_uv.mData[0], v_uv.mData[1]);
			//	has_uv = true;
			//}
			



			//CE can only handle per vertex mapping. We duplicate vertices that have multiple mappings
			/*if (!mapped.insert(vertex_index).second)
			{
				bool remap_normal = false;
				bool remap_uv = false;
				bool remap_vc = false;
				if (has_normal && !(nif_n == normals[vertex_index]))
				{
					remap_normal = true;
				}
				if (has_uv && !(nif_uv == uvs[vertex_index]))
				{
					int lTextureUVIndex = m->GetTextureUVIndex(t, i);
					remap_uv = true;
				}
				if (has_vc && !(nif_color == vcs[vertex_index]))
				{
					remap_vc = true;
				}
				if (remap_normal || remap_uv || remap_vc) {
					triangle[i] = verts.size();
					verts.push_back(verts[vertex_index]);
					if (has_normal) {
						normals.push_back(nif_n);
						if (has_uv)
						{
							tangents.push_back(nif_t);
							bitangents.push_back(nif_bt);
						}
					}
					if (has_uv)
						uvs.push_back(nif_uv);
					if (has_vc)
						vcs.push_back(nif_color);
					
				}
			}
			else {
				if (has_normal) {
					normals[vertex_index] = nif_n;
					if (has_uv)
					{
						tangents[vertex_index] = nif_t;
						bitangents[vertex_index] = nif_bt;
					}
				}
				if (has_uv)
					uvs[vertex_index] = nif_uv;
				if (has_vc)
					vcs[vertex_index] = nif_color;
			}*/
		}

		tris.emplace_back(triangle[0], triangle[1], triangle[2]);
	}
	if (verts.size() > 0) {
		data->SetHasVertices(true);
		data->SetVertices(verts);
	}

	if (tris.size()) {
		data->SetHasTriangles(true);
		data->SetNumTriangles(tris.size());
		data->SetNumTrianglePoints(tris.size() * 3);
		data->SetTriangles(tris);

		if (normals.empty() && verts.size())
		{
			Vector3 COM;
			CalculateNormals(verts, tris, normals, COM);
		}
	}

	if (normals.size() > 0) {
		data->SetHasNormals(true);
		data->SetNormals(normals);
	}

	if (vcs.size())
	{
		data->SetHasVertexColors(true);
		data->SetVertexColors(vcs);
	}

	if (m->GetDeformerCount(FbxDeformer::eSkin) > 0) {

		skins_Map[m] = out;
		skins_control_Points.insert({ m, cp });
	}
	else {
		meshes.insert(m);
	}



	if (uvs.size() > 0) {
		data->SetHasUv(true);
		data->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV));
		data->SetUvSets({ uvs });
	}

	data->SetConsistencyFlags(CT_STATIC);

	if (verts.size() != 0 && tris.size() != 0 && uvs.size() != 0) {
		//Vector3 COM;;
		//TriGeometryContext g(verts, COM, tris, uvs, normals);
		data->SetHasNormals(1);
		//recalculate
		data->SetNormals(normals);
		//switched to uniform with nifskope
		data->SetTangents(bitangents);
		data->SetBitangents(tangents);
		data->SetBsVectorFlags(static_cast<BSVectorFlags>(data->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
	}

	BSLightingShaderProperty* shader = new BSLightingShaderProperty();

	if (data->GetHasVertexColors() == false)
		shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2() & ~SLSF2_VERTEX_COLORS));

	if (m->GetDeformerCount(FbxDeformer::eSkin) > 0) {
		shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() | SLSF1_SKINNED));
	}


	BSShaderTextureSetRef textures = new BSShaderTextureSet();
	textures->SetTextures({
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
		});



	if (node->GetNodeCount() > 0)
	{
		if (node->GetNodeCount() > 1)
			Log::Info("node->NodeCount() is above 1. Is this normal?");

		FbxNode* tempN = node->GetNode();
		vector<string> vTextures = textures->GetTextures();
		FbxProperty prop = nullptr;

		prop = tempN->GetFirstProperty();
		while (prop.IsValid())
		{
			string name = prop.GetNameAsCStr();
			string slot_prefix = "slot";
			if (prop.GetFlag(FbxPropertyFlags::eUserDefined))
			{
				auto it = search(
					name.begin(), name.end(),
					slot_prefix.begin(), slot_prefix.end(),
					[](char ch1, char ch2) { return toupper(ch1) == toupper(ch2); }
				);
				if (it != name.end())
				{
					name.erase(name.begin(), name.begin() + 4);
					int slot = atoi(name.c_str());
					if (slot > 2 && slot < 9)
					{
						vTextures[slot-1] = format_texture(prop.Get<FbxString>().Buffer());
					}
				}
			}
			prop = tempN->GetNextProperty(prop);
		}
		FbxLayerElementMaterial* layerElement = m->GetElementMaterial();
		if (layerElement != NULL)
		{
			int sizeb = m->GetElementMaterialCount();
			int index = 0;
			FbxLayerElement::EMappingMode mode = layerElement->GetMappingMode();
		
			FbxLayerElementArrayTemplate<int>& iarray = layerElement->GetIndexArray();
			index = iarray[0];

			FbxSurfaceMaterial * material = tempN->GetMaterial(index);
			if (material != NULL)
			{
				FbxPropertyT<FbxDouble3> colour;
				FbxPropertyT<FbxDouble> factor;
				FbxFileTexture *texture;

				out->SetAlphaProperty(AlphaFlagsHandler(material).to_property());

				//diffuse:
				prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse, true);
				if (prop.IsValid())
				{
					texture = prop.GetSrcObject<FbxFileTexture>(0);
					if (texture)
					{
						vTextures[0] = format_texture(texture->GetFileName());
						if (texture->Alpha > 0.0)
							hasAlpha = true;
					}

				}

				//normal:
				prop = material->FindProperty(FbxSurfaceMaterial::sBump, true);

				if (prop.IsValid())
				{
					texture = prop.GetSrcObject<FbxFileTexture>(0);

					if (texture)
					{
						vTextures[1] = format_texture(texture->GetFileName());
					}
					else {
						prop = material->FindProperty(FbxSurfaceMaterial::sNormalMap, true);
						if (prop.IsValid())
						{
							texture = prop.GetSrcObject<FbxFileTexture>(0);
							if (texture)
							{
								vTextures[1] = format_texture(texture->GetFileName());
							}
						}
					}
				}

				//Specular
				prop = material->FindProperty("Roughness", true);
				if (prop.IsValid())
				{
					texture = prop.GetSrcObject<FbxFileTexture>(0);
					if (texture)
					{
						vTextures[1] = format_texture(texture->GetFileName());
					}
				}
				//if this isn't found, then we could go down the alternate route and do 1f-transparency?
				factor = material->FindProperty("Opacity", true);

				if (factor.IsValid())
				{
					shader->SetAlpha(factor.Get());
				}

				//spec:
				colour = material->FindProperty(material->sSpecular, true);
				factor = material->FindProperty(material->sSpecularFactor, true);
				if (colour.IsValid() && factor.IsValid())
				{
					//correct set this flag or my ocd will throw fits.
					factor.Get() > 0.0 ? shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() | SLSF1_SPECULAR)) : shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
					FbxDouble3 colourvec = colour.Get();
					shader->SetSpecularStrength(factor.Get());
					shader->SetSpecularColor(Color3(colourvec[0], colourvec[1], colourvec[2]));
				}

				//emissive
				colour = material->FindProperty(material->sEmissive, true);
				factor = material->FindProperty(material->sEmissiveFactor, true);
				if (colour.IsValid() && factor.IsValid())
				{
					FbxDouble3 colourvec = colour.Get();
					shader->SetEmissiveMultiple(factor.Get());
					shader->SetEmissiveColor(Color3(colourvec[0], colourvec[1], colourvec[2]));
				}

				//	//shiny/gloss
				factor = material->FindProperty(material->sShininess, true);
				if (factor.IsValid())
				{
					shader->SetGlossiness(factor.Get());
				}

				textures->SetTextures(vTextures);

				//unpack forced shader flags if any
				int pack_flags_1 = get_property<FbxInt>(material, "shader_flags_1");
				int pack_flags_2 = get_property<FbxInt>(material, "shader_flags_2");
				unsigned int f1; f1 |= pack_flags_1;
				unsigned int f2; f2 |= pack_flags_2;
				if (material->FindProperty("shader_flags_1").IsValid())
				{
					shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(pack_flags_1)); // = get_property<FbxBool>(material, "color_blending_enable");
					shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(pack_flags_2));
				}
				FbxProperty shader_type = material->FindProperty("shader_type");
				if (shader_type.IsValid())
				{
					shader->SetSkyrimShaderType(NifFile::shader_type_value(shader_type.Get<FbxString>().Buffer()));
				}
				FbxProperty environment_map_scale = material->FindProperty("environment_map_scale");
				if (environment_map_scale.IsValid())
				{
					shader->SetEnvironmentMapScale((float)environment_map_scale.Get<FbxDouble>());
				}
			}
		}
	}



	//if (out->GetAlphaProperty() == NULL && hasAlpha) {
	//	NiAlphaPropertyRef alpharef = new NiAlphaProperty();
	//	alpharef->SetFlags(237);
	//	alpharef->SetThreshold(128);
	//	out->SetAlphaProperty(alpharef);
	//}

	shader->SetTextureSet(textures);


	out->SetFlags(524302);
	out->SetShaderProperty(shader);
	out->SetData(StaticCast<NiGeometryData>(data));


	return out;
}

void FBXWrangler::importShapes(NiNodeRef parent, FbxNode* child, const FBXImportOptions& options) {
	//NiNodeRef dummy = new NiNode();
	//string name = child->GetName();
	//dummy->SetName(unsanitizeString(name));
	vector<NiAVObjectRef> children= parent->GetChildren();
	size_t attributes_size = child->GetNodeAttributeCount();
	for (int i = 0; i < attributes_size; i++) {
		if (FbxNodeAttribute::eMesh == child->GetNodeAttributeByIndex(i)->GetAttributeType())
		{		
			children.push_back(StaticCast<NiAVObject>(importShape(child->GetNodeAttributeByIndex(i), options)));
		}
	}
	parent->SetChildren(children);
}

KeyType collect_times(FbxAnimCurve* curveX, set<double>& times, KeyType fixed_type)
{
	KeyType type = CONST_KEY;
	if (curveX != NULL)
	{
		for (int i = 0; i < curveX->KeyGetCount(); i++)
		{
			FbxAnimCurveKey& key = curveX->KeyGet(i);
			times.insert(key.GetTime().GetSecondDouble());
			KeyType new_type = CONST_KEY;
			switch (key.GetInterpolation())
			{
			case FbxAnimCurveDef::EInterpolationType::eInterpolationConstant:
				break;
			case FbxAnimCurveDef::EInterpolationType::eInterpolationLinear:
				new_type = LINEAR_KEY;
				break;
			case FbxAnimCurveDef::EInterpolationType::eInterpolationCubic:
				new_type = QUADRATIC_KEY;
				break;
			}
			if (i > 0 && type != new_type)
			{
				Log::Warn("Found an FbxAnimCurve with mixed types of interpolation, NIF doesn't support that for translations!");
			}
			type = new_type;
		}
	}
	return type;
}

void AdjustBezier(float fLastValue, float fLastTime,
	float& fLastOut, float fNextValue, float fNextTime, float& fNextIn,
	float fNewTime, float fNewValue, float& fNewIn, float& fNewOut)
{
	// Find the coefficients of a cubic polynomial that fits the given 
	// values and slopes.

	float fOldDeltaX = fNextValue - fLastValue;
	float fOldDeltaT = fNextTime - fLastTime;
	float fNewDeltaTA = fNewTime - fLastTime;
	float fNewDeltaTB = fNextTime - fNewTime;

	// calculate normalized time
	float t = fNewDeltaTA / fOldDeltaT;

	float a = -2.0f * fOldDeltaX + fLastOut + fNextIn;
	float b = 3.0f * fOldDeltaX - 2.0f * fLastOut - fNextIn;

	// calculate tangent
	fNewIn = (((3.0f * a * t + 2.0f * b) * t + fLastOut) / fOldDeltaT);

	// normalize in and out
	fNewOut = fNewIn * fNewDeltaTB;
	fNewIn *= fNewDeltaTA;

	// renormalize last and next tangents

	fLastOut *= fNewDeltaTA / fOldDeltaT;
	fNextIn *= fNewDeltaTB / fOldDeltaT;
}

void addTranslationKeys(NiTransformInterpolator* interpolator, FbxNode* node, FbxAnimCurve* curveX, FbxAnimCurve* curveY, FbxAnimCurve* curveZ, double time_offset) {

	KeyType interp = CONST_KEY;
	set<double> times;
	interp = collect_times(curveX, times, interp);
	interp = collect_times(curveY, times, interp);
	interp = collect_times(curveZ, times, interp);

	vector<FbxAnimCurve*> curves = {
		curveX, curveY, curveZ
	};

	bool inserted_initial_position = false;
	if (times.size() > 0)
	{
		inserted_initial_position = times.insert(time_offset).second;

		NiTransformDataRef data = interpolator->GetData();
		if (data == NULL) data = new NiTransformData();
		KeyGroup<Vector3 > tkeys = data->GetTranslations();
		vector<Key<Vector3 > > keyvalues = tkeys.keys;
		
		int index = 0;
		double intpart;
		for (const auto& time : times) {
			FbxTime lTime;
			lTime.SetSecondDouble((float)time);
			FbxVector4 pos = node->EvaluateLocalTransform(FBXSDK_TIME_ZERO).GetT();
			FbxVector4 trans;
			for (int i = 0; i < 3; i++)
			{
				double find = curves[i]->KeyFind(lTime);
				double fractpart = modf(find, &intpart);
				if (find == -1.0)
					trans[i] = pos[i];
				else if (intpart == 0.0)
					trans[i] = curves[i]->KeyGetValue((int)fractpart);
				else if (interp == CONST_KEY)
					trans[i] = pos[i];
				else
					trans[i] = curves[i]->Evaluate(lTime);;
			}


			Key<Vector3 > temp;
			temp.data = Vector3(trans[0], trans[1], trans[2]);
			temp.forward_tangent = { 0, 0 ,0 };
			temp.backward_tangent = { 0, 0, 0 };
			temp.time = time - time_offset;
			keyvalues.push_back(temp);

			index++;
		}
		if (interp == QUADRATIC_KEY)
		{
			for (int i=1; i < keyvalues.size()-1; i++)
			{
				Key<Vector3 >& prev = keyvalues[i - 1];
				Key<Vector3 >& current = keyvalues[i];
				Key<Vector3 >& next = keyvalues[i + 1];

				for (int j=0; j<3; j++)
					AdjustBezier(prev.data[j], prev.time, prev.backward_tangent[j],
						next.data[j], next.time, next.forward_tangent[j],
						current.time, current.data[j], current.forward_tangent[j], current.backward_tangent[j]);
			}
		}

		tkeys.numKeys = keyvalues.size();
		tkeys.keys = keyvalues;
		tkeys.interpolation = interp;
		data->SetTranslations(tkeys);
		interpolator->SetData(data);
	}
}

template<>
class Accessor<NiTransformData> {
public:
	Accessor(NiTransformDataRef ref) {
		ref->numRotationKeys = 1;
	}

	Accessor(NiTransformDataRef ref, int size) {
		ref->numRotationKeys = size;
	}
};

int pack_float_key(FbxAnimCurve* curveI, KeyGroup<float>& keys, float time_offset, bool deg_to_rad)
{
	int IkeySize = 0;
	KeyType type = CONST_KEY;
	bool has_key_in_time_offset = false;
	bool key_in_time_offset_added = false;
	if (curveI != NULL)
	{
		IkeySize = curveI->KeyGetCount();
		if (IkeySize > 0) {
			for (int i = 0; i < IkeySize; i++) {
				FbxAnimCurveKey fbx_key = curveI->KeyGet(i);
				KeyType new_type = CONST_KEY;
				switch (fbx_key.GetInterpolation())
				{
				case FbxAnimCurveDef::EInterpolationType::eInterpolationConstant:
					break;
				case FbxAnimCurveDef::EInterpolationType::eInterpolationLinear:
					new_type = LINEAR_KEY;
				case FbxAnimCurveDef::EInterpolationType::eInterpolationCubic:
					new_type = QUADRATIC_KEY;
				}
				if (i > 0 && type != new_type)
				{
					Log::Warn("Found an FbxAnimCurve with mixed types of interpolation, NIF doesn't support that for translations!");
				}
				type = new_type;
				Key<float> new_key;
				if (fbx_key.GetTime().GetSecondDouble() == time_offset)
					has_key_in_time_offset = true;
				new_key.time = fbx_key.GetTime().GetSecondDouble() - time_offset;
				new_key.data = fbx_key.GetValue();
				new_key.forward_tangent = 0;
				new_key.backward_tangent = 0;
				if (deg_to_rad)
				{
					new_key.data = deg2rad(new_key.data);
				}
				keys.keys.push_back(new_key);
			}
			if (!has_key_in_time_offset)
			{
				key_in_time_offset_added = true;
				IkeySize += 1;
				Key<float> new_key;
				new_key.time = 0.0;
				FbxTime ttime; ttime.SetSecondDouble(time_offset);
				new_key.data = curveI->Evaluate(time_offset);
				new_key.forward_tangent = 0;
				new_key.backward_tangent = 0;
				if (deg_to_rad)
				{
					new_key.data = deg2rad(new_key.data);
				}
				keys.keys.insert(keys.keys.begin(), new_key);
			}

			keys.numKeys = keys.keys.size();
			keys.interpolation = type;
		}
		if (type == QUADRATIC_KEY)
		{
			vector<Key<float > >& keyvalues = keys.keys;
			//int i = key_in_time_offset_added == false ? 1 : 2;
			for (int i=1; i < keyvalues.size() - 1; i++)
			{
				Key<float >& prev = keyvalues[i - 1];
				Key<float >& current = keyvalues[i];
				Key<float >& next = keyvalues[i + 1];

				AdjustBezier(prev.data, prev.time, prev.backward_tangent,
					next.data, next.time, next.forward_tangent,
					current.time, current.data, current.forward_tangent, current.backward_tangent);
			}
		}
	}
	return IkeySize;
}


void addRotationKeys(NiTransformInterpolator* interpolator, FbxNode* node, FbxAnimCurve* curveI, FbxAnimCurve* curveJ, FbxAnimCurve* curveK, double time_offset) {
	//this is simpler because curves can be evaluated one at a time
	NiTransformDataRef data = interpolator->GetData();
	if (data == NULL) data = new NiTransformData();
	Niflib::array<3, KeyGroup<float > > tkeys = data->GetXyzRotations();

	int IkeySize = pack_float_key(curveI, tkeys[0], time_offset, true);
	int JkeySize = pack_float_key(curveJ, tkeys[1], time_offset, true);
	int KkeySize = pack_float_key(curveK, tkeys[2], time_offset, true);

	if (IkeySize > 0 || JkeySize > 0 || KkeySize > 0) {
		Accessor<NiTransformData> fix_rot(data);
		data->SetXyzRotations(tkeys);
		data->SetRotationType(XYZ_ROTATION_KEY);
		interpolator->SetData(data);
	}
}

void addScaleKeys(NiTransformInterpolator* interpolator, FbxNode* node, FbxAnimCurve* curveI, FbxAnimCurve* curveJ, FbxAnimCurve* curveK, double time_offset) {
	//this is simpler because curves can be evaluated one at a time
	NiTransformDataRef data = interpolator->GetData();
	if (data == NULL) data = new NiTransformData();
	KeyGroup<float >  tkeys = data->GetScales();

	int IkeySize = pack_float_key(curveI, tkeys, time_offset, false);
	if (IkeySize > 0) {
		data->SetScales(tkeys);
		interpolator->SetData(data);
	}
}

void fix_interpolation(vector<Key<float >>& keyvalues)
{
	for (int i = 1; i < keyvalues.size() - 1; i++)
	{
		Key<float >& prev = keyvalues[i - 1];
		Key<float >& current = keyvalues[i];
		Key<float >& next = keyvalues[i + 1];

		AdjustBezier(prev.data, prev.time, prev.backward_tangent,
			next.data, next.time, next.forward_tangent,
			current.time, current.data, current.forward_tangent, current.backward_tangent);
	}
}

void fix_interpolation(vector<Key<Vector3 >>& keyvalues)
{
	for (int i = 1; i < keyvalues.size() - 1; i++)
	{
		Key<Vector3 >& prev = keyvalues[i - 1];
		Key<Vector3 >& current = keyvalues[i];
		Key<Vector3 >& next = keyvalues[i + 1];

		for (int j = 0; j<3; j++)
			AdjustBezier(prev.data[j], prev.time, prev.backward_tangent[j],
				next.data[j], next.time, next.forward_tangent[j],
				current.time, current.data[j], current.forward_tangent[j], current.backward_tangent[j]);
	}
}

inline hkTransform to_havok_matrix(const FbxAMatrix& m)
{
	hkTransform out;
	out(0, 0) = m.Get(0, 0); out(0, 1) = m.Get(0, 1); out(0, 2) = m.Get(0, 2); out(0, 3) = m.Get(3, 0);
	out(1, 0) = m.Get(1, 0); out(1, 1) = m.Get(1, 1); out(1, 2) = m.Get(1, 2); out(1, 3) = m.Get(3, 1);
	out(2, 0) = m.Get(2, 0); out(2, 1) = m.Get(2, 1); out(2, 2) = m.Get(2, 2); out(2, 3) = m.Get(3, 2);
	out(3, 0) = m.Get(0, 3); out(3, 1) = m.Get(1, 3); out(3, 2) = m.Get(2, 3); out(3, 3) = m.Get(3, 3);
	return out;
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

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void handle_singularities(vector<Key<float > >& keys, set<float>& times)
{
	bool crossed = false;
	int sign = 0;
	for (int i = 1; i < keys.size() - 1; i++)
	{
		Key<float >& prev_key = keys[i - 1];
		Key<float >& current_key = keys[i];
		Key<float >& next_key = keys[i + 1];
		
		
		if (crossed)
			current_key.data = sign * (2 * M_PI - current_key.data);
		if (current_key.time - prev_key.time <= 0.3 && abs(current_key.data - prev_key.data) > M_PI_2)
		{
			sign = sgn(prev_key.data);
			crossed = !crossed;
			current_key.data = sign * (2 * M_PI - current_key.data);
		}
	}
}

double FBXWrangler::convert(FbxAnimLayer* pAnimLayer, NiControllerSequenceRef sequence, set<NiObjectRef>& targets, NiControllerManagerRef manager, NiMultiTargetTransformControllerRef multiController, string accum_root_name, double last_start, double last_stop)
{
	// Display general curves.
	vector<ControlledBlock > blocks = sequence->GetControlledBlocks();
	for (FbxNode* pNode : unskinned_bones)
	{
		//FbxNode* pNode = pair.first;
		FbxAnimCurve* lXAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve* lYAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve* lZAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
		FbxAnimCurve* lIAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		FbxAnimCurve* lJAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		FbxAnimCurve* lKAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		FbxAnimCurve* lsXAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		FbxAnimCurve* lsYAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		FbxAnimCurve* lsZAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

		if (lXAnimCurve->KeyGetCount()>0 || lYAnimCurve->KeyGetCount()>0 || lZAnimCurve->KeyGetCount()>0 ||
			lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL ||
			lsXAnimCurve != NULL || lsYAnimCurve != NULL || lsZAnimCurve != NULL)
		{

			NiObjectRef target = conversion_Map[pNode];
			targets.insert(target);

			NiTransformInterpolatorRef interpolator = new NiTransformInterpolator();
			NiQuatTransform trans;
			unsigned int float_min = 0xFF7FFFFF;
			float* lol = (float*)&float_min;
			trans.translation = Vector3(*lol, *lol, *lol);
			trans.rotation = Quaternion(*lol, *lol, *lol, *lol);
			trans.scale = *lol;
			interpolator->SetTransform(trans);

			if (lXAnimCurve->KeyGetCount()>0 || lYAnimCurve->KeyGetCount()>0 || lZAnimCurve->KeyGetCount()>0) {
				addTranslationKeys(interpolator, pNode, lXAnimCurve, lYAnimCurve, lZAnimCurve, last_start);
			}

			if (lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL) {
				addRotationKeys(interpolator, pNode, lIAnimCurve, lJAnimCurve, lKAnimCurve, last_start);
			}

			if (lsXAnimCurve != NULL) {
				addScaleKeys(interpolator, pNode, lsXAnimCurve, lsYAnimCurve, lsZAnimCurve, last_start);
			}

			ControlledBlock block;
			block.interpolator = interpolator;
			block.nodeName = DynamicCast<NiAVObject>(conversion_Map[pNode])->GetName();
			block.controllerType = "NiTransformController";
			block.controller = multiController;

			blocks.push_back(block);

			sequence->SetControlledBlocks(blocks);

			sequence->SetStartTime(0.0);
			sequence->SetStopTime(last_stop - last_start);
			sequence->SetManager(manager);
			sequence->SetAccumRootName(accum_root_name);

			NiTextKeyExtraDataRef extra_data = new NiTextKeyExtraData();
			extra_data->SetName(string(""));
			Key<IndexString> start_key;
			start_key.time = 0;
			start_key.data = "start";

			Key<IndexString> end_key;
			end_key.time = last_stop - last_start;
			end_key.data = "end";

			extra_data->SetTextKeys({ start_key,end_key });
			extra_data->SetNextExtraData(NULL);

			sequence->SetTextKeys(extra_data);

			sequence->SetFrequency(1.0);
			sequence->SetCycleType(CYCLE_CLAMP);

		}
	}
	return 0.0;
}

set<float> convert_rigid_animation(NiTransformInterpolatorRef interpolator, FbxAnimLayer* pAnimLayer, FbxNode* animated_node, float offset = 0.0f)
{
	//collect all the original keys
	vector<FbxAnimCurve*> curves = {
		animated_node->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true),
		animated_node->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true),
		animated_node->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true),
		animated_node->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X),
		animated_node->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y),
		animated_node->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z),
		animated_node->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X),
		animated_node->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y),
		animated_node->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z)
	};

	KeyType translation_interp = LINEAR_KEY;
	KeyType rotation_I_interp = LINEAR_KEY;
	KeyType rotation_J_interp = LINEAR_KEY;
	KeyType rotation_K_interp = LINEAR_KEY;
	KeyType scale_interp = LINEAR_KEY;

	set<float> key_times;

	for (int k = 0; k<curves.size(); k++)
	{
		FbxAnimCurve* curve = curves[k];
		if (curve != NULL)
		{
			int IkeySize = curve->KeyGetCount();
			for (int i = 0; i < IkeySize; i++) {
				FbxAnimCurveKey fbx_key = curve->KeyGet(i);
				key_times.insert(fbx_key.GetTime().GetSecondDouble());
				FbxAnimCurveDef::EInterpolationType type = fbx_key.GetInterpolation();
				KeyType nif_type;
				switch (type)
				{
				case FbxAnimCurveDef::EInterpolationType::eInterpolationLinear:
					nif_type = LINEAR_KEY;
					break;
				case FbxAnimCurveDef::EInterpolationType::eInterpolationConstant:
					nif_type = CONST_KEY;
					break;
				case FbxAnimCurveDef::EInterpolationType::eInterpolationCubic:
					nif_type = QUADRATIC_KEY;
					break;
				default:
					break;
				}

				if (k < 3 && nif_type > translation_interp) translation_interp = nif_type;
				if (k == 3 && nif_type > rotation_I_interp) rotation_I_interp = nif_type;
				if (k == 4 && nif_type > rotation_J_interp) rotation_J_interp = nif_type;
				if (k == 5 && nif_type > rotation_K_interp) rotation_K_interp = nif_type;
				if (k > 5 && nif_type > scale_interp) scale_interp = nif_type;

			}
		}
		else {
			if (k < 3 && curves[0] == NULL && curves[1] == NULL && curves[2] == NULL) translation_interp = CONST_KEY;
			if (k == 3) rotation_I_interp = CONST_KEY;
			if (k == 4) rotation_J_interp = CONST_KEY;
			if (k == 5) rotation_K_interp = CONST_KEY;
			if (k > 5) scale_interp = CONST_KEY;
		}
	}

	if (!key_times.empty()) key_times.insert(offset);

	NiTransformDataRef data = new NiTransformData();

	vector<Key<Vector3 > > translation_key_values;
	vector<Key<float > > rotation_key_I_values;
	vector<Key<float > > rotation_key_J_values;
	vector<Key<float > > rotation_key_K_values;
	vector<Key<Quaternion>> quat_values;
	vector<Key<float > > scale_key_values;

	FbxTime fbx_Time;
	for (const auto& time : key_times)
	{
		fbx_Time.SetSecondDouble((float)time);
		FbxAMatrix transform = animated_node->EvaluateLocalTransform(fbx_Time);//getLocalNodeTransformFromProperties(animated_node, fbx_Time);
		FbxVector4 translation = transform.GetT();
		FbxVector4 rotation = transform.GetR();
		FbxVector4 scale = transform.GetS();

		Key<Vector3> translation_key;
		Key<float > rotation_key_I;
		Key<float > rotation_key_J;
		Key<float > rotation_key_K;
		Key<Quaternion> quat_rotation_key;
		Key<float> scale_key;

		translation_key.time = time - offset;
		translation_key.data = { (float)translation[0], (float)translation[1], (float)translation[2] };
		translation_key.forward_tangent = { 0, 0, 0 };
		translation_key.backward_tangent = { 0, 0, 0 };
		translation_key_values.push_back(translation_key);

		rotation_key_I.time = time - offset;
		rotation_key_J.time = time - offset;
		rotation_key_K.time = time - offset;
		rotation_key_I.data = deg2rad(rotation[0]);
		rotation_key_J.data = deg2rad(rotation[1]);
		rotation_key_K.data = deg2rad(rotation[2]);
		rotation_key_I.forward_tangent = 0;
		rotation_key_I.backward_tangent = 0;
		rotation_key_J.forward_tangent = 0;
		rotation_key_J.backward_tangent = 0;
		rotation_key_K.forward_tangent = 0;
		rotation_key_K.backward_tangent = 0;
		rotation_key_I_values.push_back(rotation_key_I);
		rotation_key_J_values.push_back(rotation_key_J);
		rotation_key_K_values.push_back(rotation_key_K);

		scale_key.time = time - offset;
		scale_key.data = scale[0];
		scale_key.forward_tangent = 0;
		scale_key.backward_tangent = 0;
		scale_key_values.push_back(scale_key);
	}

	handle_singularities(rotation_key_I_values, key_times);
	handle_singularities(rotation_key_J_values, key_times);
	handle_singularities(rotation_key_K_values, key_times);

	//Fix interpolation
	if (translation_interp == QUADRATIC_KEY)
		fix_interpolation(translation_key_values);
	if (rotation_I_interp == QUADRATIC_KEY)
		fix_interpolation(rotation_key_I_values);
	if (rotation_J_interp == QUADRATIC_KEY)
		fix_interpolation(rotation_key_J_values);
	if (rotation_K_interp == QUADRATIC_KEY)
		fix_interpolation(rotation_key_K_values);
	if (scale_interp == QUADRATIC_KEY)
		fix_interpolation(scale_key_values);

	KeyGroup<Vector3 > translation_keys;
	translation_keys.interpolation = translation_interp;
	translation_keys.numKeys = translation_key_values.size();
	translation_keys.keys = translation_key_values;
	data->SetTranslations(translation_keys);

	Niflib::array<3, KeyGroup<float > > rotation_keys;
	rotation_keys[0].interpolation = rotation_I_interp;
	rotation_keys[0].numKeys = rotation_key_I_values.size();
	rotation_keys[0].keys = rotation_key_I_values;
	rotation_keys[1].interpolation = rotation_J_interp;
	rotation_keys[1].numKeys = rotation_key_J_values.size();
	rotation_keys[1].keys = rotation_key_J_values;
	rotation_keys[2].interpolation = rotation_K_interp;
	rotation_keys[2].numKeys = rotation_key_K_values.size();
	rotation_keys[2].keys = rotation_key_K_values;

	Accessor<NiTransformData> fix_rot(data);
	data->SetXyzRotations(rotation_keys);
	data->SetRotationType(XYZ_ROTATION_KEY);

	KeyGroup<float > scale_keys;
	scale_keys.interpolation = scale_interp;
	scale_keys.numKeys = scale_key_values.size();
	scale_keys.keys = scale_key_values;
	data->SetScales(scale_keys);

	interpolator->SetData(data);

	return key_times;
}

//double FBXWrangler::convert(FbxAnimLayer* pAnimLayer, NiControllerSequenceRef sequence, set<NiObjectRef>& targets, NiControllerManagerRef manager, NiMultiTargetTransformControllerRef multiController, string accum_root_name, double last_start, double last_stop)
//{
//	// Display general curves.
//	vector<ControlledBlock > blocks = sequence->GetControlledBlocks();
//	for (FbxNode* pNode : unskinned_bones)
//	{
//		NiTransformInterpolatorRef interpolator = new NiTransformInterpolator();
//		NiQuatTransform trans;
//		unsigned int float_min = 0xFF7FFFFF;
//		float* lol = (float*)&float_min;
//
//		trans.translation = Vector3(*lol, *lol, *lol);
//		trans.rotation = Quaternion(*lol, *lol, *lol, *lol);
//		trans.scale = *lol;
//		interpolator->SetTransform(trans);
//
//		set<float> keys = convert_rigid(interpolator, pAnimLayer, pNode, last_start);
//		if (keys.empty())
//			continue;
//
//		NiObjectRef target = conversion_Map[pNode];
//		targets.insert(target);
//		if (target->IsDerivedType(NiNode::TYPE))
//		{
//			DynamicCast<NiNode>(target)->SetFlags(142);
//		}
//
//		ControlledBlock block;
//		block.interpolator = interpolator;
//		block.nodeName = DynamicCast<NiAVObject>(conversion_Map[pNode])->GetName();
//		block.controllerType = "NiTransformController";
//		block.controller = multiController;
//
//		blocks.push_back(block);
//
//		sequence->SetControlledBlocks(blocks);
//
//		sequence->SetStartTime(0.0);
//		sequence->SetStopTime(last_stop-last_start);
//		sequence->SetManager(manager);
//		sequence->SetAccumRootName(accum_root_name);
//
//		NiTextKeyExtraDataRef extra_data = new NiTextKeyExtraData();
//		extra_data->SetName(string(""));
//		Key<IndexString> start_key;
//		start_key.time = 0.0;
//		start_key.data = "start";
//
//		Key<IndexString> end_key;
//		end_key.time = last_stop - last_start;
//		end_key.data = "end";
//
//		extra_data->SetTextKeys({ start_key,end_key });
//		extra_data->SetNextExtraData(NULL);
//
//		sequence->SetTextKeys(extra_data);
//
//		sequence->SetFrequency(1.0);
//		sequence->SetCycleType(CYCLE_CLAMP);
//	}
//	return 0.0;
//}

//build embedded KF animations, for anything unskinned
void FBXWrangler::buildKF() {
	//create a controller manager
	NiControllerManagerRef controller = new NiControllerManager();
	vector<Ref<NiControllerSequence > > sequences = controller->GetControllerSequences();
	NiMultiTargetTransformControllerRef transformController = new NiMultiTargetTransformController();
	set<NiObjectRef> extra_targets;
	for (int i = 0; i < unskinned_animations.size(); i++)
	{
		FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(i);
		scene->SetCurrentAnimationStack(lAnimStack);
		//could contain more than a layer, but by convention we wean just the first
		FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(0);
		FbxTimeSpan reference = lAnimStack->GetReferenceTimeSpan();
		FbxTimeSpan local = lAnimStack->GetLocalTimeSpan();
		NiControllerSequenceRef sequence = new NiControllerSequence();
		//Translate
		convert(lAnimLayer, sequence, extra_targets, controller, transformController, string(conversion_root->GetName()), local.GetStart().GetSecondDouble(), local.GetStop().GetSecondDouble());

		sequence->SetName(string(lAnimStack->GetName()));
		sequences_names.insert(string(lAnimStack->GetName()));
		sequences.push_back(sequence);
	}

	//Reset stack
	scene->SetCurrentAnimationStack(scene->GetSrcObject<FbxAnimStack>(0));

	vector<NiAVObject * > etargets = transformController->GetExtraTargets();
	NiDefaultAVObjectPaletteRef palette = new NiDefaultAVObjectPalette();
	vector<AVObject > avobjs = palette->GetObjs();
	for (const auto& target : extra_targets) {
		etargets.push_back(DynamicCast<NiAVObject>(target));
		AVObject avobj;
		avobj.avObject = DynamicCast<NiAVObject>(target);
		avobj.name = DynamicCast<NiAVObject>(target)->GetName();
		avobjs.push_back(avobj);
	}
	AVObject avobj;
	avobj.avObject = StaticCast<NiAVObject>(conversion_root);
	avobj.name = StaticCast<NiAVObject>(conversion_root)->GetName();
	avobjs.push_back(avobj);

	transformController->SetFlags(44);
	transformController->SetFrequency(1.0f);
	transformController->SetPhase(0.0f);
	transformController->SetExtraTargets(etargets);
	transformController->SetTarget(conversion_root);
	//transformController->SetStartTime(0x7f7fffff);
	//transformController->SetStopTime(0xff7fffff);


	palette->SetScene(StaticCast<NiAVObject>(conversion_root));
	palette->SetObjs(avobjs);

	controller->SetObjectPalette(palette);
	controller->SetControllerSequences(sequences);
	controller->SetNextController(StaticCast<NiTimeController>(transformController));

	controller->SetFlags(12);
	controller->SetFrequency(1.0f);
	controller->SetPhase(0.0f);
	//controller->SetStartTime(0x7f7fffff);
	//controller->SetStopTime(0xff7fffff);

	controller->SetTarget(conversion_root);
	conversion_root->SetController(StaticCast<NiTimeController>(controller));
}

void FBXWrangler::checkAnimatedNodes()
{
	size_t stacks = scene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < stacks; i++)
	{
		FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(i);
		//could contain more than a layer, but by convention we use just the first, assuming the animation has been baked
		FbxAnimLayer* pAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(0);
		//for safety, we only check analyzed nodes
		for (const auto& pair : conversion_Map)
		{
			FbxNode* pNode = pair.first;

			bool isAnimated = false;
			bool hasFloatTracks = false;
			bool hasSkinnedTracks = false;
			bool isAnnotated = false;

			vector<FbxAnimCurve*> movements_curves;

			movements_curves.push_back(pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X));
			movements_curves.push_back(pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y));
			movements_curves.push_back(pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z));
			movements_curves.push_back(pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X));
			movements_curves.push_back(pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y));
			movements_curves.push_back(pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z));
			movements_curves.push_back(pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X));
			movements_curves.push_back(pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y));
			movements_curves.push_back(pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z));

			for (FbxAnimCurve* curve : movements_curves)
			{
				if (curve != NULL)
				{
					//check if the curve itself is just 0
					size_t keys = curve->KeyGetCount();
					if (keys > 0)
					{
						////has movements tracks
						isAnimated = true;
						animated_nodes.insert(pNode);
						if (skinned_bones.find(pNode) != skinned_bones.end() ||
							//it might not have the mesh not a proper eskeleton, but if we set an external one
							//check all the possible nodes for bone tracks
							!external_skeleton_path.empty())
						{
							hasSkinnedTracks = true;
						}
					}
					break;
				}
			}

			FbxProperty p = pNode->GetFirstProperty();
			
			while (p.IsValid())
			{
				string property_name = p.GetNameAsCStr();
				FbxAnimCurveNode* curveNode = p.GetCurveNode();
				if (property_name.find("hk") != string::npos)
				{
					//has annotations
					if (p.GetPropertyDataType().Is(FbxEnumDT))
					{
						annotated.insert(p);
					}
					else if (p.IsAnimated()){
						float_properties.insert(p);
					}
				}					
				p = pNode->GetNextProperty(p);
			}

			if (isAnimated) {
				if (hasSkinnedTracks)
					skinned_animations.insert(lAnimStack);
				else
				{
					unskinned_animations.insert(lAnimStack);
					unskinned_bones.insert(pNode);
				}
			}
		}
	}
}

void FBXWrangler::setExternalSkeletonPath(const string& external_skeleton_path) {
	this->external_skeleton_path = external_skeleton_path;
}

vector<size_t> getParentChain(const vector<int>& parentMap, const size_t node) {
	vector<size_t> vresult;
	int current_node = node;
	if (current_node > 0 && current_node < parentMap.size())
	{
		//his better not be a graph
		while (current_node != -1) {
			vresult.insert(vresult.begin(),current_node);
			current_node = parentMap[current_node];
		}
	}

	return move(vresult);
}

size_t getNearestCommonAncestor(const vector<int>& parentMap, const set<size_t>& nodes) {
	size_t result = NULL;
	size_t max_size_index = 0;
	size_t max_size = 0;
	int actual_index = 0;
	vector<vector<size_t>> chains;
	for (const auto& node_index : nodes)
	{
		chains.push_back(getParentChain(parentMap, node_index));
	}

	for (const auto& chain : chains) {
		if (chain.size() > max_size) {
			max_size = chain.size();
			max_size_index = actual_index;
		}
		actual_index++;
	}
	const vector<size_t>& max_chain = chains[max_size_index];
	for (const auto& bone : max_chain) {
		for (const auto& chain : chains) {
			if (find(chain.begin(), chain.end(), bone) == chain.end()) {
				return result;
			}
		}
		result = bone;
	}
	return result;
}


set<FbxNode*> FBXWrangler::buildBonesList()
{
	set<FbxNode*> bones;
	for (int meshIndex = 0; meshIndex < scene->GetGeometryCount(); ++meshIndex)
	{
		const auto mesh = static_cast<FbxMesh*>(scene->GetGeometry(meshIndex));
		if (mesh->GetDeformerCount(FbxDeformer::eSkin) > 0) {

			for (int iSkin = 0; iSkin < mesh->GetDeformerCount(FbxDeformer::eSkin); iSkin++) {
				FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(iSkin, FbxDeformer::eSkin);
				for (int iCluster = 0; iCluster < skin->GetClusterCount(); iCluster++) {
					FbxCluster* cluster = skin->GetCluster(iCluster);
					if (!cluster->GetLink())
						continue;
					bones.insert(cluster->GetLink());
				}
			}
		}
	}
	return move(bones);
}

struct KeySetter {};

template<>
struct Accessor<KeySetter> {
	Accessor(bhkListShapeRef list, const vector<unsigned int>& keys)
	{
		list->unknownInts = keys;
	}
};

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
			bhkCMSDChunk&	chunkNif = chunkListNif[chunkIdxNif];

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

bhkCMSDMaterial consume_material_from_shape(hkpShape* shape)
{
	hkpNamedMeshMaterial* material = (hkpNamedMeshMaterial *)shape->getUserData();
	string name = material->m_name;
	bhkCMSDMaterial m; m.material = NifFile::material_value(name);
	m.filter.layer_sk = (SkyrimLayer)material->m_filterInfo;
	delete material;
	return m;
}

bhkShapeRef FBXWrangler::convert_from_hk(const hkpShape* shape, bhkCMSDMaterial& aggregate_layer)
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

FbxNode* FBXWrangler::find_animated_parent(FbxNode* rigid_body)
{
	FbxNode* result = rigid_body;
	while (result != NULL)
	{
		if (unskinned_bones.find(result) != unskinned_bones.end() ||
			skinned_bones.find(result) != skinned_bones.end())
			return result;
		result = result->GetParent();
	}
	return NULL;
}

NiCollisionObjectRef FBXWrangler::build_physics(FbxNode* rigid_body, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes)
{
	NiCollisionObjectRef return_collision = NULL;
	string name = rigid_body->GetName();
	vector<hkpNamedMeshMaterial> materials;
	if (name.find("_rb") != string::npos)
	{
		bhkCollisionObjectRef collision;
		if (export_rig)
			collision = new bhkBlendCollisionObject();
		else
			collision = new bhkCollisionObject();

		bhkRigidBodyRef body;
		if (export_rig)
			body = new bhkRigidBody();
		else
			body = new bhkRigidBodyT();
		FbxAMatrix transform;
		if (export_rig)
			transform = rigid_body->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);
		else
			transform = rigid_body->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
		FbxVector4 T = transform.GetT();
		FbxQuaternion Q = transform.GetQ();
		Niflib::hkQuaternion q;
		q.x = (float)Q[0];
		q.y = (float)Q[1];
		q.z = (float)Q[2];
		q.w = (float)Q[3];
		float bhkScaleFactorInverse = 0.01428f; // 1 skyrim unit = 0,01428m
		body->SetTranslation({ (float)T[0]*bhkScaleFactorInverse,(float)T[1] * bhkScaleFactorInverse, (float)T[2] * bhkScaleFactorInverse });
		body->SetRotation(q);
		bhkCMSDMaterial body_layer;
		size_t depth = 0;
		hkRefPtr<hkpRigidBody> hk_body = HKXWrapper::build_body(rigid_body, geometry_meshes);
		hkpRigidBodyCinfo body_cinfo; hk_body->getCinfo(body_cinfo);
		body->SetShape(convert_from_hk(body_cinfo.m_shape, body_layer));
		body->SetCenter(TOVECTOR4(body_cinfo.m_centerOfMass));
		body->SetInertiaTensor(TOINERTIAMATRIX(body_cinfo.m_inertiaTensor));
		body->SetMass(hk_body->getMass());
		if (find_animated_parent(rigid_body) != NULL)
		{
			//the fix is in
			body_layer.filter.layer_sk = SKYL_ANIMSTATIC;
		}
		if (body_layer.filter.layer_sk == SKYL_ANIMSTATIC)
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
	}
	else if (name.find("_sp") != string::npos) {
		bhkSPCollisionObjectRef phantom_collision = new bhkSPCollisionObject();
		bhkSimpleShapePhantomRef phantom = new bhkSimpleShapePhantom();
		bhkCMSDMaterial phantom_layer;
		size_t depth = 0;
		Vector3 center;
		//phantom->SetShape(convert_from_hk(HKXWrapper::build_shape(rigid_body->GetChild(0), geometry_meshes), phantom_layer, depth, center));
		phantom_collision->SetBody(StaticCast<bhkWorldObject>(phantom));
		return_collision = StaticCast<NiCollisionObject>(phantom_collision);
	}
	else
		throw runtime_error("Unknown rigid body syntax!");

	return_collision->SetTarget(DynamicCast<NiAVObject>(conversion_Map[rigid_body->GetParent()]));
	return return_collision;
}

void FBXWrangler::buildCollisions()
{
	multimap<FbxNode*, FbxMesh*> bodies_meshes_map;

	std::function<void(FbxNode*)> findShapesToBeCollisioned = [&](FbxNode* root) {
		//recurse until we find a shape, avoiding rb as their mesh will be taken into account later
		string name = root->GetName();
		if (ends_with(name, "_rb") || ends_with(name, "_sp"))
			return;
		for (int i = 0; i < root->GetChildCount(); i++) {
			FbxNode* child = root->GetChild(i);

			findShapesToBeCollisioned(root->GetChild(i));
		}
		if (root->GetNodeAttribute() != NULL &&
			root->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//I'm a mesh, find my nearest parent RB, if any
			FbxNode* parent = root;
			FbxNode* body = NULL;
			while (parent != NULL)
			{
				for (int i = 0; i < parent->GetChildCount(); i++) {
					FbxNode* child = parent->GetChild(i);
					string child_name = child->GetName();
					if (ends_with(child_name, "_rb") || ends_with(child_name, "_sp"))
					{
						body = child;
						break;
					}
				}
				if (body != NULL)
					break;
				parent = parent->GetParent();
			}

			for (int i = 0; i < root->GetNodeAttributeCount(); i++)
			{
				if (root->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)
					bodies_meshes_map.insert({ body,
						(FbxMesh*)root->GetNodeAttributeByIndex(i) });
			}
		}

	};

	findShapesToBeCollisioned(scene->GetRootNode());

	for (const auto& rb : physic_entities)
	{
		pair<multimap<FbxNode*, FbxMesh*>::iterator, std::multimap<FbxNode*, FbxMesh*>::iterator> this_body_meshes;
		this_body_meshes = bodies_meshes_map.equal_range(rb);
		FbxNode* this_body_parent = rb->GetParent();
		set<pair<FbxAMatrix, FbxMesh*>> meshes;
		for (multimap<FbxNode*, FbxMesh*>::iterator it = this_body_meshes.first; it != this_body_meshes.second; ++it)
		{
			FbxAMatrix transform;
			FbxNode* mesh_parent = it->second->GetNode();
			while (mesh_parent != NULL && mesh_parent != this_body_parent)
			{
				transform = mesh_parent->EvaluateLocalTransform(FBXSDK_TIME_ZERO) * transform;
				mesh_parent = mesh_parent->GetParent();
			}
			meshes.insert({ transform, it->second });
		}
		NiAVObjectRef ni_parent = DynamicCast<NiAVObject>(conversion_Map[rb->GetParent()]);
		ni_parent->SetCollisionObject(build_physics(rb, meshes));
	}
}

bool FBXWrangler::LoadMeshes(const FBXImportOptions& options) {
	if (!scene)
		return false;

	//Split meshes before starting

	FbxGeometryConverter lConverter(sdkManager);
	lConverter.SplitMeshesPerMaterial(scene, true);
	lConverter.Triangulate(scene, true);

	set<FbxNode*> bones;
	set<NiAVObjectRef> deferred_skins;
	if (export_skin)
		bones = buildBonesList();

	FbxNode* root = scene->GetRootNode();
	//if (export_skin)
	//	conversion_root = new NiNode();
	//else
	//	conversion_root = new BSFadeNode();
	//conversion_root->SetName(string("Scene"));
	
	//nodes
	std::function<void(FbxNode*)> loadNodeChildren = [&](FbxNode* root) {
		NiNodeRef parent = DynamicCast<NiNode>(conversion_Map[root]);
		if (parent == NULL) {
			if (export_skin)
				parent = new NiNode();
			else
				parent = new BSFadeNode();

			conversion_root = parent;
			conversion_root->SetName(string("Scene"));

			if (!hasNoTransform(root)) {
				NiNodeRef proxyNiNode = new NiNode();
				setAvTransform(root, parent);
				proxyNiNode->SetName(string("rootTransformProxy"));
				conversion_root->SetChildren({ StaticCast<NiAVObject>(proxyNiNode) });
				conversion_Map[root] = proxyNiNode;
			}
			else {
				conversion_Map[root] = conversion_root;
			}
		}

		for (int i = 0; i < root->GetChildCount(); i++) {
			FbxNode* child = root->GetChild(i);
			NiAVObjectRef nif_child = NULL;
			//if (child->GetNodeAttribute() != NULL && child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
			//	nif_child = StaticCast<NiAVObject>(importShapes(child, options));
			//	setAvTransform(child, nif_child);
			//}
			string child_name = child->GetName();
			if (child_name.find("_con_") == string::npos && 
				(ends_with(child_name,"_rb") || ends_with(child_name, "_sp")))
			{
				//collisions are leaves, defer them and return
				physic_entities.insert(child);
				continue;
			}
			if (ends_with(child_name, "_support") || child_name.find("AssimpFbx") != string::npos)
			{
				//ignore support nodes added for root meshes
				loadNodeChildren(child);
				return;
			}
			if (nif_child == NULL) {
				nif_child = new NiNode();
				nif_child->SetName(unsanitizeString(string(child->GetName())));
				setAvTransform(child, nif_child);
			}
			conversion_Map[child] = nif_child;
			if (export_skin)
				parent = conversion_root;
			if (parent != NULL) {
				vector<Ref<NiAVObject > > children = parent->GetChildren();
				if (export_skin)
				{
					if (nif_child->IsDerivedType(NiNode::TYPE))
					{
						NiNodeRef node_child = DynamicCast<NiNode>(nif_child);
						vector<Ref<NiAVObject > > to_add_children = node_child->GetChildren();
						for (const auto& ref : to_add_children) {
							if (ref->IsDerivedType(NiTriBasedGeom::TYPE)) {
								//defer
								deferred_skins.insert(ref);
								//children.push_back(ref);
							}
						}
					}
					if (bones.find(child) != bones.end())
					{
						setAvTransform(child, nif_child, false, true);
						children.push_back(nif_child);
					}
				}
				else {
					children.push_back(nif_child);
				}
				parent->SetChildren(children);
				conversion_parent_Map[StaticCast<NiAVObject>(nif_child)] = StaticCast<NiAVObject>(parent);
			}

			loadNodeChildren(child);
		}
		for (int i = 0; i < root->GetNodeAttributeCount(); i++)
		{
			if (root->GetNodeAttributeByIndex(i) != NULL && root->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh) {
				importShapes(parent, root, options);
				break;
			}
		}
	};

	//if (!hasNoTransform(root)) {
	//	NiNodeRef proxyNiNode = new NiNode();
	//	setAvTransform(root, proxyNiNode);
	//	proxyNiNode->SetName(string("rootTransformProxy"));
	//	conversion_root->SetChildren({ StaticCast<NiAVObject>(proxyNiNode) });
	//	conversion_Map[root] = proxyNiNode;
	//}
	//else {
	//	conversion_Map[root] = conversion_root;
	//}
	loadNodeChildren(root);

	//skins
	for (const auto& p : skins_Map)
	{
		convertSkins(p.first, p.second, skins_control_Points[p.first]);
	}

	//insertion deferred for skins
	if (export_skin)
	{
		vector<Ref<NiAVObject > > children = conversion_root->GetChildren();
		for (const auto& skin_def : deferred_skins)
		{
			children.push_back(skin_def);
		}
		conversion_root->SetChildren(children);
	}

	//animations
	size_t stacks = scene->GetSrcObjectCount<FbxAnimStack>();
	if (stacks > 0)
	{
		checkAnimatedNodes();
		//now we have the list of both skinned and unskinned bones and skinned and unskinned stacks of the FBX
		if (unskinned_animations.size() > 0)
			buildKF();
		if (skinned_animations.size() > 0) {

			if (external_skeleton_path.empty())
			{
				//build a skeleton, it must be complete.
				set<FbxNode*> skeleton;
				for (FbxNode* bone : skinned_bones)
				{
					FbxNode* current_node = bone;
					do {
						skeleton.insert(current_node);
						current_node = current_node->GetParent();
					} while (current_node != NULL && current_node != root);
				}
				//get back the ordered skeleton
				vector<FbxNode*> hkskeleton = hkxWrapper.create_skeleton("skeleton", skeleton);
				//create the sequences
				havok_sequences = hkxWrapper.create_animations("skeleton", hkskeleton, skinned_animations, scene->GetGlobalSettings().GetTimeMode());
			}
			else {

				string external_name;
				string external_root;
				vector<string> external_floats;
				vector<string> external_bones = hkxWrapper.read_track_list(external_skeleton_path, external_name, external_root, external_floats);
				//maya is picky about names, and stuff may be very well sanitized! especially skeeltons, which use an illegal syntax in Skyrim

				vector<FbxNode*> tracks;
				vector<uint32_t> transformTrackToBoneIndices;
				vector<FbxProperty> floats;
				vector<uint32_t> transformTrackToFloatIndices;

				//TODO: nif importer from max actually screwed up a lot of things,
				//we must also check for uppercase, lowercases and so on
				for (int i = 0; i < external_bones.size(); i++)
				{
					FbxNode* track = scene->FindNodeByName(external_bones[i].c_str());
					if (track == NULL)
					{
						//try uppercase, due to nif exporter changing that
						string uppercase;
						transform(external_bones[i].begin(), external_bones[i].end(), back_inserter(uppercase), toupper);
						track = scene->FindNodeByName(uppercase.c_str());
					}
					if (track == NULL)
					{
						string sanitized = external_bones[i];
						sanitizeString(sanitized);
						track = scene->FindNodeByName(sanitized.c_str());
					}
					if (track == NULL)
					{
						Log::Info("Track not present: %s", external_bones[i].c_str());
						continue;
					}
					//OPTIMIZATION: if track curve exists but doesn't have key, do not add
					if (animated_nodes.find(track) == animated_nodes.end())
						continue;
					transformTrackToBoneIndices.push_back(i);
					tracks.push_back(track);

				}
				if (external_bones.size() == tracks.size())
					//found all the bones, needs no mapping
					transformTrackToBoneIndices.clear();

				for (int i = 0; i < external_floats.size(); i++)
				{
					string track_name = external_floats[i];
					int index = track_name.find(":");

					if (index != string::npos)
					{
						string parent = track_name.substr(index + 1, track_name.size());
						track_name = track_name.substr(0, index);
						for (const auto& this_prop : float_properties)
						{
							if (track_name == this_prop.GetNameAsCStr())
							{
								transformTrackToFloatIndices.push_back(i);
								floats.push_back(this_prop);
								break;
							}
						}
					}
				}
				if (external_floats.size() == floats.size())
					//found all the tracks, needs no mapping
					transformTrackToFloatIndices.clear();

				havok_sequences = hkxWrapper.create_animations
				(
					external_name, 
					tracks, 
					skinned_animations, 
					scene->GetGlobalSettings().GetTimeMode(),
					transformTrackToBoneIndices,
					annotated,
					floats,
					transformTrackToFloatIndices
				);
			}
		}
	}

	//collisions
	buildCollisions();

	return true;
}

bool FBXWrangler::SaveAnimation(const string& fileName) {
	hkxWrapper.write_animations(fileName, havok_sequences);
	return true;
}

bool FBXWrangler::SaveSkin(const string& fileName) {
	export_skin = true;
	SaveNif(fileName);
	export_skin = false;
	return true;
}

bool FBXWrangler::SaveNif(const string& fileName) {

	NifInfo info;
	info.userVersion = 12;
	info.userVersion2 = 83;
	info.version = Niflib::VER_20_2_0_7;

	vector<NiObjectRef> objects = RebuildVisitor(conversion_root, info).blocks;
	bsx_flags_t calculated_flags = calculateSkyrimBSXFlags(objects, info);

	if (!export_skin)
	{
		//adjust for havok
		if (!skinned_animations.empty())
			calculated_flags[0] = true;

		BSXFlagsRef bref = new BSXFlags();
		bref->SetName(string("BSX"));
		bref->SetIntegerData(calculated_flags.to_ulong());

		conversion_root->SetExtraDataList({ StaticCast<NiExtraData>(bref) });
	}
	HKXWrapperCollection wrappers;

	if (!unskinned_animations.empty() || !skinned_animations.empty()) {
		fs::path in_file = fs::path(fileName).filename();
		string out_name = in_file.filename().replace_extension("").string();
		fs::path out_path = fs::path("animations") / in_file.parent_path() / out_name;
		fs::path out_path_abs = fs::path(fileName).parent_path() / out_path;
		string out_path_a = out_path_abs.string();
		string out_havok_path = skinned_animations.empty()?
			wrappers.wrap(out_name, out_path.parent_path().string(), out_path_a, "FBX", sequences_names):
			hkxWrapper.write_project(out_name, out_path.parent_path().string(), out_path_a, "FBX", sequences_names, havok_sequences);
		vector<Ref<NiExtraData > > list = conversion_root->GetExtraDataList();
		BSBehaviorGraphExtraDataRef havokp = new BSBehaviorGraphExtraData();
		havokp->SetName(string("BGED"));
		havokp->SetBehaviourGraphFile(out_havok_path);
		havokp->SetControlsBaseSkeleton(false);
		list.insert(list.begin(), StaticCast<NiExtraData>(havokp));
		conversion_root->SetExtraDataList(list);
	}

	NifFile out(info, objects);
	return out.Save(fileName);
}

vector<FbxNode*> FBXWrangler::importExternalSkeleton(const string& external_skeleton_path, vector<FbxProperty>& float_tracks)
{	
	this->external_skeleton_path = external_skeleton_path;
	return hkxWrapper.load_skeleton(external_skeleton_path, scene->GetRootNode(), float_tracks);
}

void FBXWrangler::importAnimationOnSkeleton(const string& external_skeleton_path, vector<FbxNode*>& skeleton, vector<FbxProperty>& float_tracks)
{
	hkxWrapper.load_animation(external_skeleton_path, skeleton, float_tracks);
}
