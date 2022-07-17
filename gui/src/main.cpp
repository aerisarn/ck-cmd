#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>

#include "windows/mainwindow.h"

#include <QApplication>
#include <QColor>
#include <QPushButton>
#include <QMessageBox>
#include <QTranslator>

#include <src/windows/WorkspaceLauncher.h>
#include <src/windows/SkyrimSelector.h>

#include <commands/CommandBase.h>
#include <core/games.h>
#include <core/bsa.h>
#include <src/log.h>

#include "src/config.h"



//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}

static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static hkMemoryRouter* InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
	return pMemoryRouter;
}

static void extractAssets() {
	std::string workspace_path = Settings.get<std::string>("/general/workspace_folder");
	std::string data_path = Settings.get<std::string>("/general/skyrim_le_folder");

	std::vector<std::string> bsa_files = {
		"Skyrim - Animations.bsa",
		"Dawnguard.bsa",
		"Dragonborn.bsa",
		"Update.bsa",
	};

	for (auto& bsa_file : bsa_files)
	{
		fs::path bsa_path = fs::path(data_path) / bsa_file;

		if (fs::exists(bsa_path) &&
			fs::is_regular_file(bsa_path))
		{
			ckcmd::BSA::BSAFile bsa_file(bsa_path);
			auto havok_files = bsa_file.assets("^(.*)\\.hkx$");
			auto txt_files = bsa_file.assets("^(.*)\\.txt$");
			std::vector<std::string> to_estract;
			to_estract.reserve(havok_files.size() + txt_files.size());
			to_estract.insert(to_estract.end(), havok_files.begin(), havok_files.end());
			to_estract.insert(to_estract.end(), txt_files.begin(), txt_files.end());
			for (const auto& file : to_estract) {
				size_t size = -1;
				const uint8_t* data = bsa_file.extract(file, size);
				fs::path relative = fs::relative(fs::path(file), "meshes");
				fs::path output_path = fs::path(workspace_path) / relative;
				fs::create_directories(output_path.parent_path());
				std::ofstream(output_path.string(), std::ios::binary).write((const char *)data, size);
				delete data;
			}
		}
	}
}

