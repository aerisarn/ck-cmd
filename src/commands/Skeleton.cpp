//#include "stdafx.h"
//
//#include <core/hkxcmd.h>
//#include <core/hkxutils.h>
//#include <core/hkfutils.h>
//#include <core/log.h>
//
//#include <cstdio>
//#include <sys/stat.h>
//
//#include <Common/Base/hkBase.h>
//#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
//#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
//#include <Common/Base/System/Io/IStream/hkIStream.h>
//#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>
//
//// Scene
//#include <Common/SceneData/Scene/hkxScene.h>
//#include <Common/Serialize/Util/hkRootLevelContainer.h>
//#include <Common/Serialize/Util/hkLoader.h>
//
//// Physics
//#include <Physics/Dynamics/Entity/hkpRigidBody.h>
//#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
//#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
//
//#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
//#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
//
//#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
//#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>
//
//#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
//#include <Physics\Dynamics\World\hkpPhysicsSystem.h>
//#include <Physics\Utilities\Serialize\hkpPhysicsData.h>
//
//// Animation
//#include <Animation/Animation/Rig/hkaSkeleton.h>
//#include <Animation/Animation/hkaAnimationContainer.h>
//#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
//#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
//#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
//#include <Animation/Animation/Rig/hkaPose.h>
//#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
//#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
//#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
//
//// Serialize
//#include <Common/Serialize/Util/hkSerializeUtil.h>
//
//// Niflib
//#include <niflib.h>
//#include <obj/NiObject.h>
//#include <obj/NiNode.h>
//#include <obj/bhkBlendCollisionObject.h>
//#include <obj/NiControllerSequence.h>
//#include <obj/NiStringPalette.h>
//#include <obj/NiTriStrips.h>
//#include <obj/NiStringExtraData.h>
//
//#include <obj/bhkShape.h>
//#include <obj/bhkSphereShape.h>
//#include <obj/bhkCapsuleShape.h>
//#include <obj/bhkRigidBody.h>
//#include <obj/bhkConstraint.h>
//#include <obj/bhkBallAndSocketConstraint.h>
//#include <obj/bhkBallSocketConstraintChain.h>
//#include <obj/bhkBreakableConstraint.h>
//#include <obj/bhkMalleableConstraint.h>
//#include <obj/bhkPhantom.h>
//#include <obj/bhkRagdollConstraint.h>
//#include <obj/bhkRagdollSystem.h>
//#include <obj/bhkPrismaticConstraint.h>
//#include <obj/bhkHingeConstraint.h>
//#include <obj/bhkLimitedHingeConstraint.h>
//
//using namespace std;
//
//static void HelpString(hkxcmd::HelpType type){
//	switch (type)
//	{
//	case hkxcmd::htShort: Log::Info("Skeleton - Dump the transform or float list for a given skeleton"); break;
//	case hkxcmd::htLong:  
//		{
//			char fullName[MAX_PATH], exeName[MAX_PATH];
//			GetModuleFileName(NULL, fullName, MAX_PATH);
//			_splitpath(fullName, NULL, NULL, exeName, NULL);
//			Log::Info("Usage: %s DumpList [-opts[modifiers]] [infile] [outfile]", exeName);
//			Log::Info("  Dump the transform or float list for a given skeleton.");
//			Log::Info("    This is useful when exporting animation to get bone list synchronized with source.");
//			Log::Info("");
//			Log::Info("<Switches>");
//			Log::Info(" -i <path>          Input File or directory");
//			Log::Info(" -o <path>          Output File - Defaults to input file with '-out' appended");
//			Log::Info("");
//		}
//		break;
//	}
//}
//
//namespace {
//	static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v){
//		return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
//	}
//
//	static inline Niflib::Vector4 TOVECTOR4(const hkVector4& v){
//		return Niflib::Vector4(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
//	}
//
//	static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v){
//		return hkVector4(v.x, v.y, v.z, v.w);
//	}
//
//	static inline Niflib::Quaternion TOQUAT(const ::hkQuaternion& q, bool inverse = false){
//		Niflib::Quaternion qt(q.m_vec.getSimdAt(3), q.m_vec.getSimdAt(0), q.m_vec.getSimdAt(1), q.m_vec.getSimdAt(2));
//		return inverse ? qt.Inverse() : qt;
//	}
//
//	static inline ::hkQuaternion TOQUAT(const Niflib::Quaternion& q, bool inverse = false){
//		hkVector4 v(q.x, q.y, q.z, q.w);
//		v.normalize4();
//		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
//		if (inverse) qt.setInverse(qt);
//		return qt;
//	}
//
//	static inline ::hkQuaternion TOQUAT(const Niflib::hkQuaternion& q, bool inverse = false){
//		hkVector4 v(q.x, q.y, q.z, q.w);
//		v.normalize4();
//		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
//		if (inverse) qt.setInverse(qt);
//		return qt;
//	}
//
//	static inline hkMatrix3 TOMATRIX3(const Niflib::InertiaMatrix& q, bool inverse = false){
//		hkMatrix3 m3;
//		m3.setCols(TOVECTOR4(q.rows[0]), TOVECTOR4(q.rows[1]), TOVECTOR4(q.rows[2]));
//		if (inverse) m3.invert(0.001);
//		return m3;
//	}
//
//	static inline hkpMotion::MotionType TOMOTIONTYPE(const Niflib::hkMotionType& ms) {
//		switch (ms) {
//		case Niflib::hkMotionType::MO_SYS_INVALID:
//			return hkpMotion::MotionType::MOTION_INVALID;
//		case Niflib::hkMotionType::MO_SYS_DYNAMIC:
//			return hkpMotion::MotionType::MOTION_DYNAMIC;
//		case Niflib::hkMotionType::MO_SYS_SPHERE_INERTIA:
//			return hkpMotion::MotionType::MOTION_SPHERE_INERTIA;
//		case Niflib::hkMotionType::MO_SYS_SPHERE_STABILIZED:
//			return hkpMotion::MotionType::MOTION_SPHERE_INERTIA;
//		case Niflib::hkMotionType::MO_SYS_BOX_INERTIA:
//			return hkpMotion::MotionType::MOTION_BOX_INERTIA;
//		case Niflib::hkMotionType::MO_SYS_BOX_STABILIZED:
//			return hkpMotion::MotionType::MOTION_BOX_INERTIA;
//		case Niflib::hkMotionType::MO_SYS_KEYFRAMED:
//			return hkpMotion::MotionType::MOTION_KEYFRAMED;
//		case Niflib::hkMotionType::MO_SYS_FIXED:
//			return hkpMotion::MotionType::MOTION_FIXED;
//		case Niflib::hkMotionType::MO_SYS_THIN_BOX:
//			return hkpMotion::MotionType::MOTION_THIN_BOX_INERTIA;
//		case Niflib::hkMotionType::MO_SYS_CHARACTER:
//			return hkpMotion::MotionType::MOTION_CHARACTER;
//		default:
//			return hkpMotion::MotionType::MOTION_INVALID;
//		}
//	}
//
//	static inline hkpCollidableQualityType TOMOTIONQUALITY(const Niflib::hkQualityType& ms) {
//		switch (ms) {
//		case Niflib::hkQualityType::MO_QUAL_INVALID:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_INVALID;
//		case Niflib::hkQualityType::MO_QUAL_FIXED:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_FIXED;
//		case Niflib::hkQualityType::MO_QUAL_KEYFRAMED:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_KEYFRAMED;
//		case Niflib::hkQualityType::MO_QUAL_DEBRIS:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_DEBRIS;
//		case Niflib::hkQualityType::MO_QUAL_MOVING:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_MOVING;
//		case Niflib::hkQualityType::MO_QUAL_CRITICAL:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_CRITICAL;
//		case Niflib::hkQualityType::MO_QUAL_BULLET:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_BULLET;
//		case Niflib::hkQualityType::MO_QUAL_USER:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_USER;
//		case Niflib::hkQualityType::MO_QUAL_CHARACTER:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_CHARACTER;
//		case Niflib::hkQualityType::MO_QUAL_KEYFRAMED_REPORT:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING;
//		default:
//			return hkpCollidableQualityType::HK_COLLIDABLE_QUALITY_INVALID;
//		}
//	}
//
//	static inline Niflib::Quaternion TOQUAT(const hkRotation& rot, bool inverse = false){
//		return TOQUAT(::hkQuaternion(rot), inverse);
//	}
//	static inline float Average(const Niflib::Vector3& val) {
//		return (val.x + val.y + val.z) / 3.0f;
//	}
//
//	const float MY_FLT_EPSILON = 1e-5f;
//	static inline bool EQUALS(float a, float b){
//		return fabs(a - b) < MY_FLT_EPSILON;
//	}
//	static inline int COMPARE(float a, float b){
//		float d = a - b;
//		return (fabs(d) < MY_FLT_EPSILON ? 0 : (d > 0 ? 1 : -1));
//	}
//
//	static inline bool EQUALS(const Niflib::Vector3& a, const Niflib::Vector3& b){
//		return (EQUALS(a.x, b.x) && EQUALS(a.y, b.y) && EQUALS(a.z, b.z));
//	}
//
//	static inline bool EQUALS(const Niflib::Quaternion& a, const Niflib::Quaternion& b){
//		return EQUALS(a.w, b.w) && EQUALS(a.x, b.x) && EQUALS(a.y, b.y) && EQUALS(a.z, b.z);
//	}
//
//	const float FramesPerSecond = 30.0f;
//	//const float FramesIncrement = 0.0325f;
//	const float FramesIncrement = 0.033333f;
//}
//
//
//using namespace Niflib;
//
//class ShapeVisitor {
//public:
//	virtual void visit(bhkSphereShape& shape, hkpRigidBodyCinfo& parent) = 0;
//	virtual void visit(bhkCapsuleShape& shape, hkpRigidBodyCinfo& parent) = 0;
//
//	virtual void visitShape(Ref<bhkShape> shape, hkpRigidBodyCinfo& parent) {
//		if (shape->IsSameType(bhkSphereShape::TYPE))
//			visit(*((bhkSphereShapeRef)shape), parent);
//		else if (shape->IsSameType(bhkCapsuleShape::TYPE))
//			visit(*((bhkCapsuleShapeRef)shape), parent);
//		else
//			throw new runtime_error("Unimplemented shape type!");
//	}
//
//
//};
//
//class ShapeBuilder : public ShapeVisitor {
//public:
//	virtual void visit(bhkSphereShape& shape, hkpRigidBodyCinfo& parent) {
//		parent.m_shape = new hkpSphereShape(shape.GetRadius()*7);
//	}
//
//	virtual void visit(bhkCapsuleShape& shape, hkpRigidBodyCinfo& parent) {
//		parent.m_shape = new hkpCapsuleShape
//		(
//		TOVECTOR4(shape.GetFirstPoint() * 7),
//		TOVECTOR4(shape.GetSecondPoint() * 7),
//			shape.GetRadius()*7
//		);
//	}
//};
//
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
//
//	virtual hkpConstraintInstance* visitConstraint(Ref<bhkConstraint> constraint) {
//		hkpConstraintData* data = NULL;
//		if (constraint->IsSameType(bhkRagdollConstraint::TYPE))
//			data = visit(((bhkRagdollConstraintRef)constraint)->GetRagdoll());
//		else if (constraint->IsSameType(bhkPrismaticConstraint::TYPE))
//			data = visit(((bhkPrismaticConstraintRef)constraint)->GetPrismatic());
//		else if (constraint->IsSameType(bhkMalleableConstraint::TYPE))
//			data = visit(((bhkMalleableConstraintRef)constraint)->GetMalleable());
//		else if (constraint->IsSameType(bhkHingeConstraint::TYPE))
//			data = visit(((bhkHingeConstraintRef)constraint)->GetHinge());
//		else if (constraint->IsSameType(bhkLimitedHingeConstraint::TYPE))
//			data = visit(((bhkLimitedHingeConstraintRef)constraint)->GetLimitedHinge());
//		else
//			throw new runtime_error("Unimplemented constraint type!");
//		return new hkpConstraintInstance(getEntity(constraint->GetEntities()[0]), getEntity(constraint->GetEntities()[1]), data);
//	}
//
//	ConstraintVisitor(vector<bhkBlendCollisionObjectRef>& nbodies, hkArray<hkpRigidBody*>& hkbodies) : nifBodies(nbodies), hkBodies(hkbodies) {}
//};
//
//class ConstraintBuilder : public ConstraintVisitor {
//public:
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
//		hkpConstraintInstance* inst = NULL;
//		switch (descriptor.type) {
//			case BALLANDSOCKET:
//				//TODO
//				break;
//			case HINGE:
//				//constraint.GetMalleable().hinge;
//			case LIMITED_HINGE:
//
//			case PRISMATIC:
//
//			case RAGDOLL:
//
//			case STIFFSPRING:
//
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
//
//static bool ExecuteCmd(hkxcmdLine &cmdLine)
//{
//	string inpath;
//	string outpath;
//	int argc = cmdLine.argc;
//	char **argv = cmdLine.argv;
//	hkSerializeUtil::SaveOptionBits flags = (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_TEXT_FORMAT|hkSerializeUtil::SAVE_TEXT_NUMBERS);
//
//#pragma region Handle Input Args
//	for (int i = 0; i < argc; i++)
//	{
//		char *arg = argv[i];
//		if (arg == NULL)
//			continue;
//		if (arg[0] == '-' || arg[0] == '/')
//		{
//
//			switch (tolower(arg[1]))
//			{
//			case 'f':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//						break;
//					flags = (hkSerializeUtil::SaveOptionBits)StringToFlags(param, SaveFlags, hkSerializeUtil::SAVE_DEFAULT);
//				} break;
//
//			case 'd':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//					{
//						Log::SetLogLevel(LOG_DEBUG);
//						break;
//					}
//					else
//					{
//						Log::SetLogLevel((LogLevel)StringToEnum(param, LogFlags, LOG_INFO));
//					}
//				} break;
//
//			case 'o':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//						break;
//					if (outpath.empty())
//					{
//						outpath = param;
//					}
//					else
//					{
//						Log::Error("Output file already specified as '%s'", outpath.c_str());
//					}
//				} break;
//
//			case 'i':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//						break;
//					if (inpath.empty())
//					{
//						inpath = param;
//					}
//					else
//					{
//						Log::Error("Input file already specified as '%s'", inpath.c_str());
//					}
//				} break;
//
//			default:
//				Log::Error("Unknown argument specified '%s'", arg);
//				break;
//			}
//		}
//		else if (inpath.empty())
//		{
//			inpath = arg;
//		}
//		else if (outpath.empty())
//		{
//			outpath = arg;
//		}
//	}
//#pragma endregion
//
//	//if (inpath.empty()){
//	//	HelpString(hkxcmd::htLong);
//	//	return false;
//	//}
//	if (PathIsDirectory(inpath.c_str()))
//	{
//		//char path[MAX_PATH];
//		//strcpy(path, inpath.c_str());
//		//PathAddBackslash(path);
//		//strcat(path, "skeleton.nif");
//		//GetFullPathName(path, MAX_PATH, path, NULL);
//		//inpath = path;
//	}
//
//
//
//	char rootDir[MAX_PATH];
//	strcpy(rootDir, inpath.c_str());
//	GetFullPathName(rootDir, MAX_PATH, rootDir, NULL);
//	if (!PathIsDirectory(rootDir))
//		PathRemoveFileSpec(rootDir);
//
//	// explicit exclusions due to crashes
//	stringlist excludes;
//	excludes.push_back("*project.hkx");
//	excludes.push_back("*behavior.hkx");
//	excludes.push_back("*charater.hkx");
//	excludes.push_back("*character.hkx");
//
//	//vector<string> files;
//	//FindFiles(files, inpath.c_str(), excludes);
//	//if (files.empty() || files.size() != 1)
//	//{
//	//	Log::Error("No files found in '%s'", inpath.c_str());
//	//	return false;
//	//}
//
//	hkMallocAllocator baseMalloc;
//	// Need to have memory allocated for the solver. Allocate 1mb for it.
//	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024) );
//	hkBaseSystem::init( memoryRouter, errorReport );
//    LoadDefaultRegistry();
//
//	{
//		//Verify path:
//		Log::Info("Creature path: %s", inpath.c_str());
//
//		//Search assets:
//		stringlist excludes;
//		stringlist includes;
//
//		//animations
//		includes.push_back("*.kf");
//		vector<string> animations;
//		FindFiles(animations, inpath.c_str(), excludes, true, includes);
//
//		//meshes		
//		includes.clear();
//		excludes.clear();
//		includes.push_back("*.nif");
//		excludes.push_back("*skeleton.nif");
//		vector<string> meshes;
//		FindFiles(meshes, inpath.c_str(), excludes, true, includes);
//
//		//skeleton
//		vector<string> skeleton;
//		includes.clear();
//		excludes.clear();
//		includes.push_back("*skeleton.nif");
//		FindFiles(skeleton, inpath.c_str(), excludes, true, includes);
//
//		for (vector<string>::iterator itr = skeleton.begin(); itr != skeleton.end(); ++itr)
//			Log::Info("Found Skeleton: %s\n", itr->c_str());
//
//		for (vector<string>::iterator itr = meshes.begin(); itr != meshes.end(); ++itr)
//			Log::Info("Found Mesh: %s\n", itr->c_str());
//
//		for (vector<string>::iterator itr = animations.begin(); itr != animations.end(); ++itr)
//			Log::Info("Found Animation: %s\n", itr->c_str());
//
//		Log::Info("Loading all animations...\n");
//
//		map<string, vector<NiControllerSequenceRef>> animationBlocksMap;
//		set<string> bonesFoundIntoAnimations;
//
//		for (vector<string>::iterator itr = animations.begin(); itr != animations.end(); ++itr) {
//			//Animation roots
//			Log::Info("Loading %s ...", itr->c_str());
//			vector<NiControllerSequenceRef> blocks = Niflib::DynamicCast<NiControllerSequence>(Niflib::ReadNifList(itr->c_str(), NULL /*, &options*/));
//			animationBlocksMap[*itr] = blocks;
//
//			for (auto& controllerBlock : blocks) {
//				if (controllerBlock != NULL) {
//					for (auto& controlledBlock : controllerBlock->GetControlledBlocks()) {
//						int offset = controlledBlock.nodeNameOffset;
//						bonesFoundIntoAnimations.insert(controllerBlock->GetStringPalette()->GetSubStr(offset));
//					}
//				}
//			}
//		}
//
//		Log::Info("Found skeleton hints\n");
//		//read the skeleton
//		vector<NiObjectRef> skeleton_blocks = Niflib::ReadNifList(skeleton[0], NULL /*, &options*/);
//
//		vector<NiNodeRef> skeleton_nodes = DynamicCast<NiNode>(skeleton_blocks);
//		vector<NiNodeRef> bones;
//
//		for (auto& node : skeleton_nodes) {
//			//Normal Bones
//			if (node->GetName().find("Bip") != string::npos) {
//				bones.push_back(node);
//				bonesFoundIntoAnimations.erase(node->GetName());
//			}
//			//Weapon
//			if (node->GetName().find("Weapon") != string::npos) {
//				bones.push_back(node);
//				bonesFoundIntoAnimations.erase(node->GetName());
//			}
//			//Magic
//			if (node->GetName().find("magicNode") != string::npos) {
//				bones.push_back(node);
//				bonesFoundIntoAnimations.erase(node->GetName());
//			}
//		}
//
//		Log::Info("Load Meshes and search for missing bones\n");
//
//		map<string, vector<NiObjectRef>> meshesBlocksMap;
//
//		for (vector<string>::iterator itr = meshes.begin(); itr != meshes.end(); ++itr) {
//			//Animation roots
//			Log::Info("Loading %s ...", itr->c_str());
//			vector<NiObjectRef> blocks = Niflib::ReadNifList(itr->c_str(), NULL /*, &options*/);
//			meshesBlocksMap[*itr] = blocks;
//
//			vector<NiNodeRef> mesh_nodes = DynamicCast<NiNode>(blocks);
//			vector<NiTriStripsRef> meshes = DynamicCast<NiTriStrips>(blocks);
//			vector<NiStringExtraDataRef> stringDatas = DynamicCast<NiStringExtraData>(blocks);
//
//			//for (auto& node : mesh_nodes) {
//			//	if (bonesFoundIntoAnimations.find(node->GetName()) != bonesFoundIntoAnimations.end()) {
//			//		//Found the bone
//
//			//		bonesFoundIntoAnimations.erase(node->GetName());
//			//	}
//			//}
//
//			for (auto& node : meshes) {
//				if (bonesFoundIntoAnimations.find(node->GetName()) != bonesFoundIntoAnimations.end()) {
//					//Found the bone, get the attachment
//					string parentBoneName = "";
//					for (auto& stringdata : stringDatas) {
//						if (stringdata->GetName() == "Prn") {
//							parentBoneName = stringdata->GetData();
//						}
//					}
//					if (parentBoneName == "") throw runtime_error("Unable to find parent bone name");
//					//find the actual parent bone
//					for (vector<NiNodeRef>::iterator i = bones.begin(); i < bones.end(); i++) {
//						NiNodeRef bone = *i;
//						//create a fake bone and insert into the skeleton
//						if (bone->GetName() == parentBoneName) {
//							NiNodeRef fakeBone = DynamicCast<NiNode>(NiNode::Create());
//							fakeBone->SetName(node->GetName());
//							fakeBone->SetParent(bone);
//							fakeBone->SetLocalTransform(node->GetLocalTransform());
//							i = bones.insert(++i, fakeBone);
//						}
//					}
//					bonesFoundIntoAnimations.erase(node->GetName());
//				}
//			}
//		}
//
//		Log::Info("Build skeleton\n");
//
//		NiNodeRef skeleton_root = NULL;
//
//		//create parentMap
//		vector<int> parentMap(bones.size());
//		for (size_t i = 0; i < bones.size(); i++) {
//			NiNodeRef bone = bones[i];
//			NiNodeRef parent = bone->GetParent();
//			while (parent != NULL) {
//				auto parentBone = find(bones.begin(), bones.end(), parent);
//				if (parentBone != bones.end())
//				{
//					parentMap[i] = parentBone - bones.begin();
//					break;
//				}
//				parent = parent->GetParent();
//			}
//			if (parent == NULL) {
//				parentMap[i] = -1;
//				skeleton_root = bone;
//			}
//		}
//
//		Log::Info("Parent map created, root bone: %s\n", skeleton_root->GetName().c_str());
//
//		hkRefPtr<hkaSkeleton> hkSkeleton = new hkaSkeleton();
//		hkSkeleton->m_name = skeleton_root->GetName().c_str();
//
//		//Allocate
//		hkSkeleton->m_parentIndices.setSize(bones.size());
//		hkSkeleton->m_bones.setSize(bones.size());
//		hkSkeleton->m_referencePose.setSize(bones.size());
//
//		for (size_t i = 0; i < bones.size(); i++) {
//			NiNodeRef bone = bones[i];
//			//parent map
//			hkSkeleton->m_parentIndices[i] = parentMap[i];
//
//			//bone track
//			hkaBone& hkBone = hkSkeleton->m_bones[i];
//			hkBone.m_name = bone->GetName().c_str();
//			if (bone == skeleton_root)
//				hkBone.m_lockTranslation = false;
//			else
//				hkBone.m_lockTranslation = true;
//
//			double scale = 1.0;
//
//			//reference pose
//			hkSkeleton->m_referencePose[i].setTranslation(TOVECTOR4(bone->GetLocalTransform().GetTranslation()*scale));
//			hkSkeleton->m_referencePose[i].setRotation(TOQUAT(bone->GetLocalTransform().GetRotation().AsQuaternion()));
//			hkSkeleton->m_referencePose[i].setScale(hkVector4(bone->GetLocalTransform().GetScale(), bone->GetLocalTransform().GetScale(), bone->GetLocalTransform().GetScale()));
//		}
//
//		hkRootLevelContainer rootCont;
//		hkRefPtr<hkaAnimationContainer> skelAnimCont = new hkaAnimationContainer();
//		skelAnimCont->m_skeletons.append(&hkSkeleton,1);
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Merged Animation Container", skelAnimCont.val(), &skelAnimCont->staticClass()));
//
//		Log::Info("Build ragdoll Skeleton\n");
//
//		//collect rigid bodies
//		vector<bhkBlendCollisionObjectRef> rigidBodies = DynamicCast<bhkBlendCollisionObject>(skeleton_blocks);
//		vector<string> rbNames(rigidBodies.size());
//		//create ragdoll parentMap and bone maps
//		vector<int> ragdollParentMap(rigidBodies.size());
//		vector<int> ragdollAnimationParentMap(rigidBodies.size());
//		vector<int> animationRagdollParentMap;
//
//		bhkBlendCollisionObjectRef ragdoll_root = NULL;
//		int ragdoll_root_index = -1;
//
//		//blend collisions don't have parent, look forward to the skeleton
//		for (size_t i = 0; i < bones.size(); i++) {
//			NiNodeRef bone = bones[i];
//			NiNodeRef parent = bone->GetParent();
//			bhkBlendCollisionObjectRef rb = DynamicCast<bhkBlendCollisionObject>(bone->GetCollisionObject());
//			if (rb == NULL) continue;
//			int rbIndex = find(rigidBodies.begin(), rigidBodies.end(), rb) - rigidBodies.begin();
//			//set the skeletal parent for this rigid body
//			ragdollAnimationParentMap[rbIndex] = i;
//			rbNames[rbIndex] = "Ragdoll_" + bone->GetName();
//			//find the ragdoll parent
//			bhkBlendCollisionObjectRef rbParent = NULL;
// 			while (parent != NULL) {
//				auto parentBone = find(bones.begin(), bones.end(), parent);
//				rbParent = DynamicCast<bhkBlendCollisionObject>(parent->GetCollisionObject());
//				if (rbParent != NULL)
//				{
//					int rbParentIndex = find(rigidBodies.begin(), rigidBodies.end(), rbParent) - rigidBodies.begin();
//					ragdollParentMap[rbIndex] = rbParentIndex;
//					break;
//				}
//				parent = parent->GetParent();
//			}
//			if (rbParent == NULL) {
//				ragdollParentMap[rbIndex] = -1;
//				ragdoll_root = rb;
//				ragdoll_root_index = find(rigidBodies.begin(), rigidBodies.end(), ragdoll_root) - rigidBodies.begin();
//			}
//		}
//
//		Log::Info("Ragdoll Parent map created, root bone: %s\n", rbNames[ragdoll_root_index].c_str());
//
//		hkRefPtr<hkaSkeleton> hkRagdollSkeleton = new hkaSkeleton();
//		hkRagdollSkeleton->m_name = rbNames[ragdoll_root_index].c_str();
//
//		//Allocate
//		hkRagdollSkeleton->m_parentIndices.setSize(rigidBodies.size());
//		hkRagdollSkeleton->m_bones.setSize(rigidBodies.size());
//		hkRagdollSkeleton->m_referencePose.setSize(rigidBodies.size());
//
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			bhkBlendCollisionObjectRef body = rigidBodies[i];
//			bhkRigidBodyRef rbody = body->GetBody();
//			NiNodeRef bone = bones[ragdollAnimationParentMap[i]];
//			//parent map
//			hkRagdollSkeleton->m_parentIndices[i] = ragdollParentMap[i];
//
//			double scale = 1.0;
//
//			//bone track
//			hkaBone& hkBone = hkRagdollSkeleton->m_bones[i];
//			hkBone.m_name = rbNames[i].c_str();
//			if (i == ragdoll_root_index) {
//				hkBone.m_lockTranslation = false;
//				
//
//				hkRagdollSkeleton->m_referencePose[i].setTranslation(TOVECTOR4(bone->GetWorldTransform().GetTranslation()*scale));
//				hkRagdollSkeleton->m_referencePose[i].setRotation(TOQUAT(bone->GetWorldTransform().GetRotation().AsQuaternion()));
//				hkRagdollSkeleton->m_referencePose[i].setScale(hkVector4(bone->GetWorldTransform().GetScale(), bone->GetLocalTransform().GetScale(), bone->GetLocalTransform().GetScale()));
//
//				//hkQsTransform rbTransform(hkQsTransform::IdentityInitializer);
//				//rbTransform.setTranslation(TOVECTOR4(rbody->GetTranslation()*scale));
//				//rbTransform.setRotation(TOQUAT(rbody->GetRotation()));
//				//rbTransform.setScale(hkVector4(1.0, 1.0, 1.0));
//
//				//hkRagdollSkeleton->m_referencePose[i].setMul(hkRagdollSkeleton->m_referencePose[i], rbTransform);
//			}
//			else {
//				hkBone.m_lockTranslation = true;
//				//calculate the previous rb world transform
//				int findroot = ragdollParentMap[i];
//				hkQsTransform previous(hkQsTransform::IDENTITY);
//				while (findroot!=-1) {
//					previous.setMul(hkRagdollSkeleton->m_referencePose[findroot], previous);
//					findroot = ragdollParentMap[findroot];
//				}
//
//				//calculate the relative transform between the parent rb bone
//				//hkQsTransform& previous = hkRagdollSkeleton->m_referencePose[ragdollParentMap[i]];
//				hkQsTransform& next = hkRagdollSkeleton->m_referencePose[i];
//
//				next.setTranslation(TOVECTOR4(bone->GetWorldTransform().GetTranslation()*scale));
//				next.setRotation(TOQUAT(bone->GetWorldTransform().GetRotation().AsQuaternion()));
//				next.setScale(hkVector4(bone->GetWorldTransform().GetScale(), bone->GetWorldTransform().GetScale(), bone->GetWorldTransform().GetScale()));
//
//				//hkQsTransform rbTransform;
//				//rbTransform.setTranslation(TOVECTOR4(rbody->GetTranslation()*scale));
//				//rbTransform.setRotation(TOQUAT(rbody->GetRotation()));
//				//rbTransform.setScale(hkVector4(1.0,1.0,1.0));
//				//next.setMul(next, rbTransform);
//
//				next.setMulInverseMul(previous, next);
//			}
//		}
//
//		skelAnimCont->m_skeletons.append(&hkRagdollSkeleton, 1);
//
//		Log::Info("Build Mappings Ragdoll -> Skeleton\n");
//
//		hkaSkeletonMapperData* fromRagdollToSkeletonMapping = new hkaSkeletonMapperData();
//		fromRagdollToSkeletonMapping->m_simpleMappings.setSize(rigidBodies.size());
//		fromRagdollToSkeletonMapping->m_skeletonA = hkRagdollSkeleton;
//		fromRagdollToSkeletonMapping->m_skeletonB = hkSkeleton;
//		set<int> mappedBones;
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			hkaSkeletonMapperData::SimpleMapping& mapping = fromRagdollToSkeletonMapping->m_simpleMappings[i];
//			mapping.m_boneA = i;
//			mapping.m_boneB = ragdollAnimationParentMap[i];
//			mappedBones.insert(ragdollAnimationParentMap[i]);
//
//			//Absolute transform
//			int findroot = ragdollParentMap[i];
//			hkQsTransform ragdollBoneTransform = hkRagdollSkeleton->m_referencePose[i];
//			while (findroot != -1) {
//				ragdollBoneTransform.setMul(hkRagdollSkeleton->m_referencePose[findroot], ragdollBoneTransform);
//				findroot = ragdollParentMap[findroot];
//			}
//
//			NiNodeRef animationBone = bones[ragdollAnimationParentMap[i]];
//
//			hkQsTransform animationBoneTransform;
//			double scale = 1.0;
//			animationBoneTransform.setTranslation(TOVECTOR4(animationBone->GetWorldTransform().GetTranslation()*scale));
//			animationBoneTransform.setRotation(TOQUAT(animationBone->GetWorldTransform().GetRotation().AsQuaternion()));
//			animationBoneTransform.setScale(hkVector4(animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale()));
//
//			mapping.m_aFromBTransform.setMulInverseMul(ragdollBoneTransform, animationBoneTransform);
//		}
//
//		for (int i = 0; i < bones.size(); i++) {
//			if (mappedBones.find(i) == mappedBones.end())
//				fromRagdollToSkeletonMapping->m_unmappedBones.pushBack(i);
//		}
//
//		hkRefPtr<hkaSkeletonMapper> ragdollToAnimationMapper = new hkaSkeletonMapper(*fromRagdollToSkeletonMapping);
//
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("SkeletonMapper", ragdollToAnimationMapper.val(), &ragdollToAnimationMapper->staticClass()));
//
//
//		Log::Info("Build Mappings Skeleton -> Ragdoll\n");
//
//		hkaSkeletonMapperData* fromSkeletonToRagdollMapping = new hkaSkeletonMapperData();
//		fromSkeletonToRagdollMapping->m_simpleMappings.setSize(rigidBodies.size());
//		fromSkeletonToRagdollMapping->m_skeletonA = hkSkeleton;
//		fromSkeletonToRagdollMapping->m_skeletonB = hkRagdollSkeleton;
//
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			hkaSkeletonMapperData::SimpleMapping& mapping = fromSkeletonToRagdollMapping->m_simpleMappings[i];
//			mapping.m_boneA = ragdollAnimationParentMap[i];
//			mapping.m_boneB = i;
//			mappedBones.insert(ragdollAnimationParentMap[i]);
//
//			//Absolute transform
//			int findroot = ragdollParentMap[i];
//			hkQsTransform ragdollBoneTransform = hkRagdollSkeleton->m_referencePose[i];
//			while (findroot != -1) {
//				ragdollBoneTransform.setMul(hkRagdollSkeleton->m_referencePose[findroot], ragdollBoneTransform);
//				findroot = ragdollParentMap[findroot];
//			}
//
//			NiNodeRef animationBone = bones[ragdollAnimationParentMap[i]];
//
//			hkQsTransform animationBoneTransform;
//			double scale = 1.0;
//			animationBoneTransform.setTranslation(TOVECTOR4(animationBone->GetWorldTransform().GetTranslation()*scale));
//			animationBoneTransform.setRotation(TOQUAT(animationBone->GetWorldTransform().GetRotation().AsQuaternion()));
//			animationBoneTransform.setScale(hkVector4(animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale()));
//
//			mapping.m_aFromBTransform.setMulInverseMul(animationBoneTransform, ragdollBoneTransform);
//		}
//
//		hkRefPtr<hkaSkeletonMapper> animationToRagdollMapper = new hkaSkeletonMapper(*fromSkeletonToRagdollMapping);
//
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("SkeletonMapper", animationToRagdollMapper.val(), &animationToRagdollMapper->staticClass()));
//
//		hkArray<hkpRigidBody*> hkpBodies(rigidBodies.size());
//
//		//hkArrayBase<hkpConstraintInstance*> constraints();
//
//		//Rigid Bodies
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			bhkRigidBodyRef bhkRB = rigidBodies[i]->GetBody();
//
//			//rigid body info
//			hkpRigidBodyCinfo hkpRbInfo;
//
//			hkpRbInfo.m_allowedPenetrationDepth = bhkRB->GetPenetrationDepth();
//			hkpRbInfo.m_angularDamping = bhkRB->GetAngularDamping();
//			hkpRbInfo.m_angularVelocity = TOVECTOR4(bhkRB->GetAngularVelocity());
//			hkpRbInfo.m_centerOfMass = TOVECTOR4(bhkRB->GetCenter());
//			//hkpRbInfo.m_collisionFilterInfo = bhkRB->GetLayer();
//			//hkpRbInfo.m_collisionResponse = hkpMaterial::ResponseType(bhkRB->GetCollisionResponse());
//			//hkpRbInfo.m_contactPointCallbackDelay = bhkRB->pin;
//			//hkpRbInfo.m_enableDeactivation = bhkRB->de
//			//hkpRbInfo.m_forceCollideOntoPpu = bhkRB->;
//			hkpRbInfo.m_friction = bhkRB->GetFriction();
//			//hkpRbInfo.m_gravityFactor = bhkRB->get;
//			hkpRbInfo.m_inertiaTensor = TOMATRIX3(bhkRB->GetInertia());
//			hkpRbInfo.m_linearDamping = bhkRB->GetLinearDamping();
//			hkpRbInfo.m_linearVelocity = TOVECTOR4(bhkRB->GetLinearVelocity());
//			//hkpRbInfo.m_localFrame;
//			hkpRbInfo.m_mass = bhkRB->GetMass();
//			hkpRbInfo.m_maxAngularVelocity = bhkRB->GetMaxAngularVelocity();
//			hkpRbInfo.m_maxLinearVelocity = bhkRB->GetMaxLinearVelocity();
//			//hkpRbInfo.m_motionType = TOMOTIONTYPE(bhkRB->GetMotionSystem());
//			//hkpRbInfo.m_numShapeKeysInContactPointProperties;
//			hkpRbInfo.m_position = TOVECTOR4(bhkRB->GetTranslation()*7);
//			//hkpRbInfo.m_qualityType = TOMOTIONQUALITY(bhkRB->GetQualityType());
//			//hkpRbInfo.m_responseModifierFlags;
//			hkpRbInfo.m_restitution = bhkRB->GetRestitution();
//			//hkpRbInfo.m_rollingFrictionMultiplier = bhkRB->RO;
//			hkpRbInfo.m_rotation = TOQUAT(bhkRB->GetRotation());
//			//hkpRbInfo.m_shape;
//			
//
//			ShapeBuilder().visitShape(bhkRB->GetShape(), hkpRbInfo);
//			//makeShape(*shape, hkpRbInfo);
//			hkpRbInfo.m_solverDeactivation;
//			hkpRbInfo.m_timeFactor;
//
//			hkpBodies[i] = new hkpRigidBody(hkpRbInfo);
//	
//		}
//
//		//contraints
//		hkArray<hkpConstraintInstance*> constraints;
//		
//		for (int i = 0; i < rigidBodies.size(); i++) {
//			bhkRigidBodyRef bhkRB = rigidBodies[i]->GetBody();
//			vector<bhkSerializableRef> bhkConstraints = bhkRB->GetConstraints();
//			if (bhkConstraints.empty()) continue;
//			
//			for (int j = 0; j < bhkConstraints.size(); j++) {
//				if (bhkConstraints[j] == NULL) continue;
//				constraints.pushBack(ConstraintBuilder(rigidBodies, hkpBodies).visitConstraint(bhkConstraints[j]));
//				/*vector<int> entitiesIndexes;
//				if (bhkConstraints[j]->IsDerivedType(bhkConstraint::TYPE)) {
//					bhkConstraintRef c = bhkConstraints[j];
//					
//					for (bhkEntity* e : c->GetEntities()) {
//						for (int k = 0; k < rigidBodies.size(); k++) {
//							if (e == rigidBodies[k]->GetBody())
//								entitiesIndexes.push_back(k);
//						}
//					}
//				}
//				if (entitiesIndexes.size() == 2){
//					constraints.pushBack(new hkpConstraintInstance(hkpBodies[entitiesIndexes[1]], hkpBodies[entitiesIndexes[0]], new hkpRagdollConstraintData()));
//				}*/
//			}
//			
//		}
//
//		//hkaRagdollInstance ( const hkArrayBase<hkpRigidBody*>& rigidBodies, const hkArrayBase<hkpConstraintInstance*>& constraints, const hkaSkeleton* skeleton );
//		hkRefPtr<hkaRagdollInstance> ragdoll = new hkaRagdollInstance(hkpBodies, constraints, hkRagdollSkeleton.val());
//
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("RagdollInstance", ragdoll.val(), &ragdoll->staticClass()));
//
//		//Physics system
//
//
//		hkRefPtr<hkpPhysicsSystem> system = new hkpPhysicsSystem();
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			system->addRigidBody(hkpBodies[i]);
//		}
//		for (int i = 0; i < rigidBodies.size() - 1; i++) {
//			system->addConstraint(constraints[i]);
//		}
//
//		hkRefPtr<hkpPhysicsData> data = new hkpPhysicsData();
//		data->addPhysicsSystem(system.val());
//
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Physics Data", data.val(), &data->staticClass()));
//
//		//hkRefPtr<hkaSkeleton> hkRagdollSkeleton = new hkaSkeleton();
//		//hkRagdollSkeleton->m_name = skeleton_root->GetName().c_str();
//
//		//Allocate
//		hkSkeleton->m_parentIndices.setSize(bones.size());
//		hkSkeleton->m_bones.setSize(bones.size());
//		hkSkeleton->m_referencePose.setSize(bones.size());
//
//
//		Log::Info("Exporting 'skeleton.hkx'");
//
//		hkPackFormat pkFormat = HKPF_DEFAULT;
//		hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;
//		hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);
//
//		hkOstream stream("skeleton.hkx");
//		hkVariant root = { &rootCont, &rootCont.staticClass() };
//		hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
//		if (res != HK_SUCCESS)
//		{
//			Log::Error("Havok reports save failed.");
//		}
//	}
//
//	hkBaseSystem::quit();
//	hkMemoryInitUtil::quit();
//
//
//	return true;
//}
//static bool SafeExecuteCmd(hkxcmdLine &cmdLine)
//{
//   __try{
//      return ExecuteCmd(cmdLine);
//   } __except (EXCEPTION_EXECUTE_HANDLER){
//      return false;
//   }
//}
//
//REGISTER_COMMAND(Skeleton, HelpString, SafeExecuteCmd);
//
