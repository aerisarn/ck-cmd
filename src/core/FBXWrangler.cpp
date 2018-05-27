/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius
See the included LICENSE file
*/

#include <core/FBXWrangler.h>
#include <core/EulerAngles.h>
#include <core/MathHelper.h>

#include <commands/Geometry.h>

using namespace ckcmd::FBX;
using namespace  ckcmd::Geometry;

////extern ConfigurationManager Config;

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


class FBXBuilderVisitor : public RecursiveFieldVisitor<FBXBuilderVisitor> {
	const NifInfo& this_info;
	FbxScene& scene;
	deque<FbxNode*> build_stack;
	set<void*>& alreadyVisitedNodes;
	set<void*>& alreadyStartedNodes;
	map<void*, FbxNode*> built_nodes;
	map<NiSkinInstance*, NiTriBasedGeom*> skins;
	set<NiControllerManager*>& managers;
	NifFile& nif_file;
	FbxAnimStack* lAnimStack = NULL;

	FbxNode* AddGeometry(NiTriStrips& node) {
		const string& shapeName = node.GetName();

		if (node.GetData() == NULL) return FbxNode::Create(&scene, shapeName.c_str());

		const vector<Vector3>& verts = node.GetData()->GetVertices();
		const vector<Vector3>& norms = node.GetData()->GetNormals();

		vector<Triangle>& tris = vector<Triangle>(0);
		vector<TexCoord>& uvs = vector<TexCoord>(0);

		if (node.GetData()->IsSameType(NiTriStripsData::TYPE)) {
			NiTriStripsDataRef ref = DynamicCast<NiTriStripsData>(node.GetData());
			tris = triangulate(ref->GetPoints());
			if (!ref->GetUvSets().empty())
				uvs = ref->GetUvSets()[0];
		}

		return AddGeometry(shapeName, verts, norms, tris, uvs);
	}

	FbxNode* AddGeometry(NiTriShape& node) {

		const string& shapeName = node.GetName();

		if (node.GetData() == NULL) return FbxNode::Create(&scene, shapeName.c_str());

		const vector<Vector3>& verts = node.GetData()->GetVertices();
		const vector<Vector3>& norms = node.GetData()->GetNormals();

		vector<Triangle>& tris = vector<Triangle>(0);
		vector<TexCoord>& uvs = vector<TexCoord>(0);

		if (node.GetData()->IsSameType(NiTriShapeData::TYPE)) {
			NiTriShapeDataRef ref = DynamicCast<NiTriShapeData>(node.GetData());
			tris = ref->GetTriangles();
			if (!ref->GetUvSets().empty())
				uvs = ref->GetUvSets()[0];
		}

		if (verts.empty())
			return FbxNode::Create(&scene, shapeName.c_str());

		return AddGeometry(shapeName, verts, norms, tris, uvs);
	}

	FbxNode* AddGeometry(const string& shapeName, 
							const vector<Vector3>& verts,
							const vector<Vector3>& norms,
							const vector<Triangle>& tris,
							vector<TexCoord>& uvs) {

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

		m->InitControlPoints(verts.size());
		FbxVector4* points = m->GetControlPoints();

		for (int i = 0; i < m->GetControlPointsCount(); i++) {
			points[i] = FbxVector4(verts[i].x, verts[i].y, verts[i].z);
			if (normElement)
				normElement->GetDirectArray().Add(FbxVector4(norms[i].x, norms[i].y, norms[i].z));
			if (uvElement)
				uvElement->GetDirectArray().Add(FbxVector2(uvs[i].u, uvs[i].v));
		}

		if (!tris.empty()) {
			for (auto &t : tris) {
				m->BeginPolygon();
				m->AddPolygon(t.v1);
				m->AddPolygon(t.v2);
				m->AddPolygon(t.v3);
				m->EndPolygon();
			}
		}

		FbxNode* mNode = FbxNode::Create(&scene, shapeName.c_str());
		//parent->AddChild(mNode);
		//setTransform(&node, mNode);
		mNode->SetNodeAttribute(m);

		// Intended for Maya
		//mNode->LclScaling.Set(FbxDouble3(1, 1, 1));
		//mNode->LclRotation.Set(FbxDouble3(-90, 0, 0));
		//mNode->LclTranslation.Set(FbxDouble3(0, 120, 0));

		return mNode;
	}