static void extractIDLE() {
	std::string workspace_path = Settings.get<std::string>("/general/workspace_folder");
	std::string data_path = Settings.get<std::string>("/general/skyrim_le_folder");

	if (fs::exists(fs::path(data_path) / "creatures.esp"))
	{
		fs::remove(fs::path(data_path) / "creatures.esp");
	}

	Collection skyrimCollection = Collection((char* const)data_path.c_str(), 3);
	ModFlags masterFlags = ModFlags(0xA);
	masterFlags.IsInLoadOrder = true;
	ModFile* update = NULL;
	ModFile* skyrim = skyrimCollection.AddMod("Skyrim.esm", masterFlags);
	if (fs::exists(fs::path(data_path) / "Update.esm"))
		update = skyrimCollection.AddMod("Update.esm", masterFlags);
	ModFile* dragonborn = NULL;
	if (fs::exists(fs::path(data_path) / "Update.esm"))
		dragonborn = skyrimCollection.AddMod("Dragonborn.esm", masterFlags);
	ModFlags espFlags = ModFlags(0x1818);
	espFlags.IsNoLoad = false;
	espFlags.IsFullLoad = true;
	espFlags.IsMinLoad = false;
	espFlags.IsCreateNew = true;
	ModFile* creatures = skyrimCollection.AddMod("creatures.esp", espFlags);

	char* argvv[4];
	argvv[0] = new char();
	argvv[1] = new char();
	argvv[2] = new char();
	argvv[3] = new char();
	logger.init(4, argvv);

	skyrimCollection.Load();

	std::set<Sk::IDLERecord*> idles;
	std::set<Sk::AACTRecord*> aacts;

	for (auto idle_record_it = skyrimCollection.FormID_ModFile_Record.begin(); idle_record_it != skyrimCollection.FormID_ModFile_Record.end(); idle_record_it++)
	{
		Record* record = idle_record_it->second;
		if (record->GetType() == REV32(IDLE)) {
			ModFile* winningMod;
			Record* winningRecord;
			skyrimCollection.LookupWinningRecord(record->formID, winningMod, winningRecord);
			idles.insert(dynamic_cast<Sk::IDLERecord*>(winningRecord));
		}
		if (record->GetType() == REV32(AACT)) {
			ModFile* winningMod;
			Record* winningRecord;
			skyrimCollection.LookupWinningRecord(record->formID, winningMod, winningRecord);
			aacts.insert(dynamic_cast<Sk::AACTRecord*>(winningRecord));
		}
	}

	//need these to full form trees
	for (auto aact_record_it = aacts.begin(); aact_record_it != aacts.end(); aact_record_it++)
	{
		Sk::AACTRecord* aact = *aact_record_it;
		Record* to_copy = static_cast<Record*>(aact);

		Record* result = skyrimCollection.CopyRecord(to_copy, creatures, NULL, NULL, aact->EDID.value, createFlags::fSetAsOverride | createFlags::fCopyWinningParent);
		result->IsChanged(true);
		Sk::AACTRecord* copied = dynamic_cast<Sk::AACTRecord*>(result);
		result->IsChanged(true);
		copied->EDID.value = aact->EDID.value;
		copied->CNAM.value = aact->CNAM.value;

		result->IsChanged(true);
		result->IsLoaded(true);
		FormIDMasterUpdater checker(creatures->FormIDHandler);
		checker.Accept(result->formID);
	}

	std::set< Sk::IDLERecord*> added;

	////IDLE are full of errors, we do a first pass to check and categorize
	for (auto idle_record_it = idles.begin(); idle_record_it != idles.end(); idle_record_it++)
	{
		Sk::IDLERecord* idle = *idle_record_it;
		std::string this_behavior = "";
		std::string edid = "";

		if (!idle->DNAM.IsLoaded())
			idle->DNAM.Load();

		if (NULL != idle->DNAM.value)
			this_behavior = idle->DNAM.value;

		if (!idle->EDID.IsLoaded())
			idle->EDID.Load();

		if (idle->EDID.value == NULL)
		{
			edid = "Unnamed";
		}
		else {
			edid = idle->EDID.value;
		}

		//counter check behavior with parent and sibling
		if (!idle->ANAM.IsLoaded())
			idle->ANAM.Load();


		std::string parent_behavior = "";
		std::string sibling_behavior = "";
		auto parent = idle->ANAM.value.parent;
		Sk::IDLERecord* parent_idle = NULL;
		auto find_parent_it = skyrimCollection.FormID_ModFile_Record.find(parent);
		if (find_parent_it != skyrimCollection.FormID_ModFile_Record.end())
		{
			auto parent_record = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
			if (NULL != parent_record && REV32(IDLE) == parent_record->GetType())
			{
				parent_idle = dynamic_cast<Sk::IDLERecord*>(parent_record);
				if (!parent_idle->DNAM.IsLoaded())
					parent_idle->DNAM.Load();

				if (NULL != parent_idle->DNAM.value)
					parent_behavior = parent_idle->DNAM.value;
			}
		}
		auto sibling = idle->ANAM.value.sibling;
		auto find_sibling_it = skyrimCollection.FormID_ModFile_Record.find(sibling);
		if (find_sibling_it != skyrimCollection.FormID_ModFile_Record.end())
		{
			auto sibling_record = dynamic_cast<Sk::IDLERecord*>(find_sibling_it->second);
			if (NULL != sibling_record && REV32(IDLE) == sibling_record->GetType())
			{
				Sk::IDLERecord* sibling_idle = dynamic_cast<Sk::IDLERecord*>(sibling_record);
				if (!sibling_idle->DNAM.IsLoaded())
					sibling_idle->DNAM.Load();

				if (NULL != sibling_idle->DNAM.value)
					sibling_behavior = sibling_idle->DNAM.value;
			}
		}

		if (!parent_behavior.empty())
		{
			if (this_behavior.empty())
				this_behavior = parent_behavior;
			else if (!this_behavior.empty() && 
				this_behavior != parent_behavior)
			{	
				if (!sibling_behavior.empty())
				{
					//two different behaviors between parent and childer, check the sibling
					if (sibling_behavior == parent_behavior)
						this_behavior = parent_behavior;
				}
				else {
					//check parent's parent to be sure
					std::string parent_parent_behavior;
					if (parent_idle != NULL)
					{
						auto find_parent_it = skyrimCollection.FormID_ModFile_Record.find(parent_idle->ANAM.value.parent);
						if (find_parent_it != skyrimCollection.FormID_ModFile_Record.end())
						{
							auto parent_record = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
							if (NULL != parent_record && REV32(IDLE) == parent_record->GetType())
							{
								parent_idle = dynamic_cast<Sk::IDLERecord*>(parent_record);
								if (!parent_idle->DNAM.IsLoaded())
									parent_idle->DNAM.Load();

								if (NULL != parent_idle->DNAM.value)
									parent_parent_behavior = parent_idle->DNAM.value;
							}
						}
					}
					if (parent_behavior == parent_parent_behavior)
						this_behavior = parent_behavior;
					else if (parent_parent_behavior.empty())
						this_behavior = parent_behavior;
				}
			}
		}
		if (this_behavior.empty() && !sibling_behavior.empty())
		{
			this_behavior = sibling_behavior;
		}

		while (this_behavior.empty() && parent_idle != NULL)
		{
			auto find_parent_it = skyrimCollection.FormID_ModFile_Record.find(parent_idle->ANAM.value.parent);
			if (find_parent_it != skyrimCollection.FormID_ModFile_Record.end())
			{
				auto parent_record = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
				if (NULL != parent_record && REV32(IDLE) == parent_record->GetType())
				{
					parent_idle = dynamic_cast<Sk::IDLERecord*>(parent_record);
					if (!parent_idle->DNAM.IsLoaded())
						parent_idle->DNAM.Load();

					if (NULL != parent_idle->DNAM.value)
						this_behavior = parent_idle->DNAM.value;
				}
			}
			else {
				break;
			}
			parent_idle = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
		}

		if (idle->EDID.value != NULL && 
			(
					strcmp(idle->EDID.value, "SabreCatTurnToRun") == 0 ||
					strcmp(idle->EDID.value, "SabreCatIdleToRunLeft90") == 0 ||
					strcmp(idle->EDID.value, "SabreCatIdleToRunLeft180") == 0 ||
					strcmp(idle->EDID.value, "SabreCatIdleToRunRight90") == 0 ||
					strcmp(idle->EDID.value, "SabreCatIdleToRunRight180") == 0
			)
		)
		{
			this_behavior = "Actors\\SabreCat\\Behaviors\\SabreCatBehavior.hkx";
		}

		std::string event = "";
		if (!idle->ENAM.IsLoaded())
			idle->ENAM.Load();
		if (idle->ENAM.value != NULL)
			event = idle->ENAM.value;

		if (this_behavior.empty())
			__debugbreak();

		Record* to_copy = static_cast<Record*>(idle);

		// declaring character array 
		char* char_array = new char[edid.size() + 1];

		// copying the contents of the 
		// string to char array 
		strcpy(char_array, edid.c_str());

		Record* result = skyrimCollection.CopyRecord(to_copy, creatures, NULL, NULL, char_array, createFlags::fSetAsOverride | createFlags::fCopyWinningParent);
		result->IsChanged(true);
		Sk::IDLERecord* copied = dynamic_cast<Sk::IDLERecord*>(result);
		added.insert(copied);
		result->IsChanged(true);
		copied->EDID.value = idle->EDID.value;
		copied->ANAM = idle->ANAM;
		copied->CTDA = idle->CTDA;
		copied->DATA = idle->DATA;
		copied->ENAM = idle->ENAM;

		char* behavior_char = new char[this_behavior.size() + 1];
		strcpy(behavior_char, this_behavior.c_str());

		copied->DNAM.value = behavior_char;

		result->IsChanged(true);
		result->IsLoaded(true);
		FormIDMasterUpdater checker(creatures->FormIDHandler);
		checker.Accept(result->formID);
	}

	ModSaveFlags skSaveFlags = ModSaveFlags(2);
	skSaveFlags.IsCleanMasters = true;
	string esp_name = "creatures.esp";
	skyrimCollection.SaveMod(creatures, skSaveFlags, (char* const)esp_name.c_str());

	creatures->Close();
	if (NULL != dragonborn)
		dragonborn->Close();
	if (NULL != update)
		update->Close();
	skyrim->Close();

	if (fs::exists(fs::path(data_path) / esp_name))
	{
		fs::copy_file(fs::path(data_path) / esp_name, fs::path(workspace_path) / esp_name);
	}
	else {
		QMessageBox::tr("Unable to assemble the IDLE plugin. Creature cache won't be generated properly");
	}
}


