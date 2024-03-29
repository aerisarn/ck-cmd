#include <src/models/NIFTreeModel.h>
#include <QProgressDialog>

#include <src/log.h>

using namespace ckcmd::HKX;
using namespace ckcmd::NIF;

NIFTreeModel::NIFTreeModel(const fs::path& folder)
{
	int files_to_load = 0;
	for (auto const& dir_entry : fs::recursive_directory_iterator(folder))
	{
		if (dir_entry.path().extension() == ".nif")
		{
			auto file = dir_entry.path();
			files_to_load++;
		}
	}
	
	QProgressDialog progress(QString("Loading %1 NIF files").arg(files_to_load), "Stop", 0, files_to_load);
	progress.setWindowModality(Qt::WindowModal);

	int loaded_files = 0;
	for (auto const& dir_entry : fs::recursive_directory_iterator(folder))
	{
		if (dir_entry.path().extension() == ".nif")
		{
			if (progress.wasCanceled())
				break;
			progress.setValue(loaded_files++);

			auto file = dir_entry.path();
			LOGINFO << "NIF: " << file.string() << '\n';
			_files.emplace_back(
				NifFile(file.string())
			);
		}
	}
}

NIFTreeModel::~NIFTreeModel()
{
}

BSLightingShaderPropertyRef ckcmd::HKX::getProperty(NiAVObjectRef mesh)
{
	if (mesh->IsSameType(NiTriShape::TYPE))
	{
		return DynamicCast<BSLightingShaderProperty>(DynamicCast<NiTriShape>(mesh)->GetShaderProperty());
	}
	if (mesh->IsSameType(BSTriShape::TYPE))
	{
		return DynamicCast<BSLightingShaderProperty>(DynamicCast<BSTriShape>(mesh)->GetShaderProperty());
	}
	return nullptr;
}

QVariant NIFTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return _folder.string().c_str();

	if (role != Qt::DisplayRole)
		return QVariant();

	if (index.internalId() == (quintptr)-1 && index.row() < _files.size())
	{
		return _files.at(index.row()).fileName.c_str();
	}
	if (index.internalId() < _files.size())
	{
		auto& nif_file = _files.at(index.internalId());
		int meshes = nif_file.getNumBlocks({ NiTriShape::TYPE, BSTriShape::TYPE });
		if (index.row() < meshes)
		{
			auto mesh = DynamicCast<NiAVObject>(nif_file.getBlock(index.row(), { NiTriShape::TYPE,BSTriShape::TYPE }));
			auto property = getProperty(mesh);
			QString diffuse = "<none>";
			if (property != NULL)
			{
				auto slot = property->GetTextureSet();
				if (nullptr != slot)
				{
					auto& textures = slot->GetTextures();
					if (textures.size() > 0)
						diffuse = textures[0].c_str();
				}
			}
			QString label = QString("%1 [%2]").arg(mesh->GetName().c_str()).arg(diffuse);
			return label;
		}
	}
	return "<invalid>";
}

QModelIndex NIFTreeModel::index(int row, int col, const QModelIndex& parent) const
{
    if (!hasIndex(row, col, parent))
        return QModelIndex();

    if (!parent.isValid())
    {
		return createIndex(row, col, (quintptr)-1);
	}

	return createIndex(row, col, parent.row());
}

QModelIndex NIFTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

	if (index.internalId() == (quintptr)-1)
		return QModelIndex();

	return createIndex(index.internalId(), 0, (quintptr)-1);
}

int NIFTreeModel::rowCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return _files.size();

	if (index.internalId() == (quintptr)-1 && index.row() < _files.size())
	{
		auto& nif_file = _files.at(index.row());
		return nif_file.getNumBlocks({ NiTriShape::TYPE, BSTriShape::TYPE });
	}
    return 0;
}

int NIFTreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant NIFTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

Niflib::NiAVObjectRef NIFTreeModel::block(QModelIndex index)
{
	if (index.internalId()>=0 && index.internalId() < _files.size())
	{
		auto& nif_file = _files.at(index.internalId());
		int meshes = nif_file.getNumBlocks({ NiTriShape::TYPE, BSTriShape::TYPE });
		if (index.row() < meshes)
		{
			return DynamicCast<NiAVObject>(nif_file.getBlock(index.row(), {NiTriShape::TYPE, BSTriShape::TYPE }));
		}
	}
	return nullptr;
}

ckcmd::NIF::NifFile& NIFTreeModel::file(QModelIndex index)
{
	if (index.internalId() >= 0 && index.internalId() < _files.size())
	{
		return _files.at(index.internalId());
	}
	if (index.row() >= 0 && index.row() < _files.size())
	{
		return _files.at(index.row());
	}
	return ckcmd::NIF::NifFile();
}