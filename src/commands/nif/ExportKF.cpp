//#pragma region Headers
//#include "stdafx.h"
//
//#include <core/hkxcmd.h>
//#include <core/hkxutils.h>
//#include <core/hkfutils.h>
//#include <core/log.h>
//
//#include <direct.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//#pragma region Niflib Headers
////////////////////////////////////////////////////////////////////////////
//// Niflib Includes
////////////////////////////////////////////////////////////////////////////
//#include <niflib.h>
//#include <nif_io.h>
//#include "obj/NiObject.h"
//#include "obj/NiNode.h"
//#include "obj/NiTexturingProperty.h"
//#include "obj/NiSourceTexture.h"
//#include "obj/NiTriBasedGeom.h"
//#include "obj/NiTriBasedGeomData.h"
//#include "obj/NiTriShape.h"
//#include "obj/NiTriStrips.h"
//#include <obj/NiControllerSequence.h>
//#include <obj/NiControllerManager.h>
//#include <obj/NiInterpolator.h>
//#include <obj/NiTransformInterpolator.h>
//#include <obj/NiTransformData.h>
//#include <obj/NiTransformController.h>
//#include <obj/NiTimeController.h>
//#include <obj/NiTransformController.h>
//#include <obj/NiTextKeyExtraData.h>
//#include <obj/NiKeyframeController.h>
//#include <obj/NiKeyframeData.h>
//#include <obj/NiStringPalette.h>
//#include <obj/NiBSplineTransformInterpolator.h>
//#include <obj/NiDefaultAVObjectPalette.h>
//#include <obj/NiMultiTargetTransformController.h>
//#include <obj/NiGeomMorpherController.h>
//#include <obj/NiMorphData.h>
//#include <obj/NiBSplineCompFloatInterpolator.h>
//#include <obj/NiFloatInterpolator.h>
//#include <obj/NiFloatData.h>
//#include <Key.h>
//
//typedef Niflib::Key<float> FloatKey;
//typedef Niflib::Key<Niflib::Quaternion> QuatKey;
//typedef Niflib::Key<Niflib::Vector3> Vector3Key;
//typedef Niflib::Key<Niflib::IndexString> StringKey;
//
//#pragma endregion
//
//#pragma region Havok Headers
////////////////////////////////////////////////////////////////////////////
//// Havok Includes
////////////////////////////////////////////////////////////////////////////
//
//#include <Common/Base/hkBase.h>
//#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
//#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
//#include <Common/Base/System/Io/IStream/hkIStream.h>
//
//#include <cstdio>
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
//// Animation
//#include <Animation/Animation/Rig/hkaSkeleton.h>
//#include <Animation/Animation/hkaAnimationContainer.h>
//#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
//#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
//#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
//#include <Animation/Animation/Animation/SplineCompressed/hkaSplineCompressedAnimation.h>
//#include <Animation/Animation/Rig/hkaPose.h>
//#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
//#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
//#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
//#include <Animation/Animation/Rig/hkaSkeletonUtils.h>
//
//// Serialize
//#include <Common/Serialize/Util/hkSerializeUtil.h>
//#pragma endregion
//
////////////////////////////////////////////////////////////////////////////
//// Our Includes
////////////////////////////////////////////////////////////////////////////
//#include <float.h>
//#include <cstdio>
//#include <sys/stat.h>
//using namespace Niflib;
//using namespace std;
//
//#pragma endregion
//
////////////////////////////////////////////////////////////////////////////
//// Enumeration Types
////////////////////////////////////////////////////////////////////////////
//namespace {
//enum {
//	IPOS_X_REF	=	0,
//	IPOS_Y_REF	=	1,
//	IPOS_Z_REF	=	2,
//	IPOS_W_REF	=	3,
//};
//enum AccumType
//{
//	AT_NONE = 0,
//	AT_X = 0x01,
//	AT_Y = 0x02,
//	AT_Z = 0x04,
//
//	AT_XYZ = AT_X | AT_Y | AT_Z,
//	AT_FORCE = 0x80000000,
//};
//
//}
////////////////////////////////////////////////////////////////////////////
//// Constants
////////////////////////////////////////////////////////////////////////////
//
//const unsigned int IntegerInf = 0x7f7fffff;
//const unsigned int IntegerNegInf = 0xff7fffff;
//const float FloatINF = *(float*)&IntegerInf;
//const float FloatNegINF = *(float*)&IntegerNegInf;
////////////////////////////////////////////////////////////////////////////
//// Structures
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
//// Helper functions
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
//// Classes
////////////////////////////////////////////////////////////////////////////
//
//struct AnimationExport
//{
//	AnimationExport(NiControllerSequenceRef seq, hkRefPtr<hkaSkeleton> skeleton, hkRefPtr<hkaAnimationBinding> binding);
//
//	bool doExport();
//	bool exportNotes( );
//	bool exportController();
//
//	//bool SampleAnimation( INode * node, Interval &range, PosRotScale prs, NiKeyframeDataRef data );
//	NiControllerSequenceRef seq;
//	hkRefPtr<hkaAnimationBinding> binding;
//	hkRefPtr<hkaSkeleton> skeleton;
//};
//
//
//float QuatDot(const Quaternion& q, const Quaternion&p)
//{
//	return q.w*p.w + q.x*p.x + q.y*p.y + q.z*p.z;
//}
//
//AnimationExport::AnimationExport(NiControllerSequenceRef seq, hkRefPtr<hkaSkeleton> skeleton, hkRefPtr<hkaAnimationBinding> binding)
//{
//	this->seq = seq;
//	this->binding = binding;
//	this->skeleton = skeleton;
//}
//
//bool AnimationExport::doExport()
//{
//	hkStringPtr rootName = binding->m_originalSkeletonName;
//	seq->SetStartTime(FloatINF);
//	seq->SetStopTime(FloatINF);
//	seq->SetFrequency(1.0f);
//	seq->SetCycleType( CYCLE_CLAMP );
//	seq->SetAccumRootName(string(rootName));
//
//	seq->SetStartTime(0.0);
//
//	if (!exportNotes())
//		return false;
//
//	return exportController();
//}
//
//bool AnimationExport::exportNotes( )
//{
//	vector<StringKey> textKeys;
//
//#if 0
//	NiTextKeyExtraDataRef textKeyData = new NiTextKeyExtraData();
//	seq->SetTextKey(textKeyData);
//
//	//seq->SetName( note.m_text );
//
//	AccumType accumType = AT_NONE;
//
//	hkRefPtr<hkaAnimation> anim = binding->m_animation;
//	const hkInt32 numAnnotationTracks = anim->m_annotationTracks.getSize();
//
//	// Scan the animation for get up points
//	for (hkInt32 t=0; t< numAnnotationTracks; ++t )
//	{
//		hkaAnnotationTrack& track = anim->m_annotationTracks[t];
//		const hkInt32 numAnnotations = track.m_annotations.getSize();
//		for( hkInt32 a = 0; a < numAnnotations; ++a )
//		{
//			hkaAnnotationTrack::Annotation& note = track.m_annotations[a];
//
//			if ( seq->GetStartTime() == FloatINF || seq->GetStartTime() > note.m_time )
//				seq->SetStartTime(note.m_time);
//			if ( seq->GetStopTime() == FloatINF || seq->GetStopTime() < note.m_time )
//				seq->SetStopTime(note.m_time);
//
//			StringKey strkey;
//			strkey.time = note.m_time;
//			strkey.data = note.m_text;
//			textKeys.push_back(strkey);
//		}
//	}
//
//	textKeyData->SetKeys(textKeys);
//#endif
//	return true;
//}
//
//
//
//static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v){
//	return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
//}
//
//static inline Niflib::Vector4 TOVECTOR4(const hkVector4& v){
//	return Niflib::Vector4(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
//}
//
//static inline Niflib::Quaternion TOQUAT(const ::hkQuaternion& q, bool inverse = false){
//	Niflib::Quaternion qt(q.m_vec.getSimdAt(3), q.m_vec.getSimdAt(0), q.m_vec.getSimdAt(1), q.m_vec.getSimdAt(2));
//	return inverse ? qt.Inverse() : qt;
//}
//static inline Niflib::Quaternion TOQUAT(const hkRotation& rot, bool inverse = false){
//	return TOQUAT(::hkQuaternion(rot), inverse);
//}
//static inline float Average(const Niflib::Vector3& val) {
//	return (val.x + val.y + val.z) / 3.0f;
//}
//
//const float MY_FLT_EPSILON = 1e-5f;
//static inline bool EQUALS(float a, float b){
//	return fabs(a - b) < MY_FLT_EPSILON;
//}
//static inline bool EQUALS(float a, float b, float epsilon){
//   return fabs(a - b) < epsilon;
//}
//
//static inline bool EQUALS(const Niflib::Vector3& a, const Niflib::Vector3& b){
//	return (EQUALS(a.x,b.x) && EQUALS(a.y, b.y) && EQUALS(a.z, b.z) );
//}
//
//static inline bool EQUALS(const Niflib::Quaternion& a, const Niflib::Quaternion& b){
//	return EQUALS(a.w, b.w) && EQUALS(a.x,b.x) && EQUALS(a.y, b.y) && EQUALS(a.z, b.z);
//}
//
//
//struct BoneDataReference
//{
//	string name;
//	NiTransformControllerRef transCont;
//	NiTransformInterpolatorRef interpolator;
//	NiTransformDataRef transData;
//	vector<Vector3Key> trans;
//	vector<QuatKey> rot;
//	vector<FloatKey> scale;
//
//	Vector3 lastTrans;
//	Quaternion lastRotate;
//	float lastScale;
//};
//
//bool AnimationExport::exportController()
//{
//	hkRefPtr<hkaAnimation> anim = binding->m_animation;
//	int nframes = anim->getNumOriginalFrames();
//	int numTracks = anim->m_numberOfTransformTracks;
//	int nfloats = anim->m_numberOfFloatTracks;
//
//	if (numTracks == 0)
//		return false;
//
//	float duration = anim->m_duration;
//	seq->SetStopTime(duration);
//
//	int nbones = skeleton->m_bones.getSize();
//
//	// dont know how to deal with this
//	if (numTracks > nbones)
//	{
//		Log::Error("Error processing skeleton '%s' number of tracks exceed bones.", (LPCSTR)binding->m_originalSkeletonName);
//		return false;
//	}
//
//	hkLocalArray<float> floatsOut(nfloats);
//	hkLocalArray<hkQsTransform> transformOut(numTracks);
//	floatsOut.setSize(nfloats);
//	transformOut.setSize(numTracks);
//	hkReal startTime = 0.0;
//
//	hkArray<hkInt16> tracks;
//	tracks.setSize(numTracks);
//	for (int i=0; i<numTracks; ++i) tracks[i]=i;
//
//	vector<BoneDataReference> dataList;
//	dataList.resize(numTracks);
//
//   vector<bool> scaleWarn;
//   scaleWarn.resize(numTracks);
//
//	NiObjectNETRef placeHolder = new NiObjectNET();
//	for (int i=0,n=numTracks; i<n; ++i)
//	{
//		NiTransformControllerRef controller = new NiTransformController();
//		NiTransformInterpolatorRef interp = new NiTransformInterpolator();
//		controller->SetInterpolator(interp);
//
//		NiTransformDataRef data = new NiTransformData();
//		interp->SetData(data);
//
//		hkQsTransform localTransform = skeleton->m_referencePose[i];
//		NiQuatTransform niLocalTransform;
//		niLocalTransform.translation = TOVECTOR3(localTransform.getTranslation());
//		niLocalTransform.rotation = TOQUAT(localTransform.getRotation());
//		//interp->SetTransform(niLocalTransform);
//		//interp->SetTransform(NiQuatTransform())
//		//interp->SetTranslation();
//		//interp->SetRotation();
//		//interp->SetScale(Average(TOVECTOR3(localTransform.getScale()))); // no scaling?
//
//		BoneDataReference& boneData = dataList[i];
//		boneData.name = skeleton->m_bones[i].m_name;
//		boneData.transCont = controller;
//		boneData.transData = data;
//		boneData.trans.reserve(nframes);
//		boneData.rot.reserve(nframes);
//		boneData.scale.reserve(nframes);
//		boneData.lastTrans = interp->GetTransform().translation; // GetTranslation();
//		boneData.lastRotate = interp->GetTransform().rotation;
//		boneData.lastScale = interp->GetTransform().scale;
//	}
//
//   hkReal time = startTime;
//	for (int iFrame=0; iFrame<nframes; ++iFrame, time = startTime + (hkReal)iFrame * anim->m_duration / (hkReal)(nframes-1))
//	{
//
//		//hkUint32 uiAnnotations = anim->getNumAnnotations(time, incrFrame);
//		//hkUint32 nAnnotations = anim->getAnnotations(time, incrFrame, annotations.begin(), nbones);
//
//		anim->samplePartialTracks(time, numTracks, transformOut.begin(), nfloats, floatsOut.begin(), HK_NULL);
//      hkaSkeletonUtils::normalizeRotations(transformOut.begin(), numTracks);
//
//		// assume 1-to-1 transforms
//		for (int i=0; i<numTracks; ++i)
//		{
//			BoneDataReference& data = dataList[i];
//			hkQsTransform& transform = transformOut[i];			
//			Vector3Key vk;
//			vk.time = time;
//			vk.data = TOVECTOR3(transform.getTranslation());
//			if (!EQUALS(vk.data,data.lastTrans))
//			{
//				data.trans.push_back(vk);
//				data.lastTrans = vk.data;
//			}
//
//			QuatKey qk;
//			qk.time = time;
//			qk.data = TOQUAT(transform.getRotation()).Normalized();
//			if (!EQUALS(qk.data,data.lastRotate))
//			{
//				data.rot.push_back(qk);
//				data.lastRotate = qk.data;
//			}
//
//			FloatKey sk;
//			sk.time = time;
//         Niflib::Vector3 sc = TOVECTOR3(transform.getScale());
//         if (!EQUALS(sc.x,sc.y,0.01f) && !EQUALS(sc.x,sc.z,0.01f))
//         {
//            if (!scaleWarn[i])
//            {
//               scaleWarn[i] = true;
//               Log::Warn("Non-uniform scaling found while processing '%s'.", skeleton->m_bones[i].m_name.cString());
//            }
//         }
//            
//			sk.data = Average(sc);
//			if (!EQUALS(data.lastScale,sk.data,0.01f))
//			{
//				data.scale.push_back(sk);
//				data.lastScale = sk.data;
//			}
//		}
//	}
//	for (int i=0; i<numTracks; ++i)
//	{
//		bool keep=false;
//		BoneDataReference& data = dataList[i];
//		if (!data.trans.empty())
//		{
//			//data.transData->SetTranslateType(LINEAR_KEY);
//			KeyGroup<Vector3> kg;
//			kg.keys = data.trans;			
//			data.transData->SetTranslations(kg);
//			keep = true;
//		}
//		if (!data.rot.empty())
//		{
//			data.transData->SetRotationType(QUADRATIC_KEY);
//			data.transData->SetQuaternionKeys(data.rot);
//			keep = true;
//		}
//		if (!data.scale.empty())
//		{
//			//data.transData->s(LINEAR_KEY);
//			KeyGroup<float> kg;
//			kg.keys = data.scale;
//			data.transData->SetScales(kg);
//			keep = true;
//		}
//		if (keep)
//		{
//			seq->AddController(data.name, data.transCont);
//		}
//	}
//
//	// Add some text keys for easier export from max
//	{
//		NiTextKeyExtraDataRef textKeys = new NiTextKeyExtraData();
//		vector<StringKey> keys;
//		StringKey first, last;
//		first.time = 0.0; first.data = "start";
//		last.time = duration; last.data = "end";
//		keys.push_back( first );
//		keys.push_back(last);
//		textKeys->SetTextKeys(keys);
//		seq->SetAccumRootName(IndexString("Annotations"));
//		seq->SetTextKeys(textKeys);
//		//seq->set
//		//seq->SetTextKey(textKeys);
//	}
//
//	// remove controllers now
//	vector<ControlledBlock> links = seq->GetControlledBlocks();
//	for (vector<ControlledBlock>::iterator itr = links.begin(); itr != links.end(); ++itr)
//	{
//		NiTransformControllerRef controller = (*itr).controller;
//		(*itr).interpolator = controller->GetInterpolator();
//		(*itr).controller = NULL;
//	}
//	seq->SetControlledBlocks(links);
//
//	return true;
//}
//
//
//void ExportAnimations(const string& rootdir, const string& skelfile, const vector<string>& animlist, const string& outdir, Niflib::NifInfo& nifver, bool norelativepath = false)
//{
//	hkResource* skelResource = NULL;
//	hkResource* animResource = NULL;
//	hkaSkeleton* skeleton = NULL;
//	hkaAnimationContainer * animCont= NULL;
//
//	Log::Verbose("ExportAnimation('%s','%s','%s')", rootdir.c_str(), skelfile.c_str(), outdir.c_str());
//	// Read back a serialized file
//	{
//		hkIstream stream(skelfile.c_str());
//		hkStreamReader *reader = stream.getStreamReader();
//      skelResource = hkSerializeLoadResource(reader);
//		if (skelResource)
//		{
//			const char * hktypename = skelResource->getContentsTypeName();
//			void * contentPtr = skelResource->getContentsPointer(HK_NULL, HK_NULL);
//			hkRootLevelContainer* scene = skelResource->getContents<hkRootLevelContainer>();
//			hkaAnimationContainer *skelAnimCont = scene->findObject<hkaAnimationContainer>();
//			if ( !skelAnimCont->m_skeletons.isEmpty() )
//				skeleton = skelAnimCont->m_skeletons[0];
//		}
//	}
//	if (skeleton != NULL)
//	{
//		for (vector<string>::const_iterator itr = animlist.begin(); itr != animlist.end(); ++itr)
//		{
//			string animfile = (*itr);
//			Log::Verbose("ExportAnimation Starting '%s'", animfile.c_str());
//
//
//			char outfile[MAX_PATH], relout[MAX_PATH];
//			LPCSTR extn = PathFindExtension(outdir.c_str());
//			if (stricmp(extn, ".kf") == 0)
//			{
//				strcpy(outfile, outdir.c_str());
//			}
//			else // assume its a folder
//			{
//				if (norelativepath)
//				{
//					PathCombine(outfile, outdir.c_str(), PathFindFileName(animfile.c_str()));
//				}
//				else
//				{
//					char relpath[MAX_PATH];
//					PathRelativePathTo(relpath, rootdir.c_str(), FILE_ATTRIBUTE_DIRECTORY, animfile.c_str(), 0);
//					PathCombine(outfile, outdir.c_str(), relpath);
//					GetFullPathName(outfile, MAX_PATH, outfile, NULL);
//				}				
//				PathRemoveExtension(outfile);
//				PathAddExtension(outfile, ".kf");
//			}
//			char workdir[MAX_PATH];
//			_getcwd(workdir, MAX_PATH);
//			PathRelativePathTo(relout, workdir, FILE_ATTRIBUTE_DIRECTORY, outfile, 0);
//
//			Log::Verbose("ExportAnimation Reading '%s'", animfile.c_str());
//
//
//			hkIstream stream(animfile.c_str());
//			hkStreamReader *reader = stream.getStreamReader();
//         animResource = hkSerializeLoadResource(reader);
//         if (animResource != NULL)
//         {
//			   const char * hktypename = animResource->getContentsTypeName();
//			   void * contentPtr = animResource->getContentsPointer(HK_NULL, HK_NULL);
//			   hkRootLevelContainer* scene = animResource->getContents<hkRootLevelContainer>();
//			   animCont = scene->findObject<hkaAnimationContainer>();
//			   if (animCont != NULL)
//			   {
//				   if (animCont != NULL)
//				   {
//					   int nbindings = animCont->m_bindings.getSize();
//					   if ( nbindings == 0)
//					   {
//						   Log::Error("Animation file contains no animation bindings.  Not exporting.");
//					   }
//					   else if ( nbindings != 1)
//					   {
//						   Log::Error("Animation file contains more than one animation binding.  Not exporting.");
//					   }
//					   else
//					   {
//						   for ( int i=0, n=animCont->m_bindings.getSize(); i<n; ++i)
//						   {
//							   char fname[MAX_PATH];
//							   _splitpath(animfile.c_str(), NULL, NULL, fname, NULL);
//
//							   hkRefPtr<hkaAnimationBinding> binding = animCont->m_bindings[i];
//							   NiControllerSequenceRef seq = new NiControllerSequence();
//							   seq->SetName(animfile);
//
//							   Log::Verbose("ExportAnimation Exporting '%s'", outfile);
//
//							   AnimationExport exporter(seq, skeleton, binding);
//							   if ( exporter.doExport() )
//							   {
//								   char outfiledir[MAX_PATH];
//								   strcpy(outfiledir, outfile);
//								   PathRemoveFileSpec(outfiledir);
//								   CreateDirectories(outfiledir);
//
//								   Log::Info("Exporting '%s'", relout);
//								   Niflib::WriteNifTree(outfile, seq, nifver);
//							   }
//							   else
//							   {
//								   Log::Error("Export failed for '%s'", relout);
//							   }
//						   }
//					   }
//				   }
//			   }
//			   animResource->removeReference();
//         }
//		}
//	}
//	if (skelResource) skelResource->removeReference();
//}
////////////////////////////////////////////////////////////////////////////
//
//static void HelpString(hkxcmd::HelpType type){
//	switch (type)
//	{
//	case hkxcmd::htShort: Log::Info("ExportKF - Convert Havok HKX animation to Gamebryo KF animation."); break;
//	case hkxcmd::htLong:  
//		{
//			char fullName[MAX_PATH], exeName[MAX_PATH];
//			GetModuleFileName(NULL, fullName, MAX_PATH);
//			_splitpath(fullName, NULL, NULL, exeName, NULL);
//			
//			Log::Info("Usage: %s ExportKF [-opts[modifiers]] [skel.hkx] [anim.hkx] [anim.kf]", exeName); 
//			Log::Info("  Convert Havok HKX animation to Gamebryo KF animation." );
//			Log::Info("  If a folder is specified then the folder will be searched for any projects and convert those." );
//			Log::Info("");
//			Log::Info("<Options>" );
//			Log::Info("  skel.hkx      Path to Havok skeleton for animation binding." );
//			Log::Info("  anim.hkx      Path to Havok animation to convert" );
//			Log::Info("  anim.kf       Path to Gamebryo animation to write (Default: anim.hkx with kf ext)" );
//			Log::Info("<Switches>" );
//			Log::Info(" -d[:level]     Debug Level: ERROR,WARN,INFO,DEBUG,VERBOSE (Default: INFO)" );
//			Log::Info(" -n             Disable recursive file processing" );
//			Log::Info(" -v x.x.x.x     Nif Version to write as - Defaults to 20.2.0.7" );
//			Log::Info(" -u x           Nif User Version to write as - Defaults to 12" );
//			Log::Info(" -u2 x          Nif User2 Version to write as - Defaults to 83" );
//			Log::Info("");
//		}
//		break;
//	}
//}
//
//static void ExportProject( const string &projfile, const char * rootPath, const char * outdir, Niflib::NifInfo& nifver, bool recursion)
//{
//	vector<string> skelfiles, animfiles;
//	char projpath[MAX_PATH], skelpath[MAX_PATH], animpath[MAX_PATH];
//
//	if ( wildmatch("*skeleton.hkx", projfile) )
//	{
//		skelfiles.push_back(projfile);
//
//		GetFullPathName(projfile.c_str(), MAX_PATH, projpath, NULL);
//		PathRemoveFileSpec(projpath);
//		PathAddBackslash(projpath);
//		PathCombine(animpath, projpath, "..\\animations\\*.hkx");
//		FindFiles(animfiles, animpath, recursion);	
//	}
//	else
//	{
//		GetFullPathName(projfile.c_str(), MAX_PATH, projpath, NULL);
//		PathRemoveFileSpec(projpath);
//		PathAddBackslash(projpath);
//		PathCombine(skelpath, projpath, "character assets\\*skeleton.hkx");
//		FindFiles(skelfiles, skelpath, recursion);
//		PathCombine(animpath, projpath, "animations\\*.hkx");
//		FindFiles(animfiles, animpath, recursion);
//	}
//	if (skelfiles.empty())
//	{
//		Log::Warn("No skeletons found. Skipping '%s'", projpath);
//	}
//	else if (skelfiles.size() != 1)
//	{
//		Log::Warn("Multiple skeletons found. Skipping '%s'", projpath);
//	}
//	else if (animfiles.empty())
//	{
//		Log::Warn("No Animations found. Skipping '%s'", projpath);
//	}
//	else
//	{
//		ExportAnimations(string(rootPath), skelfiles[0],animfiles, outdir, nifver, false);
//	}
//}
//
//static bool ExecuteCmd(hkxcmdLine &cmdLine)
//{
//	bool recursion = true;
//	vector<string> paths;
//	int argc = cmdLine.argc;
//	char **argv = cmdLine.argv;
//	Niflib::NifInfo nifver;
//	nifver.version = VER_20_2_0_7;
//	nifver.userVersion = 11;
//	nifver.userVersion2 = 83;
//
//#pragma region Handle Input Args
//	for (int i = 0; i < argc; i++)
//	{
//		char *arg = argv[i];
//		if (arg == NULL)
//			continue;
//		if (arg[0] == '-' || arg[0] == '/')
//		{
//			switch (tolower(arg[1]))
//			{
//			case 'n':
//				recursion = false;
//				break;
//			case 'v':
//			 {
//				 const char *param = arg+2;
//				 if (*param == ':' || *param=='=') ++param;
//				 argv[i] = NULL;
//				 if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//					 param = argv[++i];
//					 argv[i] = NULL;
//				 }
//				 if ( param[0] == 0 )
//					 break;
//				 nifver.version = Niflib::ParseVersionString(string(param));
//				 nifver.userVersion = 0;
//				 nifver.userVersion = 0;
//			 }
//			 break;
//
//			case 'u':
//			 {
//				 const char *param = arg+2;
//				 if (*param == ':' || *param=='=') ++param;
//				 argv[i] = NULL;
//				 if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//					 param = argv[++i];
//					 argv[i] = NULL;
//				 }
//				 if ( param[0] == 0 )
//					 break;
//				 char *end;
//				 nifver.userVersion = strtol(param, &end, 0);
//				 nifver.userVersion2 = 0;
//			 }
//			 break;
//
//			case 'u2':
//			 {
//				 const char *param = arg+2;
//				 if (*param == ':' || *param=='=') ++param;
//				 argv[i] = NULL;
//				 if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//					 param = argv[++i];
//					 argv[i] = NULL;
//				 }
//				 if ( param[0] == 0 )
//					 break;
//				 char *end;
//				 nifver.userVersion2 = strtol(param, &end, 0);
//			 }
//			 break;
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
//					paths.push_back(string(param));
//				}
//				break;
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
//					paths.insert(paths.begin(), 1, string(param));
//				}
//				break;
//
//
//			default:
//				Log::Error("Unknown argument specified '%s'", arg);
//				return false;
//			}
//		}
//		else
//		{
//			paths.push_back(arg);
//		}
//	}
//#pragma endregion
//
//	if (paths.empty()){
//		HelpString(hkxcmd::htLong);
//		return false;
//	}
//
//	hkMallocAllocator baseMalloc;
//	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024) );
//	hkBaseSystem::init( memoryRouter, errorReport );
//
//	// search for projects and guess the layout
//	if (PathIsDirectory(paths[0].c_str()))
//	{
//		char searchPath[MAX_PATH], rootPath[MAX_PATH];
//		GetFullPathName(paths[0].c_str(), MAX_PATH, rootPath, NULL);
//		strcpy(searchPath, rootPath);
//		PathAddBackslash(searchPath);
//		strcat(searchPath, "*skeleton.hkx");
//
//		vector<string> files;
//		FindFiles(files, searchPath, recursion);
//		for (vector<string>::iterator itr = files.begin(); itr != files.end(); )
//		{
//			if (wildmatch("*\\skeleton.hkx", (*itr)))
//			{
//				++itr;
//			}
//			else
//			{
//				Log::Verbose("Ignoring '%s' due to inexact skeleton.hkx file match", (*itr).c_str());
//				itr = files.erase(itr);
//			}			
//		}
//
//
//		if (files.empty())
//		{
//			Log::Warn("No files found");
//			return false;
//		}
//
//		char outdir[MAX_PATH];
//		if (paths.size() > 1)
//		{
//			GetFullPathName(paths[1].c_str(), MAX_PATH, outdir, NULL);
//		}
//		else
//		{
//			strcpy(outdir, rootPath);
//		}
//
//		for (vector<string>::iterator itr = files.begin(); itr != files.end(); ++itr)
//		{
//			string projfile = (*itr).c_str();
//			ExportProject(projfile, rootPath, outdir, nifver, recursion);
//		}
//	}
//	else
//	{
//		string skelpath = (paths.size() >= 0) ? paths[0] : string();
//
//		if (skelpath.empty())
//		{
//			Log::Error("Skeleton file not specified");
//			HelpString(hkxcmd::htLong);
//		}
//		else if ( wildmatch("*project.hkx", skelpath) )
//		{
//			// handle specification of project by name
//			char rootPath[MAX_PATH];
//			GetFullPathName(skelpath.c_str(), MAX_PATH, rootPath, NULL);
//			PathRemoveFileSpec(rootPath);
//
//			if (paths.size() > 2)
//			{
//				Log::Error("Too many arguments specified");
//				HelpString(hkxcmd::htLong);
//			}
//			else
//			{
//				char outdir[MAX_PATH];
//				if (paths.size() >= 1){
//					GetFullPathName(paths[1].c_str(), MAX_PATH, outdir, NULL);
//				} else { 
//					strcpy(outdir, rootPath); 
//				}
//				ExportProject(skelpath, rootPath, outdir, nifver, recursion);
//			}
//		}
//		else
//		{
//			// handle specification of skeleton + animation + output
//			if ( !PathFileExists(skelpath.c_str()) )
//			{
//				Log::Error("Skeleton file not found at '%s'", skelpath.c_str());
//			}
//			else
//			{
//				// set relative path to current directory
//				char rootPath[MAX_PATH];
//				_getcwd(rootPath, MAX_PATH);
//
//				if (paths.size() > 3)
//				{
//					Log::Error("Too many arguments specified");
//					HelpString(hkxcmd::htLong);
//				}
//				else
//				{
//					bool norelativepath = true;
//					if (paths.size() == 1) // output files inplace
//					{
//						char animDir[MAX_PATH], tempdir[MAX_PATH];
//						strcpy(tempdir, skelpath.c_str());
//						PathRemoveFileSpec(tempdir);
//						PathCombine(animDir, tempdir, "..\animations");
//						PathAddBackslash(animDir);
//						ExportProject(skelpath, rootPath, rootPath, nifver, recursion);
//					}
//					else if (paths.size() == 2) // second path will be output
//					{
//						char outdir[MAX_PATH], tempdir[MAX_PATH];
//						strcpy(outdir, paths[1].c_str());
//						strcpy(tempdir, skelpath.c_str());
//						PathRemoveFileSpec(tempdir);
//						PathAddBackslash(tempdir);
//						PathCombine(rootPath,tempdir,"..\\animations");
//						GetFullPathName(rootPath, MAX_PATH, rootPath, NULL);
//						GetFullPathName(outdir, MAX_PATH, outdir, NULL);
//						ExportProject(skelpath, rootPath, outdir, nifver, recursion);
//					}
//					else // second path is animation, third is output
//					{
//						string animpath = paths[1];
//						if (PathIsDirectory(animpath.c_str()))
//						{
//							strcpy(rootPath, animpath.c_str());
//							animpath += string("\\*.hkx");
//							norelativepath = false;
//						}
//						vector<string> animfiles;
//						FindFiles(animfiles, animpath.c_str(), recursion);
//
//						if (animfiles.empty())
//						{
//							Log::Warn("No Animations found. Skipping '%s'", animpath.c_str());
//						}
//						else
//						{
//
//							char outdir[MAX_PATH];
//							if (paths.size() >= 2){
//								GetFullPathName(paths[2].c_str(), MAX_PATH, outdir, NULL);
//							} else { 
//								strcpy(outdir, rootPath); 
//							}
//
//							ExportAnimations(string(rootPath), skelpath, animfiles, outdir, nifver, norelativepath);
//						}
//
//					}
//				}
//			}
//		}
//	}
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
//REGISTER_COMMAND(ExportKF, HelpString, SafeExecuteCmd);
