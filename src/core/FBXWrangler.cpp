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

using namespace ckcmd::FBX;
using namespace  ckcmd::Geometry;
using namespace ckcmd::nifscan;

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

	FbxNode* getBuiltNode(const string& palette, int offset) {
		string name = getPalettedString(palette, offset);
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

	string getPalettedString(string palette_string, unsigned int offset) {
		int find = palette_string.find((char)0, offset);
		string ret = palette_string.substr(
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
			FbxNode* animatedNode = getBuiltNode(controlledBlockID, block.priority);

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
		//if (!lAnimStack)
		//	lAnimStack = FbxAnimStack::Create(&scene, "KF");

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

	//scene = FbxScene::Create(sdkManager, "ckcmd");

	//NewScene();

	if (scene)
		CloseScene();

	scene = FbxScene::Create(sdkManager, "ckcmd");

	bool status = iImporter->Import(scene);

	//FbxAxisSystem maxSystem(FbxAxisSystem::EUpVector::eZAxis, (FbxAxisSystem::EFrontVector) - 2, FbxAxisSystem::ECoordSystem::eRightHanded);
	//FbxAxisSystem::Max.ConvertScene(scene);
	//FbxSystemUnit::m.ConvertScene(scene);

	if (!status) {
		FbxStatus ist = iImporter->GetStatus();
		iImporter->Destroy();
		return false;
	}
	iImporter->Destroy();

	//FbxAxisSystem maxSystem(FbxAxisSystem::EUpVector::eZAxis, (FbxAxisSystem::EFrontVector) - 2, FbxAxisSystem::ECoordSystem::eRightHanded);
	//scene->GetGlobalSettings().SetAxisSystem(maxSystem);
	//scene->GetRootNode()->LclScaling.Set(FbxDouble3(1.0, 1.0, 1.0));


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

void setAvTransform(FbxNode* node, NiAVObject* av, bool rotation_only = false) {
	FbxDouble3 trans = node->LclTranslation.Get();
	if (!rotation_only)
		av->SetTranslation(Vector3(trans[0], trans[1], trans[2]));
	FbxDouble3 euler_angles = node->LclRotation.Get();
	EulerAngles ea = Eul_(deg2rad(euler_angles[0]), deg2rad(euler_angles[1]), deg2rad(euler_angles[2]), EulOrdXYZs);
	HMatrix out;
	Eul_ToHMatrix(ea, out);
	av->SetRotation(
		Matrix33(
			out[0][0], out[0][1], out[0][2],
			out[1][0], out[1][1], out[1][2],
			out[2][0], out[2][1], out[2][2]
		)
	);
	FbxDouble3 scaling = node->LclScaling.Get();
	//if (scaling[0] == scaling[1] && scaling[1] == scaling[2])
	//	av->SetScale(scaling[0]);
	//TODO: handle scaling
	//av->SetScale(scaling[0]);
}

NiTriShapeRef FBXWrangler::importShape(FbxNode* child, const FBXImportOptions& options) {
	NiTriShapeRef out = new NiTriShape();
	NiTriShapeDataRef data = new NiTriShapeData();
	FbxMesh* m = (FbxMesh*)child->GetNodeAttribute();

	if (!m->IsTriangleMesh()) {
		FbxGeometryConverter converter(sdkManager);
		m = (FbxMesh*)converter.Triangulate((FbxNodeAttribute*)m, true);
	}

	FbxGeometryElementUV* uv = m->GetElementUV(0);
	FbxGeometryElementNormal* normal = m->GetElementNormal(0);
	FbxGeometryElementVertexColor* vc = m->GetElementVertexColor(0);

	out->SetName(string(child->GetName()));
	int numVerts = m->GetControlPointsCount();
	int numTris = m->GetPolygonCount();

	vector<Vector3> verts;
	vector<Vector3> normals;
	vector<Color4 > vcs;

	if (normal == NULL) {
		Log::Info("Warning: cannot find normals, I'll recalculate them for %s", child->GetName());
	}
	vector<TexCoord> uvs;
	for (int v = 0; v < numVerts; v++) {
		FbxVector4 vert = m->GetControlPointAt(v);

		if (uv->GetMappingMode() != FbxGeometryElement::eByControlPoint &&
			uv->GetMappingMode() != FbxGeometryElement::eByPolygonVertex) {
			FbxGeometryElement::EMappingMode m = uv->GetMappingMode();
			cout << "lol" << endl;
		}
		
		verts.emplace_back((float)vert.mData[0], (float)vert.mData[1], (float)vert.mData[2]);
		if (uv && uv->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
			int uIndex = v;
			if (uv->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				uIndex = uv->GetIndexArray().GetAt(v);

			uvs.emplace_back((float)uv->GetDirectArray().GetAt(uIndex).mData[0],
				(float)uv->GetDirectArray().GetAt(uIndex).mData[1]);
		}

		if (normal && normal->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
			normals.emplace_back((float)normal->GetDirectArray().GetAt(v).mData[0],
				(float)normal->GetDirectArray().GetAt(v).mData[1],
				(float)normal->GetDirectArray().GetAt(v).mData[2]);
		}

		if (vc && vc->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
			vcs.emplace_back(
				Color4
				(
					(float)vc->GetDirectArray().GetAt(v).mRed,
					(float)vc->GetDirectArray().GetAt(v).mGreen,
					(float)vc->GetDirectArray().GetAt(v).mBlue,
					(float)vc->GetDirectArray().GetAt(v).mAlpha
				)
			);
		}
	}
	if (verts.size() > 0) {
		data->SetHasVertices(true);
		data->SetVertices(verts);
	}
	if (vcs.size()) 
	{
		data->SetHasVertexColors(true);
		data->SetVertexColors(vcs);
	}

	const char* uvName = nullptr;
	if (uv) {
		uvName = uv->GetName();
		uvs.resize(numVerts);
	}

	vector<Triangle> tris;

	for (int t = 0; t < numTris; t++) {
		if (m->GetPolygonSize(t) != 3)
			continue;

		int p1 = m->GetPolygonVertex(t, 0);
		int p2 = m->GetPolygonVertex(t, 1);
		int p3 = m->GetPolygonVertex(t, 2);
		tris.emplace_back(p1, p2, p3);

		if (uv && uv->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			FbxVector2 v_uv;
			bool isUnmapped;

			if (m->GetPolygonVertexUV(t, 0, uvName, v_uv, isUnmapped))
				uvs[p1] = TexCoord(v_uv.mData[0], v_uv.mData[1]);

			if (m->GetPolygonVertexUV(t, 1, uvName, v_uv, isUnmapped))
				uvs[p2] = TexCoord(v_uv.mData[0], v_uv.mData[1]);

			if (m->GetPolygonVertexUV(t, 2, uvName, v_uv, isUnmapped))
				uvs[p3] = TexCoord(v_uv.mData[0], v_uv.mData[1]);
		}
	}
	if (tris.size()) {
		data->SetHasTriangles(true);
		data->SetNumTriangles(tris.size());
		data->SetNumTrianglePoints(tris.size()*3);
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

	for (int iSkin = 0; iSkin < m->GetDeformerCount(FbxDeformer::eSkin); iSkin++) {
		FbxSkin* skin = (FbxSkin*)m->GetDeformer(iSkin, FbxDeformer::eSkin);

		for (int iCluster = 0; iCluster < skin->GetClusterCount(); iCluster++) {
			FbxCluster* cluster = skin->GetCluster(iCluster);
			if (!cluster->GetLink())
				continue;

			std::string bone = cluster->GetLink()->GetName();
			//shape.boneNames.insert(bone);
			for (int iPoint = 0; iPoint < cluster->GetControlPointIndicesCount(); iPoint++) {
				int v = cluster->GetControlPointIndices()[iPoint];
				float w = cluster->GetControlPointWeights()[iPoint];
				//shape.boneSkin[bone].SetWeight(v, w);
			}
		}
	}

	if (options.InvertU)
		for (auto &u : uvs)
			u.u = 1.0f - u.u;

	if (options.InvertV)
		for (auto &v : uvs)
			v.v = 1.0f - v.v;

	if (uvs.size() > 0) {
		data->SetHasUv(true);
		data->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV));
		data->SetUvSets({ uvs });
	}

	data->SetConsistencyFlags(CT_STATIC);

	if (verts.size() != 0 && tris.size() != 0 && uvs.size() != 0) {
		Vector3 COM;
		TriGeometryContext g(verts, COM, tris, uvs, normals);
		data->SetHasNormals(1);
		//recalculate
		data->SetNormals(g.normals);
		data->SetTangents(g.tangents);
		data->SetBitangents(g.bitangents);
		if (verts.size() != g.normals.size() || verts.size() != g.tangents.size() || verts.size() != g.bitangents.size())
			throw runtime_error("Geometry mismatch!");
		data->SetBsVectorFlags(static_cast<BSVectorFlags>(data->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
	}

	BSLightingShaderProperty* shader = new BSLightingShaderProperty();
	if (data->GetHasVertexColors() == false)
		shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2() & ~SLSF2_VERTEX_COLORS));

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

	shader->SetTextureSet(textures);

	out->SetFlags(524302);
	out->SetShaderProperty(shader);
	out->SetData(StaticCast<NiGeometryData>(data));
	

	return out;
}

void addTranslationKeys(NiTransformInterpolator* interpolator, FbxNode* node, FbxAnimCurve* curveX, FbxAnimCurve* curveY, FbxAnimCurve* curveZ, double time_offset) {
	map<double, int> timeMapX;
	map<double, int> timeMapY;
	map<double, int> timeMapZ;

	FbxDouble3 position = node->LclTranslation.Get();

	set<double> times;

	for (int i = 0; i < curveX->KeyGetCount(); i++)
	{
		FbxAnimCurveKey& key = curveX->KeyGet(i);
		timeMapX[key.GetTime().GetSecondDouble()] = i;
		times.insert(key.GetTime().GetSecondDouble());
	}
	for (int i = 0; i < curveY->KeyGetCount(); i++)
	{
		FbxAnimCurveKey& key = curveY->KeyGet(i);
		timeMapY[key.GetTime().GetSecondDouble()] = i;
		times.insert(key.GetTime().GetSecondDouble());
	}
	for (int i = 0; i < curveX->KeyGetCount(); i++)
	{
		FbxAnimCurveKey& key = curveZ->KeyGet(i);
		timeMapZ[key.GetTime().GetSecondDouble()] = i;
		times.insert(key.GetTime().GetSecondDouble());
	}
	
	if (times.size() > 0)
	{
		NiTransformDataRef data = interpolator->GetData();
		if (data == NULL) data = new NiTransformData();
		KeyGroup<Vector3 > tkeys = data->GetTranslations();
		vector<Key<Vector3 > > keyvalues = tkeys.keys;
		for (const auto& time : times) {
			float x = 0.0;
			float y = 0.0;
			float z = 0.0;
			if (timeMapX.find(time) != timeMapX.end())
			{
				x = curveX->KeyGet(timeMapX[time]).GetValue();
			}
			else {
				if (curveX != NULL)
					x = curveX->Evaluate(time);
				else
					x = position[0];
			}
			if (timeMapY.find(time) != timeMapY.end())
			{
				y = curveX->KeyGet(timeMapY[time]).GetValue();
			}
			else
			{
				if (curveY != NULL)
					y = curveY->Evaluate(time);
				else
					y = position[1];
			}
			if (timeMapZ.find(time) != timeMapZ.end())
			{
				z = curveX->KeyGet(timeMapZ[time]).GetValue();
			}
			else
			{
				if (curveZ != NULL)
					z = curveZ->Evaluate(time);
				else
					z = position[2];
			}

			Key<Vector3 > temp;
			temp.data = Vector3(x, y, z);
			temp.time = time - time_offset;
			keyvalues.push_back(temp);
		}
		tkeys.numKeys = keyvalues.size();
		tkeys.keys = keyvalues;
		//TODO
		tkeys.interpolation = LINEAR_KEY;
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
};

void addRotationKeys(NiTransformInterpolator* interpolator, FbxAnimCurve* curveI, FbxAnimCurve* curveJ, FbxAnimCurve* curveK, double time_offset) {
	//this is simpler because curves can be evaluated one at a time
	NiTransformDataRef data = interpolator->GetData();
	if (data == NULL) data = new NiTransformData();
	Niflib::array<3, KeyGroup<float > > tkeys = data->GetXyzRotations();	
	int IkeySize = curveI->KeyGetCount();
	if (IkeySize > 0) {
		KeyGroup<float>& keys = tkeys[0];
		for (int i = 0; i < IkeySize; i++) {
			FbxAnimCurveKey fbx_key = curveI->KeyGet(i);
			Key<float> new_key;
			new_key.time = fbx_key.GetTime().GetSecondDouble() - time_offset;
			new_key.data = deg2rad(fbx_key.GetValue());
			new_key.forward_tangent = deg2rad(curveI->KeyGetRightTangentVelocity(i));
			new_key.backward_tangent = deg2rad(curveI->KeyGetLeftTangentVelocity(i));
			keys.keys.push_back(new_key);
		}
		keys.numKeys = keys.keys.size();
		keys.interpolation = QUADRATIC_KEY;
	}
	int JkeySize = curveJ->KeyGetCount();
	if (JkeySize > 0) {
		KeyGroup<float>& keys = tkeys[1];
		for (int i = 0; i < JkeySize; i++) {
			FbxAnimCurveKey fbx_key = curveJ->KeyGet(i);
			Key<float> new_key;
			new_key.time = fbx_key.GetTime().GetSecondDouble() - time_offset;
			new_key.data = deg2rad(fbx_key.GetValue());
			new_key.forward_tangent = deg2rad(curveJ->KeyGetRightTangentVelocity(i));
			new_key.backward_tangent = deg2rad(curveJ->KeyGetLeftTangentVelocity(i));
			keys.keys.push_back(new_key);
		}
		keys.numKeys = keys.keys.size();
		keys.interpolation = QUADRATIC_KEY;
	}
	int KkeySize = curveK->KeyGetCount();
	if (KkeySize > 0) {
		KeyGroup<float>& keys = tkeys[2];
		for (int i = 0; i < KkeySize; i++) {
			FbxAnimCurveKey fbx_key = curveK->KeyGet(i);
			Key<float> new_key;
			new_key.time = fbx_key.GetTime().GetSecondDouble() - time_offset;
			new_key.data = deg2rad(fbx_key.GetValue());
			new_key.forward_tangent = deg2rad(curveK->KeyGetRightTangentVelocity(i));
			new_key.backward_tangent = deg2rad(curveK->KeyGetLeftTangentVelocity(i));
			keys.keys.push_back(new_key);
		}
		keys.numKeys = keys.keys.size();
		keys.interpolation = QUADRATIC_KEY;
	}
	if (IkeySize > 0 || JkeySize > 0 || KkeySize > 0) {
		Accessor<NiTransformData> fix_rot(data);
		data->SetXyzRotations(tkeys);
		data->SetRotationType(XYZ_ROTATION_KEY);
		interpolator->SetData(data);
	}
}

double FBXWrangler::convert(FbxAnimLayer* pAnimLayer, NiControllerSequenceRef sequence, set<NiObjectRef>& targets, NiControllerManagerRef manager, NiMultiTargetTransformControllerRef multiController, string accum_root_name, double last_start)
{
	// Display general curves.
	vector<ControlledBlock > blocks = sequence->GetControlledBlocks();
	for (const auto& pair : conversion_Map)
	{
		FbxNode* pNode = pair.first;
		FbxAnimCurve* lXAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		FbxAnimCurve* lYAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		FbxAnimCurve* lZAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		FbxAnimCurve* lIAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		FbxAnimCurve* lJAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		FbxAnimCurve* lKAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

		if (lXAnimCurve != NULL || lYAnimCurve != NULL || lZAnimCurve != NULL ||
			lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL) 
		{

			NiObjectRef target = conversion_Map[pNode];
			targets.insert(target);

			NiTransformInterpolatorRef interpolator = new NiTransformInterpolator();

			if (lXAnimCurve != NULL || lYAnimCurve != NULL || lZAnimCurve != NULL) {
				addTranslationKeys(interpolator, pNode, lXAnimCurve, lYAnimCurve, lZAnimCurve, last_start);
			}

			if (lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL) {
				addRotationKeys(interpolator, lIAnimCurve, lJAnimCurve, lKAnimCurve, last_start);
			}

			NiTransformDataRef data = interpolator->GetData();
			KeyGroup<float> scales;
			scales.numKeys = 0;
			scales.keys = {};
			data->SetScales(scales);

			//interpolator->SetData(new NiTransformData());

			ControlledBlock block;
			block.interpolator = interpolator;
			block.nodeName = DynamicCast<NiAVObject>(conversion_Map[pNode])->GetName();
			block.controllerType = "NiTransformController";
			block.controller = multiController;
		
			blocks.push_back(block);

			vector<FbxTimeSpan> spans(6);
		
			if (lXAnimCurve != NULL)
				lXAnimCurve->GetTimeInterval(spans[0]);
			if (lYAnimCurve != NULL)
				lYAnimCurve->GetTimeInterval(spans[1]);
			if (lZAnimCurve != NULL)
				lZAnimCurve->GetTimeInterval(spans[2]);
			if (lIAnimCurve != NULL)
				lIAnimCurve->GetTimeInterval(spans[3]);
			if (lJAnimCurve != NULL)
				lJAnimCurve->GetTimeInterval(spans[4]);
			if (lKAnimCurve != NULL)
				lKAnimCurve->GetTimeInterval(spans[5]);
			
			double start = 1e10;
			double end = -1e10;

			for (const auto& span : spans) {
				double span_start = span.GetStart().GetSecondDouble();
				double span_stop = span.GetStop().GetSecondDouble();
				if (span_start < start)
					start = span_start;
				if (span_stop > end)
					end = span_stop;
			}

			sequence->SetControlledBlocks(blocks);

			sequence->SetStartTime(0.0);
			sequence->SetStopTime(end-start);
			sequence->SetManager(manager);
			sequence->SetAccumRootName(accum_root_name);

			NiTextKeyExtraDataRef extra_data = new NiTextKeyExtraData();
			extra_data->SetName(string(""));
			Key<IndexString> start_key;
			start_key.time = 0;
			start_key.data = "start";

			Key<IndexString> end_key;
			end_key.time = end - last_start;
			end_key.data = "end";

			extra_data->SetTextKeys({ start_key,end_key });
			extra_data->SetNextExtraData(NULL);

			sequence->SetTextKeys(extra_data);

			sequence->SetFrequency(1.0);
			sequence->SetCycleType(CYCLE_CLAMP);

		}

		

		//cannot support the rest right now
#if 0
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        TX\n");
	//	}
	//	
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        TY\n");
	//	}
	//	
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        TZ\n");
	//	}
	//	
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        RX\n");
	//	}
	//	
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        RY\n");
	//	}
	//	lAnimCurve = ;
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        RZ\n");
	//	}
	//	lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        SX\n");
	//	}
	//	lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        SY\n");
	//	}
	//	lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        SZ\n");
	//	}
	//
	//// Display curves specific to a light or marker.
	//FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	//if (lNodeAttribute)
	//{
	//	lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_RED);
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        Red\n");
	//	}
	//	lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_GREEN);
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        Green\n");
	//	}
	//	lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_BLUE);
	//	if (lAnimCurve)
	//	{
	//		FBXSDK_printf("        Blue\n");
	//	}
	//	// Display curves specific to a light.
	//	FbxLight* light = pNode->GetLight();
	//	if (light)
	//	{
	//		lAnimCurve = light->Intensity.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Intensity\n");
	//		}
	//		lAnimCurve = light->OuterAngle.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Outer Angle\n");
	//		}
	//		lAnimCurve = light->Fog.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Fog\n");
	//		}
	//	}
	//	// Display curves specific to a camera.
	//	FbxCamera* camera = pNode->GetCamera();
	//	if (camera)
	//	{
	//		lAnimCurve = camera->FieldOfView.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Field of View\n");
	//		}
	//		lAnimCurve = camera->FieldOfViewX.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Field of View X\n");
	//		}
	//		lAnimCurve = camera->FieldOfViewY.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Field of View Y\n");
	//		}
	//		lAnimCurve = camera->OpticalCenterX.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Optical Center X\n");
	//		}
	//		lAnimCurve = camera->OpticalCenterY.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Optical Center Y\n");
	//		}
	//		lAnimCurve = camera->Roll.GetCurve(pAnimLayer);
	//		if (lAnimCurve)
	//		{
	//			FBXSDK_printf("        Roll\n");
	//		}
	//	}
	//	// Display curves specific to a geometry.
	//	if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
	//		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
	//		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
	//	{
	//		FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;
	//		int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
	//		for (int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	//		{
	//			FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
	//			int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
	//			for (int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
	//			{
	//				FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
	//				const char* lChannelName = lChannel->GetName();
	//				lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
	//				if (lAnimCurve)
	//				{
	//					FBXSDK_printf("        Shape %s\n", lChannelName);
	//				}
	//			}
	//		}
	//	}
	//}
	//// Display curves specific to properties
	//FbxProperty lProperty = pNode->GetFirstProperty();
	//while (lProperty.IsValid())
	//{
	//	if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
	//	{
	//		FbxString lFbxFCurveNodeName = lProperty.GetName();
	//		FbxAnimCurveNode* lCurveNode = lProperty.GetCurveNode(pAnimLayer);
	//		if (!lCurveNode) {
	//			lProperty = pNode->GetNextProperty(lProperty);
	//			continue;
	//		}
	//		FbxDataType lDataType = lProperty.GetPropertyDataType();
	//		if (lDataType.GetType() == eFbxBool || lDataType.GetType() == eFbxDouble || lDataType.GetType() == eFbxFloat || lDataType.GetType() == eFbxInt)
	//		{
	//			FbxString lMessage;
	//			lMessage = "        Property ";
	//			lMessage += lProperty.GetName();
	//			if (lProperty.GetLabel().GetLen() > 0)
	//			{
	//				lMessage += " (Label: ";
	//				lMessage += lProperty.GetLabel();
	//				lMessage += ")";
	//			};
	//			FBXSDK_printf(lMessage.Buffer());
	//			for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
	//			{
	//				lAnimCurve = lCurveNode->GetCurve(0U, c);
	//				//if (lAnimCurve)
	//				//	DisplayCurve(lAnimCurve);
	//			}
	//		}
	//		else if (lDataType.GetType() == eFbxDouble3 || lDataType.GetType() == eFbxDouble4 || lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT))
	//		{
	//			char* lComponentName1 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_RED : (char*)"X";
	//			char* lComponentName2 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_GREEN : (char*)"Y";
	//			char* lComponentName3 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_BLUE : (char*)"Z";
	//			FbxString      lMessage;

	//			lMessage = "        Property ";
	//			lMessage += lProperty.GetName();
	//			if (lProperty.GetLabel().GetLen() > 0)
	//			{
	//				lMessage += " (Label: ";
	//				lMessage += lProperty.GetLabel();
	//				lMessage += ")";
	//			}
	//			FBXSDK_printf(lMessage.Buffer());
	//			for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
	//			{
	//				lAnimCurve = lCurveNode->GetCurve(0U, c);
	//				if (lAnimCurve)
	//				{
	//					FBXSDK_printf("        Component ", lComponentName1);
	//				}
	//			}
	//			for (int c = 0; c < lCurveNode->GetCurveCount(1U); c++)
	//			{
	//				lAnimCurve = lCurveNode->GetCurve(1U, c);
	//				if (lAnimCurve)
	//				{
	//					FBXSDK_printf("        Component ", lComponentName2);
	//				}
	//			}
	//			for (int c = 0; c < lCurveNode->GetCurveCount(2U); c++)
	//			{
	//				lAnimCurve = lCurveNode->GetCurve(2U, c);
	//				if (lAnimCurve)
	//				{
	//					FBXSDK_printf("        Component ", lComponentName3);
	//				}
	//			}
	//		}
	//		else if (lDataType.GetType() == eFbxEnum)
	//		{
	//			FbxString lMessage;
	//			lMessage = "        Property ";
	//			lMessage += lProperty.GetName();
	//			if (lProperty.GetLabel().GetLen() > 0)
	//			{
	//				lMessage += " (Label: ";
	//				lMessage += lProperty.GetLabel();
	//				lMessage += ")";
	//			};
	//			FBXSDK_printf(lMessage.Buffer());
	//			for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
	//			{
	//				lAnimCurve = lCurveNode->GetCurve(0U, c);
	//				//if (lAnimCurve)
	//				//	DisplayListCurve(lAnimCurve, &lProperty);
	//			}
	//		}
	//	}
	//	lProperty = pNode->GetNextProperty(lProperty);
	//} // while
#endif
	}
	return 0.0;
}

bool FBXWrangler::LoadMeshes(const FBXImportOptions& options) {
	if (!scene)
		return false;
	
	//nodes
	std::function<void(FbxNode*)> loadNodeChildren = [&](FbxNode* root) {
		for (int i = 0; i < root->GetChildCount(); i++) {
			FbxNode* child = root->GetChild(i);
			NiAVObjectRef nif_child = NULL;
			if (child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
				nif_child = StaticCast<NiAVObject>(importShape(child, options));
				setAvTransform(child, nif_child, true);
			}
			else {
				FbxNodeAttribute::EType type = child->GetNodeAttribute()->GetAttributeType();
				cout << "lol" << endl;
			}
			if (nif_child == NULL) {
				nif_child = new NiNode();
				nif_child->SetName(string(child->GetName()));
				setAvTransform(child, nif_child);
			}
			conversion_Map[child] = nif_child;

			NiNodeRef parent = DynamicCast<NiNode>(conversion_Map[root]);
			if (parent != NULL) {
				vector<Ref<NiAVObject > > children = parent->GetChildren();
				children.push_back(nif_child);
				parent->SetChildren(children);
			}

			loadNodeChildren(child);
		}
	};

	FbxNode* root = scene->GetRootNode();
	conversion_root = new BSFadeNode();
	conversion_root->SetName(string("Scene"));

	if (!hasNoTransform(root)) {
		NiNodeRef proxyNiNode = new NiNode();
		setAvTransform(root, proxyNiNode);
		proxyNiNode->SetName(string("rootTransformProxy"));
		conversion_root->SetChildren({ StaticCast<NiAVObject>(proxyNiNode) });
		conversion_Map[root] = proxyNiNode;
	}
	else {
		conversion_Map[root] = conversion_root;
	}
	loadNodeChildren(root);

	//animations
	size_t stacks = scene->GetSrcObjectCount<FbxAnimStack>();
	if (stacks > 0)
	{
		//create a controller manager
		NiControllerManagerRef controller = new NiControllerManager();
		vector<Ref<NiControllerSequence > > sequences = controller->GetControllerSequences();
		NiMultiTargetTransformControllerRef transformController = new NiMultiTargetTransformController();
		set<NiObjectRef> extra_targets;
		for (int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(i);
			//could contain more than a layer, but by convention we wean just the first
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(0);
			FbxTimeSpan reference = lAnimStack->GetReferenceTimeSpan();
			FbxTimeSpan local = lAnimStack->GetLocalTimeSpan();
			NiControllerSequenceRef sequence = new NiControllerSequence();
			//Translate
			convert(lAnimLayer, sequence, extra_targets, controller, transformController, string(conversion_root->GetName()),reference.GetStart().GetSecondDouble());
			sequence->SetName(string(lAnimStack->GetName()));
			sequences.push_back(sequence);
		}
		
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

	

	//	FbxString lOutputString = "Animation Stack Name: ";
	//	lOutputString += lAnimStack->GetName();
	//	lOutputString += "\n\n";
	//	FBXSDK_printf(lOutputString);

	//	int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
	//	for (int l = 0; l < nbAnimLayers; l++)
	//	{
	//		FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(l);
	//		lOutputString = "AnimLayer ";
	//		lOutputString += l;
	//		lOutputString += "\n";
	//		FBXSDK_printf(lOutputString);
	//		//DisplayAnimation(lAnimLayer, pNode, isSwitcher);
	//		for (const auto & pair : conversion_Map) {
	//			FBXSDK_printf(pair.first->GetName()); FBXSDK_printf("\n");
	//			DisplayChannels(pair.first, lAnimLayer);
	//		}
	//	}

	//}

	return true;
}

bool FBXWrangler::SaveNif(const string& fileName) {

	//for (const auto& obj : conversion_Map) {
	//	objects.push_back(StaticCast<NiObject>(obj.second));
	//}
	//if (find(objects.begin(), objects.end(), conversion_root) == objects.end())
	//	objects.push_back(StaticCast<NiObject>(conversion_root));

	NifInfo info;
	info.userVersion = 12;
	info.userVersion2 = 83;
	info.version = Niflib::VER_20_2_0_7;

	vector<NiObjectRef> objects = RebuildVisitor(conversion_root, info).blocks;
	bsx_flags_t calculated_flags = calculateSkyrimBSXFlags(objects, info);

	BSXFlagsRef bref = new BSXFlags();
	bref->SetName(string("BSX"));
	bref->SetIntegerData(calculated_flags.to_ulong());

	conversion_root->SetExtraDataList({ StaticCast<NiExtraData>(bref) });

	NifFile out(info, objects);
	return out.Save(fileName);
}