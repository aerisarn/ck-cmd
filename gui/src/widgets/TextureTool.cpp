#include "TextureTool.h"

#include <QFileDialog>
#include <QMessageBox>

#include <src/log.h>


using namespace ckcmd;
using namespace ckcmd::NIF;
using namespace Niflib;

QString TextureTool::_lastOpenedFolder = "";

TextureTool::TextureTool(QWidget* parent) :
	_visibleAction(nullptr),
	_model(nullptr)
{
	setupUi(this);
}

void TextureTool::on_folderSelectButton_clicked(bool checked)
{
	auto folder = QFileDialog::getExistingDirectory(this,
		tr("Select root folder"), _lastOpenedFolder);
	if (!folder.isEmpty())
	{
		folderLineEdit->setText(folder);
		_lastOpenedFolder = folder;
	}
}

void TextureTool::on_folderLineEdit_textChanged(const QString& text)
{

	std::string std_text = text.toUtf8().constData();
	if (fs::exists(std_text) && fs::is_directory(std_text))
	{
		if (nullptr != _model)
		{
			disconnect(nifView->selectionModel(), &QItemSelectionModel::currentChanged, this, &TextureTool::on_nifView_selectionChanged);
			disconnect(filterLineEdit, &QLineEdit::textChanged, _proxyModel.get(), &QSortFilterProxyModel::setFilterFixedString);
		}

		_model = std::make_shared< ckcmd::HKX::NIFTreeModel>(std_text);

		_proxyModel = make_shared<QSortFilterProxyModel>(this);
		_proxyModel->setSourceModel(_model.get());
		_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
		_proxyModel->setRecursiveFilteringEnabled(true);
		connect(filterLineEdit,
			&QLineEdit::textChanged,
			[=](const QString& pattern) 
			{
				_proxyModel->setFilterFixedString(pattern);
				nifView->expandAll();
			}
		);

		nifView->setModel(_proxyModel.get());
		nifView->setVisible(false);
		nifView->resizeColumnToContents(0);
		nifView->expandAll();
		nifView->setVisible(true);

		connect(nifView->selectionModel(), &QItemSelectionModel::currentChanged, this, &TextureTool::on_nifView_selectionChanged);
	
		if (viewModeCheckBox->isEnabled())
			nifView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		else
			nifView->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	}
}

QAction* TextureTool::toggleViewAction()
{
	if (_visibleAction == nullptr)
	{
		_visibleAction = new QAction("&Open Texture Tool", nullptr);
		_visibleAction->setStatusTip(tr("Batch manipulation of NIF files"));
		connect(_visibleAction, SIGNAL(triggered()), this, SLOT(open()));
	}
	return _visibleAction;
}

void TextureTool::populateValues(const QModelIndex& current)
{
	auto block = _model->block(current);
	if (nullptr != block)
	{
		auto property = ckcmd::HKX::getProperty(block);
		if (property != NULL)
		{
			//Set Shader
			BSLightingShaderPropertyShaderType shader_type = property->GetSkyrimShaderType();
			shaderTypeComboBox->setCurrentIndex((int)shader_type);

			//Texture Slots
			textureSlot_0->setText("");
			textureSlot_1->setText("");
			textureSlot_2->setText("");
			textureSlot_3->setText("");
			textureSlot_4->setText("");
			textureSlot_5->setText("");
			textureSlot_6->setText("");
			textureSlot_7->setText("");

			auto slot = property->GetTextureSet();
			if (nullptr != slot)
			{
				auto& textures = slot->GetTextures();
				if (textures.size() > 0)
					textureSlot_0->setText(textures[0].c_str());
				if (textures.size() > 1)
					textureSlot_1->setText(textures[1].c_str());
				if (textures.size() > 2)
					textureSlot_2->setText(textures[2].c_str());
				if (textures.size() > 3)
					textureSlot_3->setText(textures[3].c_str());
				if (textures.size() > 4)
					textureSlot_4->setText(textures[4].c_str());
				if (textures.size() > 5)
					textureSlot_5->setText(textures[5].c_str());
				if (textures.size() > 6)
					textureSlot_6->setText(textures[6].c_str());
				if (textures.size() > 7)
					textureSlot_7->setText(textures[7].c_str());
			}

			//Shader flags
			auto flags1 = property->GetShaderFlags1_sk();
			for (int s = 0; s < 32; s++)
			{
				QString field_name = QString("SF1_%1").arg(s);
				auto field = SF1->findChild<QCheckBox*>(field_name, Qt::FindDirectChildrenOnly);
				int mask = 1 << s;
				if (flags1 & mask)
				{
					field->setChecked(true);
				}
				else {
					field->setChecked(false);
				}
			}
			auto flags2 = property->GetShaderFlags2_sk();
			for (int s = 0; s < 32; s++)
			{
				QString field_name = QString("SF2_%1").arg(s);
				auto field = SF2->findChild<QCheckBox*>(field_name, Qt::FindDirectChildrenOnly);
				int mask = 1 << s;
				if (flags2 & mask)
				{
					field->setChecked(true);
				}
				else {
					field->setChecked(false);
				}
			}
		}
	}
}

void TextureTool::on_viewModeCheckBox_toggled(bool checked)
{
	if (nullptr != _model)
	{
		if (checked)
			nifView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		else
			nifView->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	}
}

