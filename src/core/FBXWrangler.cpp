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
using namespace ckcmd::HKX;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // ...
	}
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

	scene = FbxScene::Create(sdkManager, fileName.c_str());

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



FbxAMatrix getNodeTransform(FbxNode* pNode) {
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
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

	return localMatrix * matrixGeo;
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
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

	FbxNode* pParentNode = pNode->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
	}

	return parentMatrix * localMatrix * matrixGeo;
}

void setAvTransform(FbxNode* node, NiAVObject* av, bool rotation_only = false) {
	FbxAMatrix tr = getNodeTransform(node);
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
	if (scaling[0] == scaling[1] && scaling[1] == scaling[2])
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

void FBXWrangler::convertSkins(FbxMesh* m, NiTriShapeRef shape) {
	if (m->GetDeformerCount(FbxDeformer::eSkin) > 0) {
		NiSkinInstanceRef skin = new NiSkinInstance();
		NiSkinDataRef data = new NiSkinData();
		NiSkinPartitionRef spartition = new NiSkinPartition();

		map<NiNode* , BoneData > bones_data;
		vector<SkinPartition > partitions;
		set<NiNode*> bones;
		vector<NiNode*> vbones;

		multimap<int, int> faces_map;

		int numTris = m->GetPolygonCount();
		for (int t = 0; t < numTris; t++) {
			if (m->GetPolygonSize(t) != 3)
				continue;

			int p1 = m->GetPolygonVertex(t, 0);
			int p2 = m->GetPolygonVertex(t, 1);
			int p3 = m->GetPolygonVertex(t, 2);

			faces_map.insert({ p1,t });
			faces_map.insert({ p2,t });
			faces_map.insert({ p3,t });

		}

		multimap<int, bone_weight> influence_map;

		for (int iSkin = 0; iSkin < m->GetDeformerCount(FbxDeformer::eSkin); iSkin++) {
			FbxSkin* skin = (FbxSkin*)m->GetDeformer(iSkin, FbxDeformer::eSkin);

			SkinPartition partition;
			vector<unsigned short >& vertexMap = partition.vertexMap;
			partition.vertexMap.resize(0);
			map<int, unsigned short> inverse_vertex_map;
			set<int> cluster_triangles;
			vector<int> local_bones;

			for (int iCluster = 0; iCluster < skin->GetClusterCount(); iCluster++) {
				FbxCluster* cluster = skin->GetCluster(iCluster);
				if (!cluster->GetLink())
					continue;

				skinned_bones.insert(cluster->GetLink());
				NiNode* bone = DynamicCast<NiNode>(conversion_Map[cluster->GetLink()]);

				if (bones_data.find(bone) == bones_data.end())
					bones_data[bone] = BoneData();

				BoneData& this_bone_data = bones_data[bone];

				FbxTime lTime;
				lTime.SetSecondDouble(0);
				this_bone_data.skinTransform = GetAvTransform(cluster->GetLink()->EvaluateGlobalTransform(lTime).Inverse());
				this_bone_data.numVertices += cluster->GetControlPointIndicesCount();

				vector<BoneVertData>& vertsData = this_bone_data.vertexWeights;
				if (bone != NULL) {
					for (int iPoint = 0; iPoint < cluster->GetControlPointIndicesCount(); iPoint++) {
						int v = cluster->GetControlPointIndices()[iPoint];
						float w = cluster->GetControlPointWeights()[iPoint];

						vector<unsigned short >::iterator v_local_it = find(vertexMap.begin(), vertexMap.end(), v);

						if (v_local_it == vertexMap.end()) {
							partition.numVertices++;
							inverse_vertex_map[v] = vertexMap.size();
							vertexMap.push_back(v);
						}

						pair <multimap<int, int>::iterator, multimap<int, int>::iterator> this_faces = faces_map.equal_range(v);

						for (multimap<int, int>::iterator faces_it = this_faces.first; faces_it != this_faces.second; faces_it++) {
							cluster_triangles.insert(faces_it->second);
						}

						
						influence_map.insert({ v, bone_weight(bone, w) });

						//global data
						BoneVertData vertData;
						vertData.index = v;
						vertData.weight = w;
						vertsData.push_back(vertData);
					}

					int b_local_index = 0;
					if (bones.insert(bone).second) {
						vbones.push_back(bone);
					}

				}
			}

			vector< vector<byte > >& pindexes = partition.boneIndices;
			pindexes.resize(partition.numVertices);
			vector< vector<float > >& vweights = partition.vertexWeights;
			vweights.resize(partition.numVertices);
			partition.numWeightsPerVertex = 4;
			partition.numTriangles = cluster_triangles.size();
			partition.hasFaces = cluster_triangles.size() > 0;
			partition.triangles.resize(partition.numTriangles);
			partition.trianglesCopy.resize(partition.numTriangles);

			partition.hasVertexMap = vertexMap.size() > 0;
			partition.hasVertexWeights = influence_map.size() > 0;
			partition.hasBoneIndices = influence_map.size() > 0;

			for (const auto& pair : influence_map) {
				int index = inverse_vertex_map[pair.first];
				vector<byte >& pindex = pindexes[index];
				vector<float >& vweight = vweights[index];
				vector<NiNode*>::iterator bone_it = find(vbones.begin(), vbones.end(), pair.second.bone);
				if (bone_it != vbones.end()) {
					int bone_index = distance(vbones.begin(), bone_it);
					vector<unsigned short >::iterator local_it = find(partition.bones.begin(), partition.bones.end(), bone_index);
					size_t local_index = 0;
					if (local_it == partition.bones.end())
					{
						local_index = partition.bones.size();
						partition.bones.push_back(bone_index);
					}
					else {
						local_index = distance(partition.bones.begin(), local_it);
					}

					vweight.push_back(pair.second.weight);
					pindex.push_back(local_index);
				}
			} 

			for (int i_f = 0; i_f < partition.numVertices; i_f++) {

				vector<byte >& pindex = pindexes[i_f]; 
				if (pindex.size() > 4)
					Log::Error("Too many indices for bone");

				vector<float >& vweight = vweights[i_f]; 
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

			for (int t = 0; t < cluster_triangles.size(); t++) {
				int p1 = m->GetPolygonVertex(t, 0);
				int p2 = m->GetPolygonVertex(t, 1);
				int p3 = m->GetPolygonVertex(t, 2);


				partition.triangles[t] = Triangle
				(
					inverse_vertex_map[p1],
					inverse_vertex_map[p2],
					inverse_vertex_map[p3]
				);
			}

			//for (const auto& pair : influence) {
			//	vector<byte >& pindex = pindexes[pair.second];
			//	vector<float >& vweight = vweights[pair.second];

			//}

			

			partitions.push_back(partition);
		}


		spartition->SetSkinPartitionBlocks(partitions);

		vector<BoneData > vbones_data;
		//for (const auto& p : bones_data) vbones_data.push_back(p.second);
		for (const auto& bone : vbones) vbones_data.push_back(bones_data[bone]);


		data->SetBoneList(vbones_data);
		data->SetHasVertexWeights(1);

		NiTransform id; id.scale = 1; data->SetSkinTransform(id);

		//vector<NiNode*> vbones;
		//for (const auto& b : bones) vbones.push_back(b);
		skin->SetBones(vbones);
		skin->SetData(data);
		skin->SetSkinPartition(spartition);
		skin->SetSkeletonRoot(conversion_root);

		shape->SetSkinInstance(skin);
	}

}

template <class Type, typename T> 
T getIndexedElement(FbxLayerElementTemplate<Type>* layer, size_t index)
{
	switch (layer->GetReferenceMode()) {
		case FbxLayerElement::EReferenceMode::eDirect:
			return layer->GetDirectArray().GetAt(index);
		case FbxLayerElement::eIndex:
		case FbxLayerElement::eIndexToDirect:
			return layer->GetDirectArray().GetAt(layer->GetIndexArray()->GetAt(index));
	}
}

template <class Type, typename T>
void getElement(FbxLayerElementTemplate<Type>* layer, size_t index, T& element) {
	switch (layer->GetMappingMode())
	{
	case FbxLayerElement::EMappingMode::eByControlPoint:
		T = getIndexedElement(layer, index)
	case FbxLayerElement::EMappingMode::eByPolygonVertex:
	case FbxLayerElement::EMappingMode::eByPolygon:
	case FbxLayerElement::EMappingMode::eByEdge:
	case FbxLayerElement::EMappingMode::eAllSame:
	}
}

vector<NiTriShapeRef> FBXWrangler::importMultipleShape(FbxNode* child, const FBXImportOptions& options) {
	//todo
	return vector<NiTriShapeRef>();
}

NiTriShapeRef FBXWrangler::importShape(FbxNode* child, const FBXImportOptions& options) {
	NiTriShapeRef out = new NiTriShape();
	NiTriShapeDataRef data = new NiTriShapeData();

	bool hasAlpha = false;

	FbxMesh* m = (FbxMesh*)child->GetNodeAttribute();

	const char* lMappingTypes[] = { "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" };
	const char* lReferenceMode[] = { "Direct", "Index", "Index to Direct" };

	if (!m->IsTriangleMesh()) {
		FbxGeometryConverter converter(sdkManager);
		m = (FbxMesh*)converter.Triangulate((FbxNodeAttribute*)m, true);
	}

	int lMtrlCount = 0;
	FbxNode* lNode = NULL;
	if (m) {
		lNode = m->GetNode();
		if (lNode)
			lMtrlCount = lNode->GetMaterialCount();
	}

	map<int, int> material_map;

	for (int l = 0; l < m->GetElementMaterialCount(); l++)
	{
		FbxGeometryElementMaterial* leMat = m->GetElementMaterial(l);
		if (leMat)
		{

			int lMaterialCount = 0;

			if (leMat->GetReferenceMode() == FbxGeometryElement::eDirect ||
				leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				lMaterialCount = lMtrlCount;
			}

			if (leMat->GetReferenceMode() == FbxGeometryElement::eIndex ||
				leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				int i;

				int lIndexArrayCount = leMat->GetIndexArray().GetCount();
				for (i = 0; i < lIndexArrayCount; i++)
				{					
					material_map[i] = leMat->GetIndexArray().GetAt(i);
				}
			}
		}
	}

	FbxGeometryElementUV* uv = m->GetElementUV(0);
	FbxGeometryElementNormal* normal = m->GetElementNormal(0);
	FbxGeometryElementVertexColor* vc = m->GetElementVertexColor(0);
	//FbxLayerElementArrayTemplate<int>* m_indices = new FbxLayerElementArrayTemplate<int>(eFbxInt);
	//m->GetMaterialIndices(&m_indices);

	//size_t m_ind_size = m_indices->GetCount();

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

	FbxAMatrix node_trans; node_trans = m->GetPivot(node_trans);

	for (int v = 0; v < numVerts; v++) {
		FbxVector4 vert = m->GetControlPointAt(v);

		if (uv && uv->GetMappingMode() != FbxGeometryElement::eByControlPoint &&
			uv->GetMappingMode() != FbxGeometryElement::eByPolygonVertex) {
			FbxGeometryElement::EMappingMode m = uv->GetMappingMode();
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


	const char* uvName = nullptr;
	if (uv) {
		uvName = uv->GetName();
		uvs.resize(numVerts);
	}

	if (vc && vcs.empty()) {
		vcs.resize(numVerts);
	}

	if (normal && normals.empty())
		normals.resize(numVerts);

	vector<Triangle> tris;

	for (int t = 0; t < numTris; t++) {
		if (m->GetPolygonSize(t) != 3)
			continue;

		int p1 = m->GetPolygonVertex(t, 0);
		int p2 = m->GetPolygonVertex(t, 1);
		int p3 = m->GetPolygonVertex(t, 2);
		tris.emplace_back(p1, p2, p3);

		if (normal && normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			FbxVector4 v_n;
			bool isUnmapped;

			if (m->GetPolygonVertexNormal(t, 0, v_n))
				normals[p1] = Vector3(v_n.mData[0], v_n.mData[1], v_n.mData[2]);

			if (m->GetPolygonVertexNormal(t, 1, v_n))
				normals[p2] = Vector3(v_n.mData[0], v_n.mData[1], v_n.mData[2]);

			if (m->GetPolygonVertexNormal(t, 2, v_n))
				normals[p3] = Vector3(v_n.mData[0], v_n.mData[1], v_n.mData[2]);
		}

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
		if (vc && vc->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			FbxColor color;
			for (int ti = 0; ti < 3; ti++) {
				switch (vc->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
				{
					color = vc->GetDirectArray().GetAt(t*3+ti);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = vc->GetIndexArray().GetAt(t * 3 + ti);
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

				vcs[m->GetPolygonVertex(t, ti)] =
					Color4
					(
						(float)color.mRed,
						(float)color.mGreen,
						(float)color.mBlue,
						(float)color.mAlpha
					)
				;

			}
			//if (m->GetPolygonVertex(t, 0)) 
			//{
			//	vc->GetDirectArray().GetAt(v).mData[1]
			//	uvs[p1] = TexCoord(v_uv.mData[0], v_uv.mData[1]);
			//}
			//if (m->GetPolygonVertex(t, 1))
			//	uvs[p2] = TexCoord(v_uv.mData[0], v_uv.mData[1]);

			//if (m->GetPolygonVertex(t, 2))
			//	uvs[p3] = TexCoord(v_uv.mData[0], v_uv.mData[1]);
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

	if (vcs.size())
	{
		data->SetHasVertexColors(true);
		data->SetVertexColors(vcs);
	}

	if (m->GetDeformerCount(FbxDeformer::eSkin) > 0) {

		skins_Map[m] = out;
	}
	//	NiSkinInstanceRef skin = new NiSkinInstance();

	//	for (int iSkin = 0; iSkin < m->GetDeformerCount(FbxDeformer::eSkin); iSkin++) {
	//		FbxSkin* skin = (FbxSkin*)m->GetDeformer(iSkin, FbxDeformer::eSkin);

	//		for (int iCluster = 0; iCluster < skin->GetClusterCount(); iCluster++) {
	//			FbxCluster* cluster = skin->GetCluster(iCluster);
	//			if (!cluster->GetLink())
	//				continue;

	//			std::string bone = cluster->GetLink()->GetName();
	//			//shape.boneNames.insert(bone);
	//			for (int iPoint = 0; iPoint < cluster->GetControlPointIndicesCount(); iPoint++) {
	//				int v = cluster->GetControlPointIndices()[iPoint];
	//				float w = cluster->GetControlPointWeights()[iPoint];
	//				//shape.boneSkin[bone].SetWeight(v, w);
	//			}
	//		}
	//	}
	//}

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

	if (hasAlpha) {
		NiAlphaPropertyRef alpharef = new NiAlphaProperty();
		alpharef->SetFlags(237);
		alpharef->SetThreshold(128);
		out->SetAlphaProperty(alpharef);
	}

	if (child->GetMaterialCount() != 0)
	{
		std::vector<std::string> vTextures(9);
		FbxSurfaceMaterial * material = child->GetMaterial(0);
		FbxProperty prop = NULL;
		FbxPropertyT<FbxDouble3> colour;
		FbxPropertyT<FbxDouble> factor;
		FbxFileTexture *texture;

		//specular
		colour = material->FindProperty(material->sSpecular, true);
		factor = material->FindProperty(material->sSpecularFactor, true);
		if (colour.IsValid() && factor.IsValid())
		{
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

		//shiny/gloss
		factor = material->FindProperty(material->sShininess, true);
		if (factor.IsValid())
		{
			shader->SetGlossiness(factor.Get());
		}

		//Diffuse.
		prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse, true);
		factor = material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor, true);
		if (prop.IsValid() && factor.IsValid())
		{
			texture = prop.GetSrcObject<FbxFileTexture>(0);
			if (texture != NULL)
				vTextures[0] = texture->GetFileName();
		}

		//Normal/Bump.
		prop = material->FindProperty(FbxSurfaceMaterial::sBump, true);
		factor = material->FindProperty(FbxSurfaceMaterial::sBumpFactor, true);
		if (prop.IsValid() && factor.IsValid())
		{
			texture = prop.GetSrcObject<FbxFileTexture>(0);
			if (texture != NULL)
				vTextures[1] = texture->GetFileName();
		}

		textures->SetTextures(vTextures);
	}

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
	if (curveX != NULL)
	{
		for (int i = 0; i < curveX->KeyGetCount(); i++)
		{
			FbxAnimCurveKey& key = curveX->KeyGet(i);
			//timeMapX[key.GetTime().GetSecondDouble()] = i;
			times.insert(key.GetTime().GetSecondDouble());
		}
	}
	if (curveY != NULL)
	{
		for (int i = 0; i < curveY->KeyGetCount(); i++)
		{
			FbxAnimCurveKey& key = curveY->KeyGet(i);
			//timeMapY[key.GetTime().GetSecondDouble()] = i;
			times.insert(key.GetTime().GetSecondDouble());
		}
	}
	if (curveZ != NULL)
	{
		for (int i = 0; i < curveX->KeyGetCount(); i++)
		{
			FbxAnimCurveKey& key = curveZ->KeyGet(i);
			//timeMapZ[key.GetTime().GetSecondDouble()] = i;
			times.insert(key.GetTime().GetSecondDouble());
		}
	}
	
	if (times.size() > 0)
	{
		NiTransformDataRef data = interpolator->GetData();
		if (data == NULL) data = new NiTransformData();
		KeyGroup<Vector3 > tkeys = data->GetTranslations();
		vector<Key<Vector3 > > keyvalues = tkeys.keys;
		for (const auto& time : times) {
			FbxTime lTime;

			// Set the time at two seconds.
			lTime.SetSecondDouble((float)time);
			FbxVector4 trans = node->EvaluateLocalTransform(lTime).GetT();

			//float x = 0.0;
			//float y = 0.0;
			//float z = 0.0;
			//if (timeMapX.find(time) != timeMapX.end())
			//{
			//	x = curveX->KeyGet(timeMapX[time]).GetValue();
			//}
			//else {
			//	if (curveX != NULL)
			//		x = curveX->Evaluate(time);
			//	else
			//		x = position[0];
			//}
			//if (timeMapY.find(time) != timeMapY.end())
			//{
			//	y = curveY->KeyGet(timeMapY[time]).GetValue();
			//}
			//else
			//{
			//	if (curveY != NULL)
			//		y = curveY->Evaluate(time);
			//	else
			//		y = position[1];
			//}
			//if (timeMapZ.find(time) != timeMapZ.end())
			//{
			//	z = curveZ->KeyGet(timeMapZ[time]).GetValue();
			//}
			//else
			//{
			//	if (curveZ != NULL)
			//		z = curveZ->Evaluate(time);
			//	else
			//		z = position[2];
			//}

			Key<Vector3 > temp;
			temp.data = Vector3(trans[0], trans[1], trans[2]);
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

void addRotationKeys(NiTransformInterpolator* interpolator, FbxNode* node, FbxAnimCurve* curveI, FbxAnimCurve* curveJ, FbxAnimCurve* curveK, double time_offset) {
	//this is simpler because curves can be evaluated one at a time
	NiTransformDataRef data = interpolator->GetData();
	if (data == NULL) data = new NiTransformData();
	Niflib::array<3, KeyGroup<float > > tkeys = data->GetXyzRotations();
	//set<double> times;
	//if (curveI != NULL)
	//{
	//	for (int i = 0; i < curveI->KeyGetCount(); i++)
	//	{
	//		FbxAnimCurveKey& key = curveI->KeyGet(i);
	//		times.insert(key.GetTime().GetSecondDouble());
	//	}
	//}
	//if (curveJ != NULL)
	//{
	//	for (int i = 0; i < curveJ->KeyGetCount(); i++)
	//	{
	//		FbxAnimCurveKey& key = curveJ->KeyGet(i);
	//		timeMapY[key.GetTime().GetSecondDouble()] = i;
	//		times.insert(key.GetTime().GetSecondDouble());
	//	}
	//}
	//if (curveK != NULL)
	//{
	//	for (int i = 0; i < curveK->KeyGetCount(); i++)
	//	{
	//		FbxAnimCurveKey& key = curveK->KeyGet(i);
	//		timeMapZ[key.GetTime().GetSecondDouble()] = i;
	//		times.insert(key.GetTime().GetSecondDouble());
	//	}
	//}
	//if (times.size() > 0)
	//{
	//	for (const auto& time : times) {
	//		FbxVector4 rot = node->EvaluateLocalRotation(time);
	//		Key<float> new_key_i;
	//		new_key_i.data = deg2rad(rot[0]);
	//		new_key_i.forward_tangent = deg2rad(node- curveI->KeyGetRightTangentVelocity(i));
	//		new_key_i.backward_tangent = deg2rad(curveI->KeyGetLeftTangentVelocity(i));
	//		tkeys[0].keys.push_back(new_key_i);
	//	}
	//}

	


	int IkeySize = 0;
	if (curveI != NULL)
	{
		IkeySize = curveI->KeyGetCount();
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
	}
	int JkeySize = 0;
	if (curveJ != NULL)
	{
		JkeySize = curveJ->KeyGetCount();
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
	}
	int KkeySize = 0;
	if (curveK != NULL)
	{
		KkeySize = curveK->KeyGetCount();
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
	for (FbxNode* pNode : unskinned_bones)
	{
		//FbxNode* pNode = pair.first;
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
			NiQuatTransform trans;
			trans.translation = Vector3(0, 0, 0);
			trans.rotation = Quaternion(1, 0, 0, 0);
			trans.scale = 1;
			interpolator->SetTransform(trans);

			if (lXAnimCurve != NULL || lYAnimCurve != NULL || lZAnimCurve != NULL) {
				addTranslationKeys(interpolator, pNode, lXAnimCurve, lYAnimCurve, lZAnimCurve, last_start);
			}

			if (lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL) {
				addRotationKeys(interpolator, pNode, lIAnimCurve, lJAnimCurve, lKAnimCurve, last_start);
			}

			NiTransformDataRef data = interpolator->GetData();
			if (data != NULL) {
				KeyGroup<float> scales;
				scales.numKeys = 0;
				scales.keys = {};
				data->SetScales(scales);
			}

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
		//could contain more than a layer, but by convention we wean just the first
		FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(0);
		FbxTimeSpan reference = lAnimStack->GetReferenceTimeSpan();
		FbxTimeSpan local = lAnimStack->GetLocalTimeSpan();
		NiControllerSequenceRef sequence = new NiControllerSequence();
		//Translate
		convert(lAnimLayer, sequence, extra_targets, controller, transformController, string(conversion_root->GetName()), reference.GetStart().GetSecondDouble());
		sequence->SetName(string(lAnimStack->GetName()));
		sequences_names.insert(string(lAnimStack->GetName()));
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
			//check if it's animated into this layer
			FbxProperty p = pNode->GetFirstProperty();

			bool isFloatTrack = true;
			bool isAnimated = false;

			while (p.IsValid())
			{
				if (p.IsAnimated(pAnimLayer))
				{
					FbxAnimCurveNode* curveNode = p.GetCurveNode();
					for (int j = 0; j < curveNode->GetChannelsCount(); j++)
					{
						if (strcmp(FBXSDK_CURVENODE_COMPONENT_X, curveNode->GetChannelName(j).Buffer())==0 ||
							strcmp(FBXSDK_CURVENODE_COMPONENT_Y, curveNode->GetChannelName(j).Buffer())==0 ||
							strcmp(FBXSDK_CURVENODE_COMPONENT_Z, curveNode->GetChannelName(j).Buffer()) == 0)
						{						//check the pNode Type 
							isFloatTrack = false;
							break;
						}
					}
					isAnimated = true;
				}
				if (!isFloatTrack)
					break;

				if (string(p.GetNameAsCStr()).find("hk") != string::npos)
					annotated.insert(pNode);

				p = pNode->GetNextProperty(p);
			}

			if (!isFloatTrack)
			{
				if (skinned_bones.find(pNode) == skinned_bones.end())
				{
					unskinned_bones.insert(pNode);
					unskinned_animations.insert(lAnimStack);
				}
				else {
					skinned_animations.insert(lAnimStack);
				}
			}
			else {
				float_tracks.insert(pNode);
			}

			//FbxAnimCurve* lXAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			//FbxAnimCurve* lYAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			//FbxAnimCurve* lZAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			//FbxAnimCurve* lIAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			//FbxAnimCurve* lJAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			//FbxAnimCurve* lKAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

			//if (lXAnimCurve != NULL || lYAnimCurve != NULL || lZAnimCurve != NULL ||
			//	lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL)
			//{
			//	//check the pNode Type 
			//	if (skinned_bones.find(pNode) == skinned_bones.end())
			//	{
			//		unskinned_bones.insert(pNode);
			//		unskinned_animations.insert(lAnimStack);
			//	}
			//	else {
			//		skinned_animations.insert(lAnimStack);
			//	}
			//}
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

void sanitizeString(string& to_sanitize)
{
	replaceAll(to_sanitize, " ", "_s_");
	replaceAll(to_sanitize, "[", "_ob_");
	replaceAll(to_sanitize, "]", "_cb_");
}

bool FBXWrangler::LoadMeshes(const FBXImportOptions& options) {
	if (!scene)
		return false;
	
	//nodes
	std::function<void(FbxNode*)> loadNodeChildren = [&](FbxNode* root) {
		for (int i = 0; i < root->GetChildCount(); i++) {
			FbxNode* child = root->GetChild(i);
			NiAVObjectRef nif_child = NULL;
			if (child->GetNodeAttribute() != NULL && child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
				nif_child = StaticCast<NiAVObject>(importShape(child, options));
				setAvTransform(child, nif_child);
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
				conversion_parent_Map[StaticCast<NiAVObject>(nif_child)] = StaticCast<NiAVObject>(parent);
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

	//skins
	for (const auto& p : skins_Map)
	{
		convertSkins(p.first, p.second);
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
			//add keyed annotations into tracks
			
			//add floats, single float tracks
			
			//build fbx parent map
			vector<FbxNode*> fbx_nodes;
			for (const auto& node : skeleton) {
				fbx_nodes.push_back(node);
			}
			vector<int> fbx_parent_map(fbx_nodes.size());
			for (vector<FbxNode*>::iterator node_it = fbx_nodes.begin(); node_it != fbx_nodes.end(); node_it++) 
			{
				vector<FbxNode*>::iterator parent_it = find(fbx_nodes.begin(), fbx_nodes.end(), (*node_it)->GetParent());
				if (parent_it == fbx_nodes.end())
					fbx_parent_map[distance(fbx_nodes.begin(), node_it)] = -1;
				else
					fbx_parent_map[distance(fbx_nodes.begin(), node_it)] = distance(fbx_nodes.begin(), parent_it);
			}
			//build nodes index vector;
			set<size_t> skinned_bones_indexes;
			for (const auto& index : skinned_bones) {
				skinned_bones_indexes.insert(distance(fbx_nodes.begin(), find(fbx_nodes.begin(), fbx_nodes.end(), index)));
			}

			size_t parent_index = getNearestCommonAncestor(fbx_parent_map, skinned_bones_indexes);
			FbxNode* fbx_skeletal_root = fbx_nodes[parent_index];

			if (external_skeleton_path.empty())
			{
				
				//get back the ordered skeleton
				vector<FbxNode*> hkskeleton = hkxWrapper.create_skeleton("skeleton", skeleton);
				//create the sequences
				havok_sequences = hkxWrapper.create_animations("skeleton", hkskeleton, skinned_animations, scene->GetGlobalSettings().GetTimeMode());
			}
			else {
				string external_name;
				string external_root;
				vector<string> external_bones = hkxWrapper.read_track_list(external_skeleton_path, external_name, external_root);
				//maya is picky about names, and stuff may be very well sanitized! especially skeeltons, which use an illegal syntax in Skyrim
				vector<string> sanitized_bones;
				for (const auto& name : external_bones)
				{
					string sanitized = name;
					sanitizeString(sanitized);
					sanitized_bones.push_back(sanitized);
				}

				string sanitized_external_root = external_root;
				sanitizeString(sanitized_external_root);
				vector<FbxNode*> tracks;
				vector<uint32_t> transformTrackToBoneIndices;

				
				//check that our tracks actually belong to this skeleton, at least on names;
				for (FbxNode* bone : fbx_nodes)
				{
					string sanitized = bone->GetName();
					sanitizeString(sanitized);
					vector<string>::iterator bone_position = find(sanitized_bones.begin(), sanitized_bones.end(), sanitized);
					if (bone_position == sanitized_bones.end())
					{
						Log::Warn("%s", string("Wrong skeleton: " + string(bone->GetName()) + " bone NOT FOUND into skeleton " + external_skeleton_path + " ! aborting.").c_str());
					}
					else {
						if (sanitized == sanitized_external_root)
							external_root = sanitized_external_root;
						transformTrackToBoneIndices.push_back(distance(sanitized_bones.begin(), bone_position));
						tracks.push_back(bone);
					}
				}

				havok_sequences = hkxWrapper.create_animations(external_name, tracks, skinned_animations, scene->GetGlobalSettings().GetTimeMode(), transformTrackToBoneIndices);
			}
		}
	}

	return true;
}

bool FBXWrangler::SaveAnimation(const string& fileName) {
	hkxWrapper.write_animations(fileName, havok_sequences);
	return true;
}

bool FBXWrangler::SaveNif(const string& fileName) {

	NifInfo info;
	info.userVersion = 12;
	info.userVersion2 = 83;
	info.version = Niflib::VER_20_2_0_7;

	vector<NiObjectRef> objects = RebuildVisitor(conversion_root, info).blocks;
	bsx_flags_t calculated_flags = calculateSkyrimBSXFlags(objects, info);

	//adjust for havok
	if (!skinned_animations.empty())
		calculated_flags[0] = true;

	BSXFlagsRef bref = new BSXFlags();
	bref->SetName(string("BSX"));
	bref->SetIntegerData(calculated_flags.to_ulong());



	conversion_root->SetExtraDataList({ StaticCast<NiExtraData>(bref) });

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