static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	std::string workspace_path = Settings.get<std::string>("/general/workspace_folder");
	if (workspace_path.empty() || !fs::exists(workspace_path))
	{
		//Select workspace dialog
		WorkspaceLauncher launcher;
		if (launcher.exec() != QDialog::Accepted) {
			return 1; // Error, for example
		}
		Settings.set("/general/workspace_folder", launcher._workspace_folder);
		workspace_path = launcher._workspace_folder.toStdString();
	}

	//Check if the workspace is empty
	if (fs::is_empty(workspace_path)) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			NULL, 
			QMessageBox::tr("Workspace empty"), 
			QMessageBox::tr("The selected workspace is empty, would you like to unpack vanilla LE resources?"),
			QMessageBox::Yes | QMessageBox::No
		);
		if (reply == QMessageBox::Yes) {
			fs::path data_path;
			auto& games = Games::Instance();
			if (!games.isGameInstalled(Games::Game::TES5)) {
				//Select skyrim dialog				
				while (!fs::exists(data_path / "Skyrim - Animations.bsa") 
					|| !fs::is_regular_file(data_path / "Skyrim - Animations.bsa"))
				{
					SkyrimSelector selector;
					if (selector.exec() != QDialog::Accepted) {
						return 1; // Error, for example
					}
					data_path = selector._skyrim_folder.toStdString();
					data_path /= "Data";
				}
				
			}
			else {
				data_path = games.data(Games::Game::TES5);
			}
			Settings.set("/general/skyrim_le_folder", data_path.string().c_str());
			extractAssets();		
		}
		else {
			//NTD
		}
	} 

	if (!fs::exists(fs::path(workspace_path) / "creatures.esp"))
	{
		fs::path data_path = Settings.get<std::string>("/general/skyrim_le_folder");
		if (fs::exists(data_path))
		{
			extractIDLE();
		}
	}

	MainWindow w(InitializeHavok());
	w.show();
	int res = a.exec();
	CloseHavok();
	return res;
}