void TextureTool::on_nifView_selectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (viewModeCheckBox->isChecked())
	{
		populateValues(current);
	}
}

void TextureTool::SetShaderType(BSLightingShaderPropertyRef property)
{
	auto type = (BSLightingShaderPropertyShaderType)shaderTypeComboBox->currentIndex();
	LOGINFO << "Setting shader type: " << NifFile::shader_type_name(type) << endl;
	property->SetSkyrimShaderType(type);
}

void TextureTool::SetTextures(BSLightingShaderPropertyRef property)
{
	auto textureset = property->GetTextureSet()->GetTextures();
	if (!textureSlot_0->text().isEmpty())
	{
		LOGINFO << "Setting Diffuse: " << textureSlot_0->text().toUtf8().constData() << endl;
		textureset[0] = textureSlot_0->text().toUtf8().constData();
	}
	if (!textureSlot_1->text().isEmpty())
	{
		LOGINFO << "Setting Normal: " << textureSlot_1->text().toUtf8().constData() << endl;
		textureset[1] = textureSlot_1->text().toUtf8().constData();
	}
	if (!textureSlot_2->text().isEmpty())
	{
		LOGINFO << "Setting Emissive: " << textureSlot_2->text().toUtf8().constData() << endl;
		textureset[2] = textureSlot_2->text().toUtf8().constData();
	}
	if (!textureSlot_3->text().isEmpty())
	{
		LOGINFO << "Setting Height/Parallax: " << textureSlot_3->text().toUtf8().constData() << endl;
		textureset[3] = textureSlot_3->text().toUtf8().constData();
	}
	if (!textureSlot_4->text().isEmpty())
	{
		LOGINFO << "Setting Environment/Cube Map: " << textureSlot_4->text().toUtf8().constData() << endl;
		textureset[4] = textureSlot_4->text().toUtf8().constData();
	}
	if (!textureSlot_5->text().isEmpty())
	{
		LOGINFO << "Setting Greyscale/Metallic: " << textureSlot_5->text().toUtf8().constData() << endl;
		textureset[5] = textureSlot_5->text().toUtf8().constData();
	}
	if (!textureSlot_6->text().isEmpty())
	{
		LOGINFO << "Setting Subsurface Tint: " << textureSlot_6->text().toUtf8().constData() << endl;
		textureset[6] = textureSlot_6->text().toUtf8().constData();
	}
	if (!textureSlot_7->text().isEmpty())
	{
		LOGINFO << "Setting Backlight: " << textureSlot_7->text().toUtf8().constData() << endl;
		textureset[7] = textureSlot_7->text().toUtf8().constData();
	}
	property->GetTextureSet()->SetTextures(textureset);
}

void TextureTool::SetFlags(BSLightingShaderPropertyRef property)
{
	int flags_1 = 0;
	for (int s = 0; s < 32; s++)
	{
		QString field_name = QString("SF1_%1").arg(s);
		auto field = SF1->findChild<QCheckBox*>(field_name, Qt::FindDirectChildrenOnly);
		int mask = 1 << s;
		if (field->isChecked())
		{
			LOGINFO << "Setting into Flags 1: " << NifFile::skyrimFlags1_name(s) << endl;
			flags_1 |= mask;
		}
	}
	property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)flags_1);
	int flags_2 = 0;
	for (int s = 0; s < 32; s++)
	{
		QString field_name = QString("SF2_%1").arg(s);
		auto field = SF2->findChild<QCheckBox*>(field_name, Qt::FindDirectChildrenOnly);
		int mask = 1 << s;
		if (field->isChecked())
		{
			LOGINFO << "Setting into Flags 2: " << NifFile::skyrimFlags2_name(s) << endl;
			flags_2 |= mask;
		}
	}
	property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)flags_2);
}

void TextureTool::accept()
{
	bool setShaderType = shaderTypeCheckBox->isChecked();
	bool overrideTextures = texturesCheckBox->isChecked();
	bool setShaderFlags = shaderFlagsCheckBox->isChecked();
	bool viewMode = viewModeCheckBox->isChecked();

	if (!viewMode && (overrideTextures || setShaderFlags || viewMode))
	{
		auto selection = nifView->selectionModel()->selectedIndexes();
		if (!selection.empty())
		{
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Confirm", QString("Are you sure you want to modify %1 blocks?").arg(selection.size()),
				QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				LOGINFO << "Tool running" << endl;
				QModelIndex last_file_index = QModelIndex();
				for (auto& index : selection)
				{
					auto block = _model->block(index);
					LOGINFO << "Block: " << block->GetName() << endl;
					if (nullptr != block)
					{
						auto file = index.parent();		
						auto property = ckcmd::HKX::getProperty(block);

						if (nullptr != property)
						{
							if (setShaderType)
								SetShaderType(property);
							if (overrideTextures)
								SetTextures(property);
							if (setShaderFlags)
								SetFlags(property);
						}
						if (file != last_file_index)
						{
							if (last_file_index.isValid())
							{
								LOGINFO << "Saving file: " << _model->file(last_file_index).fileName << endl;
								_model->file(last_file_index).Save();
							}
							last_file_index = file;
						}
					} 
				}
				if (last_file_index.isValid())
				{
					LOGINFO << "Saving file: " << _model->file(last_file_index).fileName << endl;
					_model->file(last_file_index).Save();
				}
			}
		}
	}
}