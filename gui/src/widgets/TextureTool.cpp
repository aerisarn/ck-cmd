#include "TextureTool.h"

#include <QFileDialog>

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
		}

		_model = std::make_shared< ckcmd::HKX::NIFTreeModel>(std_text);
		nifView->setModel(_model.get());
		nifView->setVisible(false);
		nifView->resizeColumnToContents(0);
		nifView->expandAll();
		nifView->setVisible(true);

		connect(nifView->selectionModel(), &QItemSelectionModel::currentChanged, this, &TextureTool::on_nifView_selectionChanged);
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
		auto property = DynamicCast<BSLightingShaderProperty>(block->GetShaderProperty());
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

void TextureTool::on_nifView_selectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (viewModeCheckBox->isChecked())
	{
		populateValues(current);
	}
}