	FbxNode* setNullTransform(FbxNode* node) {
		node->LclTranslation.Set(FbxDouble3(0.0,0.0,0.0));
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

	FbxNode* getBuiltNode(void* obj) {
		NiObject* node = (NiObject*)obj;
		if (node->IsSameType(NiNode::TYPE))
		{
			NiNodeRef noderef = DynamicCast<NiNode>(node);
			if (noderef != NULL && !noderef->GetName().empty()) {
				//by name:
				for (pair<void*, FbxNode*> pair : built_nodes) {
					NiNodeRef build = DynamicCast<NiNode>((NiObject*)pair.first);
					if (build != NULL && build->GetName() == noderef->GetName())
						return pair.second;
				}
			}
		}
		if (built_nodes.find(obj) != built_nodes.end())
			return built_nodes[obj];
		return NULL;
	}

	FbxNode* getBuiltNode(const string& name) {
		for (pair<void*, FbxNode*> nodes : built_nodes)
		{
			NiObject* node = (NiObject*)nodes.first;
			if (node->IsDerivedType(NiAVObject::TYPE))
			{
				NiAVObjectRef build = DynamicCast<NiAVObject>((NiObject*)nodes.first);
				if(build != NULL && 
					(build->GetName() == name || build->GetName() == name + (char)0 || build->GetName()+ (char)0 == name)) {
					return nodes.second;
				}
			}
		}
		return NULL;
	}

	void processSkins() {
		if (skins.empty())
			return;
		for (pair<NiSkinInstance*, NiTriBasedGeom*> skin : skins) {
			NiTriBasedGeomRef mesh = skin.second;
			
			NiSkinDataRef data = skin.first->GetData();
			NiSkinPartitionRef part = skin.first->GetSkinPartition();

			std::string shapeSkin = mesh->GetName() + "_skin";
			FbxSkin* fbx_skin = FbxSkin::Create(&scene, shapeSkin.c_str());
			int boneIndex = 0;
			for (NiNode* bone : skin.first->GetBones()) {
				FbxNode* jointNode = getBuiltNode(bone);
				if (jointNode) {
					std::string boneSkin = bone->GetName() + "_skin";
					FbxCluster* aCluster = FbxCluster::Create(&scene, boneSkin.c_str());

					aCluster->SetLink(jointNode);
					aCluster->SetLinkMode(FbxCluster::eTotalOne);

					BoneData& boneData = data->GetBoneList()[boneIndex];

					Vector3 translation = data->GetSkinTransform().translation;
					Quaternion rotation = data->GetSkinTransform().rotation.AsQuaternion();
					float scale = data->GetSkinTransform().scale;

					//FbxAMatrix matrix;
					//matrix.SetT(FbxDouble3(translation.x, translation.y, translation.z));
					//matrix.SetQ(FbxQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
					//matrix.SetS(FbxVector4(scale, scale, scale));

					aCluster->SetTransformLinkMatrix(jointNode->EvaluateGlobalTransform());

					for (BoneVertData& boneVertData : boneData.vertexWeights) {
						aCluster->AddControlPointIndex(boneVertData.index, boneVertData.weight);
					}
					fbx_skin->AddCluster(aCluster);

				}
				boneIndex++;
			}

			FbxMesh* shapeMesh = (FbxMesh*)built_nodes[&*mesh]->GetNodeAttribute();
			if (shapeMesh)
				shapeMesh->AddDeformer(fbx_skin);

		}
	}

	string getPalettedString(NiStringPaletteRef nipalette, unsigned int offset) {
		int find = nipalette->GetPalette().palette.find((char)0, offset);
		string ret = nipalette->GetPalette().palette.substr(
			offset,
			find - offset
		);
		return ret;
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
			FbxAnimCurve* lCurve_Trans_X = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			if (lCurve_Trans_X == NULL)
				lCurve_Trans_X = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			FbxAnimCurve* lCurve_Trans_Y = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			if (lCurve_Trans_Y == NULL)
				lCurve_Trans_Y = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			FbxAnimCurve* lCurve_Trans_Z = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			if (lCurve_Trans_Z == NULL)
				lCurve_Trans_Z = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

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
				lTime.SetTime(0, 0, key.time, 0, 0, 0, lTime.eFrames30);

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
			FbxAnimCurve* lCurve_Rot_X = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			if (lCurve_Rot_X == NULL)
				lCurve_Rot_X = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			FbxAnimCurve* lCurve_Rot_Y = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			if (lCurve_Rot_Y == NULL)
				lCurve_Rot_Y = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			FbxAnimCurve* lCurve_Rot_Z = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			if (lCurve_Rot_Z == NULL)
				lCurve_Rot_Z = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			Niflib::array<3, FbxAnimCurve* > curves;
			curves[0] = lCurve_Rot_X;
			curves[1] = lCurve_Rot_Y;
			curves[2] = lCurve_Rot_Z;

			for (int i = 0; i < 3; i++) {
				for (const Key<float>& key : rotations.at(i).keys) {

					FbxTime lTime;
					//key.time
					lTime.SetTime(0, 0, key.time, 0, 0, 0, lTime.eFrames30);

					curves[i]->KeyModifyBegin();
					int lKeyIndex = curves[i]->KeyAdd(lTime);
					curves[i]->KeySetValue(lKeyIndex, float(rad2deg(key.data)));
					curves[i]->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
					curves[i]->KeySetLeftDerivative(lKeyIndex, key.backward_tangent);
					curves[i]->KeySetRightDerivative(lKeyIndex, key.forward_tangent);
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
				lTime.SetTime(0, 0, key.time, 0, 0, 0, lTime.eFrames30);

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

	template<>
	void addTrack(NiTransformInterpolator& interpolator, FbxNode* node, FbxAnimLayer *lAnimLayer) {
		//here we have an initial transform to be applied and a set of keys
		FbxAMatrix local = node->EvaluateLocalTransform();
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
			//TODO: scale
			//data->getS
		}

	}

	void exportKFSequence(NiControllerSequenceRef ref) {
		FbxAnimLayer* layer = FbxAnimLayer::Create(&scene, ref->GetName().c_str());
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
			FbxNode* animatedNode = getBuiltNode(controlledBlockID);

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
		//if animation layer doesn't exist in the file, create it
		if (!lAnimStack)
			lAnimStack = FbxAnimStack::Create(&scene, "KF");

		for (NiControllerManager* obj : managers) {
			//Next Controller can be ignored because it should be referred by individual sequences
			for (NiControllerSequenceRef ref : obj->GetControllerSequences()) {
				exportKFSequence(ref);
			}
		}
	}

public:

	FBXBuilderVisitor(NifFile& nif, FbxNode& sceneNode, FbxScene& scene, const NifInfo& info) :
		RecursiveFieldVisitor(*this, info),
		nif_file(nif),
		alreadyVisitedNodes(set<void*>()),
		alreadyStartedNodes(set<void*>()),
		this_info(info),
		scene(scene),
		managers(set<NiControllerManager*>()),
		built_nodes(map<void*, FbxNode*>())
	{
		build_stack.push_front(&sceneNode);
		NiObjectRef rootNode = nif.GetRoot();
		rootNode->accept(*this, info);
		//Sort out skinning now
		processSkins();
		buildManagers();
	}

	FBXBuilderVisitor(NifFile& nif, FbxNode& sceneNode, FbxScene& scene, const NifInfo& info, map<void*, FbxNode*> nodeMap) :
		RecursiveFieldVisitor(*this, info),
		nif_file(nif),
		alreadyVisitedNodes(set<void*>()),
		alreadyStartedNodes(set<void*>()),
		this_info(info),
		scene(scene),
		managers(set<NiControllerManager*>()),
		built_nodes(nodeMap)
	{
		build_stack.push_front(&sceneNode);
		NiNodeRef rootNode = DynamicCast<NiNode>(nif.GetRoot());
		for (NiObjectRef child : rootNode->GetChildren())
			child->accept(*this, info);
		//Sort out skinning now
		processSkins();
		buildManagers();
	}

	map<void*, FbxNode*> getBuiltNodesMap() { return built_nodes; }

	virtual inline void start(NiObject& in, const NifInfo& info) {
		alreadyStartedNodes.insert(&in);
	}

	virtual inline void end(NiObject& in, const NifInfo& info) {
		if (alreadyVisitedNodes.find(&in) == alreadyVisitedNodes.end()) {
			build_stack.pop_front();
			alreadyVisitedNodes.insert(&in);
		}
		alreadyStartedNodes.erase(alreadyStartedNodes.find(&in));
	}

	//Always insert into the stack to be consistant
	template<class T>
	inline void visit_object(T& obj) {
		if (alreadyVisitedNodes.find(&obj) == alreadyVisitedNodes.end()) {
			void* ptr = &obj;
			FbxNode* parent = build_stack.front();
			FbxNode* current = getBuiltNode(ptr);
			if (current == NULL) {
				current = build(obj, parent);
				built_nodes[ptr] = current;
				parent->AddChild(current);
			}
			build_stack.push_front(current);
		}
	}

	//Tailored over oblivion right now
	template<>
	inline void visit_object(NiControllerManager& obj) {
		if (alreadyVisitedNodes.find(&obj) == alreadyVisitedNodes.end()) {
			managers.insert(&obj);
			alreadyVisitedNodes.insert(&obj);
		}
	}

	template<class T>
	inline void visit_compound(T& obj) {}

	template<class T>
	inline void visit_field(T& obj) {}

	template<class T>
	FbxNode* build(T& obj, FbxNode* parent) {
		NiObject* pobj = (NiObject*)&obj;
		if (pobj->IsDerivedType(NiAVObject::TYPE)) {
			NiAVObjectRef av = DynamicCast<NiAVObject>(pobj);
			FbxNode* node = FbxNode::Create(&scene, av->GetName().c_str());
			/*if (nif_file.isSkeletonOnly() && (av->GetName().find("NPC") == 0 || av->GetName().find("Bip") == 0)) {
				NiNodeRef nibone = DynamicCast<NiNode>(av);
				bool isTerminal = true;
				bool isRoot = true;
				for (NiAVObjectRef child : nibone->GetChildren()) {
					if (child->GetName().find("NPC") == 0 || child->GetName().find("Bip") == 0)
						isTerminal = false;
				}
				for (FbxNode* n : build_stack) {
					std::string name(n->GetName());
					if (name.find("NPC") == 0 || name.find("Bip") == 0)
						isRoot = false;
				}
				FbxSkeleton* skel = FbxSkeleton::Create(&scene, av->GetName().c_str());
				if (isRoot)
					skel->SetSkeletonType(FbxSkeleton::eRoot);
				else if (isTerminal)
					skel->SetSkeletonType(FbxSkeleton::eEffector);
				else
					skel->SetSkeletonType(FbxSkeleton::eLimbNode);

				node->SetNodeAttribute(skel);
			}*/
			//parent->AddChild(node);
			return setTransform(av, node);
		}
		return setNullTransform(
			FbxNode::Create(&scene, pobj->GetInternalType().GetTypeName().c_str())
		);
	}

	template<>
	FbxNode* build(NiTriShape& obj, FbxNode* parent) {
		//need to import the whole tree structure before skinning, so defer into a list
		if (obj.GetSkinInstance() != NULL)
			skins[&*(obj.GetSkinInstance())] = &obj;
		//TODO: handle materials
		return setTransform(&obj, AddGeometry(obj));
	}

	template<>
	FbxNode* build(NiTriStrips& obj, FbxNode* parent) {
		//need to import the whole tree structure before skinning, so defer into a list
		if (obj.GetSkinInstance() != NULL)
			skins[&*(obj.GetSkinInstance())] = &obj;
		//TODO: handle materials
		return setTransform(&obj, AddGeometry(obj));
	}

};

void FBXWrangler::AddNif(NifFile& nif) {
	//will cause problems due to the skinned meshes having flattened hierarchy, use an appropriate method AddExternalSkinnedMeshes
	//if (!nif.hasExternalSkin())
	FBXBuilderVisitor(nif, *scene->GetRootNode(), *scene, nif.GetInfo());
}

void FBXWrangler::AddExternalSkinnedMeshes(NifFile& skeleton, set<NifFile*> meshes) {
	map<void*, FbxNode*> skeleton_nodes;
	if (skeleton.isSkeletonOnly())
		skeleton_nodes = FBXBuilderVisitor(skeleton, *scene->GetRootNode(), *scene, skeleton.GetInfo()).getBuiltNodesMap();
	else
		return;

	for (auto& mesh : meshes)
		if (mesh->hasExternalSkin())
			FBXBuilderVisitor(*mesh, *scene->GetRootNode(), *scene, mesh->GetInfo(), skeleton_nodes);
}

//void FBXWrangler::AddSkeleton(NifFile* nif, bool onlyNonSkeleton) {
//	auto root = nif->FindBlockByName<NiNode>(Config["Anim/SkeletonRootName"]);
//	auto com = nif->FindBlockByName<NiNode>("COM");
//	if (!com)
//		com = nif->FindBlockByName<NiNode>("NPC COM [COM ]");
//	if (!com)
//		com = nif->FindBlockByName<NiNode>("Bip01 NonAccum");
//
//	// Likely a NIF with non-hierarchical nodes
//	if (!com)
//		com = nif->GetRootNode();
//	if (!com)
//		return;
//
//	if (comName.empty())
//		comName = com->GetName();
//
//	// Check if skeleton already exists
//	std::string skelName = "NifSkeleton";
//	FbxNode* skelNode = scene->GetRootNode()->FindChild(skelName.c_str());
//	if (skelNode && onlyNonSkeleton) {
//		// Add non-skeleton nodes to the existing skeleton
//		FbxNode* comNode = skelNode->FindChild(comName.c_str());
//		if (comNode) {
//			std::vector<NiNode*> boneNodes = nif->GetChildren<NiNode>(com);
//			for (auto &b : boneNodes)
//				comNode->AddChild(AddLimb(nif, b));
//		}
//	}
//	else if (!skelNode) {
//		// Create new skeleton
//		FbxSkeleton* skel = FbxSkeleton::Create(scene, skelName.c_str());
//		skel->SetSkeletonType(FbxSkeleton::eRoot);
//
//		skelNode = FbxNode::Create(scene, skelName.c_str());
//		skelNode->SetNodeAttribute(skel);
//		skelNode->LclTranslation.Set(FbxDouble3(0.0, 0.0, 0.0));
//		//skelNode->LclRotation.Set(FbxDouble3(0.0, 0.0, 0.0));
//		//skelNode->SetRotationOrder(FbxNode::eSourcePivot, eEulerZYX);
//
//		FbxNode* parentNode = skelNode;
//		if (root) {
//			FbxSkeleton* rootBone = FbxSkeleton::Create(scene, root->GetName().c_str());
//			rootBone->SetSkeletonType(FbxSkeleton::eLimbNode);
//			rootBone->Size.Set(1.0);
//
//			FbxNode* rootNode = FbxNode::Create(scene, root->GetName().c_str());
//			rootNode->SetNodeAttribute(rootBone);
//
//			rootNode->LclTranslation.Set(FbxDouble3(root->transform.translation.x, root->transform.translation.y, root->transform.translation.z));
//
//			float rx, ry, rz;
//			root->transform.ToEulerDegrees(rx, ry, rz);
//			rootNode->LclRotation.Set(FbxDouble3(rx, ry, rz));
//			//rootNode->SetRotationOrder(FbxNode::eSourcePivot, eEulerZYX);
//
//			// Add root as first node
//			parentNode->AddChild(rootNode);
//			parentNode = rootNode;
//		}
//
//		if (com) {
//			FbxSkeleton* comBone = FbxSkeleton::Create(scene, com->GetName().c_str());
//			comBone->SetSkeletonType(FbxSkeleton::eLimbNode);
//			comBone->Size.Set(1.0);
//
//			FbxNode* comNode = FbxNode::Create(scene, com->GetName().c_str());
//			comNode->SetNodeAttribute(comBone);
//
//			comNode->LclTranslation.Set(FbxDouble3(com->transform.translation.y, com->transform.translation.z, com->transform.translation.x));
//
//			float rx, ry, rz;
//			com->transform.ToEulerDegrees(rx, ry, rz);
//			comNode->LclRotation.Set(FbxDouble3(rx, ry, rz));
//			//comNode->SetRotationOrder(FbxNode::eSourcePivot, eEulerZYX);
//
//			// Add COM as child of root
//			parentNode->AddChild(comNode);
//			parentNode = comNode;
//		}
//
//		std::vector<NiNode*> boneNodes = nif->GetChildren<NiNode>(com);
//		for (auto bn : boneNodes)
//			parentNode->AddChild(AddLimb(nif, bn));
//
//		scene->GetRootNode()->AddChild(skelNode);
//	}
//}
//
//FbxNode* FBXWrangler::AddLimb(NifFile* nif, NiNode* nifBone) {
//	FbxNode* node = scene->GetRootNode()->FindChild(nifBone->GetName().c_str());
//	if (!node) {
//		// Add new bone
//		FbxSkeleton* bone = FbxSkeleton::Create(scene, nifBone->GetName().c_str());
//		bone->SetSkeletonType(FbxSkeleton::eLimbNode);
//		bone->Size.Set(1.0f);
//
//		node = FbxNode::Create(scene, nifBone->GetName().c_str());
//		node->SetNodeAttribute(bone);
//
//		Vector3 translation = nifBone->transform.translation;
//		node->LclTranslation.Set(FbxDouble3(translation.x, translation.y, translation.z));
//
//		float rx, ry, rz;
//		nifBone->transform.ToEulerDegrees(rx, ry, rz);
//		node->LclRotation.Set(FbxDouble3(rx, ry, rz));
//		//myNode->SetRotationOrder(FbxNode::eSourcePivot, eEulerZYX);
//	}
//	else {
//		// Bone already exists, but go through children and return nullptr
//		AddLimbChildren(node, nif, nifBone);
//		return nullptr;
//	}
//
//	AddLimbChildren(node, nif, nifBone);
//	return node;
//}
//
//void FBXWrangler::AddLimbChildren(FbxNode* node, NifFile* nif, NiNode* nifBone) {
//	std::vector<NiNode*> boneNodes = nif->GetChildren<NiNode>(nifBone);
//	for (auto &b : boneNodes)
//		node->AddChild(AddLimb(nif, b));
//}
//
//void FBXWrangler::AddNif(NifFile* nif, const std::string& shapeName) {
//	AddSkeleton(nif, true);
//
//	for (auto &s : nif->GetShapeNames()) {
//		if (s == shapeName || shapeName.empty()) {
//			std::vector<Triangle> tris;
//			auto shape = nif->FindBlockByName<NiShape>(s);
//			if (shape && shape->GetTriangles(tris)) {
//				const std::vector<Vector3>* verts = nif->GetRawVertsForShape(s);
//				const std::vector<Vector3>* norms = nif->GetNormalsForShape(s, false);
//				const std::vector<Vector2>* uvs = nif->GetUvsForShape(s);
//				AddGeometry(s, verts, norms, &tris, uvs);
//			}
//		}
//	}
//}
//
//void FBXWrangler::AddSkinning(AnimInfo* anim, const std::string& shapeName) {
//	FbxNode* rootNode = scene->GetRootNode();
//	FbxNode* skelNode = rootNode->FindChild("NifSkeleton");
//	if (!skelNode)
//		return;
//
//	for (auto &animSkin : anim->shapeSkinning) {
//		if (animSkin.first != shapeName && !shapeName.empty())
//			continue;
//
//		std::string shape = animSkin.first;
//		FbxNode* shapeNode = rootNode->FindChild(shape.c_str());
//		if (!shapeNode)
//			continue;
//
//		std::string shapeSkin = shape + "_sk";
//		FbxSkin* skin = FbxSkin::Create(scene, shapeSkin.c_str());
//
//		for (auto &bone : anim->shapeBones[shape]) {
//			FbxNode* jointNode = skelNode->FindChild(bone.c_str());
//			if (jointNode) {
//				std::string boneSkin = bone + "_sk";
//				FbxCluster* aCluster = FbxCluster::Create(scene, boneSkin.c_str());
//				aCluster->SetLink(jointNode);
//				aCluster->SetLinkMode(FbxCluster::eTotalOne);
//
//				auto weights = anim->GetWeightsPtr(shape, bone);
//				if (weights) {
//					for (auto &vw : *weights)
//						aCluster->AddControlPointIndex(vw.first, vw.second);
//				}
//
//				skin->AddCluster(aCluster);
//			}
//		}
//
//		FbxMesh* shapeMesh = (FbxMesh*)shapeNode->GetNodeAttribute();
//		if (shapeMesh)
//			shapeMesh->AddDeformer(skin);
//	}
//}
//
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
//
//bool FBXWrangler::ImportScene(const std::string& fileName, const FBXImportOptions& options) {
//	FbxIOSettings* ios = sdkManager->GetIOSettings();
//	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
//	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
//	ios->SetBoolProp(IMP_FBX_LINK, false);
//	ios->SetBoolProp(IMP_FBX_SHAPE, true);
//	ios->SetBoolProp(IMP_FBX_GOBO, true);
//	ios->SetBoolProp(IMP_FBX_ANIMATION, true);
//	ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
//
//	FbxImporter* iImporter = FbxImporter::Create(sdkManager, "");
//	if (!iImporter->Initialize(fileName.c_str(), -1, ios)) {
//		iImporter->Destroy();
//		return false;
//	}
//
//	NewScene();
//
//	bool status = iImporter->Import(scene);
//	iImporter->Destroy();
//
//	if (!status)
//		return false;
//
//	return LoadMeshes(options);
//}
//
//bool FBXWrangler::LoadMeshes(const FBXImportOptions& options) {
//	if (!scene)
//		return false;
//
//	std::function<void(FbxNode*)> loadNodeChildren = [&](FbxNode* root) {
//		for (int i = 0; i < root->GetChildCount(); i++) {
//			FbxNode* child = root->GetChild(i);
//
//			if (child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
//				FBXShape shape;
//				FbxMesh* m = (FbxMesh*)child->GetNodeAttribute();
//
//				if (!m->IsTriangleMesh()) {
//					FbxGeometryConverter converter(sdkManager);
//					m = (FbxMesh*)converter.Triangulate((FbxNodeAttribute*)m, true);
//				}
//
//				FbxGeometryElementUV* uv = m->GetElementUV(0);
//				FbxGeometryElementNormal* normal = m->GetElementNormal(0);
//
//				shape.name = child->GetName();
//				int numVerts = m->GetControlPointsCount();
//				int numTris = m->GetPolygonCount();
//
//				for (int v = 0; v < numVerts; v++) {
//					FbxVector4 vert = m->GetControlPointAt(v);
//					shape.verts.emplace_back((float)vert.mData[0], (float)vert.mData[1], (float)vert.mData[2]);
//					if (uv && uv->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
//						int uIndex = v;
//						if (uv->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
//							uIndex = uv->GetIndexArray().GetAt(v);
//
//						shape.uvs.emplace_back((float)uv->GetDirectArray().GetAt(uIndex).mData[0],
//							(float)uv->GetDirectArray().GetAt(uIndex).mData[1]);
//					}
//
//					if (normal && normal->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
//						shape.normals.emplace_back((float)normal->GetDirectArray().GetAt(v).mData[0],
//							(float)normal->GetDirectArray().GetAt(v).mData[1],
//							(float)normal->GetDirectArray().GetAt(v).mData[2]);
//					}
//				}
//
//				const char* uvName = nullptr;
//				if (uv) {
//					uvName = uv->GetName();
//					shape.uvs.resize(numVerts);
//				}
//
//				for (int t = 0; t < numTris; t++) {
//					if (m->GetPolygonSize(t) != 3)
//						continue;
//
//					int p1 = m->GetPolygonVertex(t, 0);
//					int p2 = m->GetPolygonVertex(t, 1);
//					int p3 = m->GetPolygonVertex(t, 2);
//					shape.tris.emplace_back(p1, p2, p3);
//
//					if (uv && uv->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
//						FbxVector2 v_uv;
//						bool isUnmapped;
//
//						if (m->GetPolygonVertexUV(t, 0, uvName, v_uv, isUnmapped))
//							shape.uvs[p1] = Vector2(v_uv.mData[0], v_uv.mData[1]);
//
//						if (m->GetPolygonVertexUV(t, 1, uvName, v_uv, isUnmapped))
//							shape.uvs[p2] = Vector2(v_uv.mData[0], v_uv.mData[1]);
//
//						if (m->GetPolygonVertexUV(t, 2, uvName, v_uv, isUnmapped))
//							shape.uvs[p3] = Vector2(v_uv.mData[0], v_uv.mData[1]);
//					}
//				}
//
//				for (int iSkin = 0; iSkin < m->GetDeformerCount(FbxDeformer::eSkin); iSkin++) {
//					FbxSkin* skin = (FbxSkin*)m->GetDeformer(iSkin, FbxDeformer::eSkin);
//
//					for (int iCluster = 0; iCluster < skin->GetClusterCount(); iCluster++) {
//						FbxCluster* cluster = skin->GetCluster(iCluster);
//						if (!cluster->GetLink())
//							continue;
//
//						std::string bone = cluster->GetLink()->GetName();
//						shape.boneNames.insert(bone);
//						for (int iPoint = 0; iPoint < cluster->GetControlPointIndicesCount(); iPoint++) {
//							int v = cluster->GetControlPointIndices()[iPoint];
//							float w = cluster->GetControlPointWeights()[iPoint];
//							shape.boneSkin[bone].SetWeight(v, w);
//						}
//					}
//				}
//
//				if (options.InvertU)
//					for (auto &u : shape.uvs)
//						u.u = 1.0f - u.u;
//
//				if (options.InvertV)
//					for (auto &v : shape.uvs)
//						v.v = 1.0f - v.v;
//
//				shapes[shape.name] = shape;
//			}
//
//			loadNodeChildren(child);
//		}
//	};
//
//	FbxNode* root = scene->GetRootNode();
//	loadNodeChildren(root);
//
//	return true;
//}
