#include <src/models/FBXTreeModel.h>

using namespace ckcmd::HKX;

QString getBoneTransform(FbxNode* pNode) {
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
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform(0.0);

	matrixGeo = localMatrix * matrixGeo;

	const FbxVector4 lT = matrixGeo.GetT();
	const FbxQuaternion lR = matrixGeo.GetQ();
	const FbxVector4 lS = matrixGeo.GetS();

	return QString("T:{%1, %2, %3}, Rq{%4, %5, %6, %7}, S{%8, %9, %10}")
		.arg(QString::number(lT[0],'f', 2))
		.arg(QString::number(lT[1],'f', 2))
		.arg(QString::number(lT[2],'f', 2))
		.arg(QString::number(lR[0],'f', 2))
		.arg(QString::number(lR[1],'f', 2))
		.arg(QString::number(lR[2],'f', 2))
		.arg(QString::number(lR[3],'f', 2))
		.arg(QString::number(lS[0],'f', 2))
		.arg(QString::number(lS[1],'f', 2))
		.arg(QString::number(lS[2],'f', 2));
}


FBXTreeModel::FBXTreeModel(FbxNode* root)
    : _scene(root->GetScene())
{

}

FBXTreeModel::FBXTreeModel(const fs::path& file)
	: _scene(nullptr)
{
	ImportScene(file);
}

void FBXTreeModel::CloseScene() {
	if (_scene)
		_scene->Destroy();

	_scene = nullptr;
}

void FBXTreeModel::ImportScene(const fs::path& file, const FBXImportOptions& options) {
	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	ios->SetBoolProp(IMP_FBX_LINK, false);
	ios->SetBoolProp(IMP_FBX_SHAPE, true);
	ios->SetBoolProp(IMP_FBX_GOBO, true);
	ios->SetBoolProp(IMP_FBX_ANIMATION, true);
	ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

	FbxImporter* iImporter = FbxImporter::Create(sdkManager, "");
	if (!iImporter->Initialize(file.string().c_str(), -1, ios)) {
		iImporter->Destroy();
		return;
	}

	if (_scene)
		CloseScene();

	_scene = FbxScene::Create(sdkManager, file.string().c_str());

	bool status = iImporter->Import(_scene);

	FbxAxisSystem maxSystem(FbxAxisSystem::EUpVector::eZAxis, (FbxAxisSystem::EFrontVector)-2, FbxAxisSystem::ECoordSystem::eRightHanded);
	maxSystem.ConvertScene(_scene);

	auto units = _scene->GetGlobalSettings().GetSystemUnit();

	if (!status) {
		FbxStatus ist = iImporter->GetStatus();
		iImporter->Destroy();
		return;
	}
	iImporter->Destroy();

	std::string exporter_name = "";
	std::string exporter_version = "";

	if (_scene->GetSceneInfo() != NULL)
	{
		FbxProperty p = _scene->GetSceneInfo()->GetFirstProperty();
		while (p.IsValid())
		{
			std::string p_name = p.GetNameAsCStr();
			if (p_name == "ApplicationName")
				exporter_name = p.Get<FbxString>().Buffer();
			if (p_name == "ApplicationVersion")
				exporter_version = p.Get<FbxString>().Buffer();
			p = _scene->GetSceneInfo()->GetNextProperty(p);
		}
	}

	if (_scene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::cm)
	{
		const FbxSystemUnit::ConversionOptions lConversionOptions = {
		  false, /* mConvertRrsNodes */
		  true, /* mConvertLimits */
		  true, /* mConvertClusters */
		  true, /* mConvertLightIntensity */
		  true, /* mConvertPhotometricLProperties */
		  true  /* mConvertCameraClipPlanes */
		};

		// Convert the scene to centimeters using the defined options.
		FbxSystemUnit::cm.ConvertScene(_scene, lConversionOptions);
	}

	//Blender sets a 100 factor for no reason
	if (exporter_name.find("Blender") != std::string::npos)
	{
		const FbxSystemUnit::ConversionOptions lConversionOptions = {
			false, /* mConvertRrsNodes */
			true, /* mConvertLimits */
			true, /* mConvertClusters */
			true, /* mConvertLightIntensity */
			true, /* mConvertPhotometricLProperties */
			true  /* mConvertCameraClipPlanes */
		};
		FbxSystemUnit::m.ConvertScene(_scene, lConversionOptions);
	}

	sdkManager->Destroy();
}

QVariant FBXTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return _scene->GetRootNode()->GetName();

	if (role != Qt::DisplayRole)
		return QVariant();

    FbxNode* node = reinterpret_cast<FbxNode*>(index.internalPointer());
	switch (static_cast<FBXTreeColumns>(index.column()))
	{
	case FBXTreeColumns::name: 
		return node->GetName();
	case FBXTreeColumns::transform: 
		return getBoneTransform(node);
	case FBXTreeColumns::boneIndex: 
		return "TODO";
	default:
		break;
	}
    return QVariant();
}

QModelIndex FBXTreeModel::index(int row, int col, const QModelIndex& parent) const
{
    if (!hasIndex(row, col, parent))
        return QModelIndex();

	FbxNode* parentNode;
    if (!parent.isValid())
    {
		parentNode = _scene->GetRootNode();
	}
	else {
		parentNode = reinterpret_cast<FbxNode*>(parent.internalPointer());
	}
	FbxNode* childNode = parentNode->GetChild(row);
    if (nullptr != childNode)
        return createIndex(row, col, childNode);
    return QModelIndex();
}

QModelIndex FBXTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    FbxNode* node = reinterpret_cast<FbxNode*>(index.internalPointer());
	if (nullptr != node && nullptr != node->GetParent())
	{
		FbxNode* parent = node->GetParent();
		if (parent == _scene->GetRootNode())
			return QModelIndex();
		
		FbxNode* grandparent = parent->GetParent();
		if (grandparent != NULL)
		{
			for (int i = 0; i < grandparent->GetChildCount(); i++)
			{
				if (grandparent->GetChild(i) == node->GetParent())
					return createIndex(i, 0, node->GetParent());
			}
		}
		else {
			return createIndex(0, 0, parent);
		}
	}
    return QModelIndex();
}

int FBXTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return _scene->GetRootNode()->GetChildCount();

    FbxNode* node = reinterpret_cast<FbxNode*>(parent.internalPointer());
	if (nullptr != node) {
		int children = node->GetChildCount();
		return children;
	}
    return 0;
}

int FBXTreeModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(FBXTreeColumns::columnMax);
}

QVariant FBXTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (static_cast<FBXTreeColumns>(section))
		{
		case FBXTreeColumns::name: return tr("Node name");
		case FBXTreeColumns::transform: return tr("Position, Translation");
		case FBXTreeColumns::boneIndex: return tr("Animation Track Index");
		default:
			break;
		}
	}
    return QVariant();
}