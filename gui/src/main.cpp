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


static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>

#include <stack>

int main(int argc, char *argv[])
{
	//fs::path in_tex = "C:\\Users\\omen\\Documents\\cinzia\\tesi\\Tesi-corretta-pre-note.tex";
	//if (!fs::exists(in_tex))
	//	throw 666;
	//std::stack<char> brackets;
	//std::ifstream t(in_tex.string());
	//std::stringstream buffer;
	//buffer << t.rdbuf();
	//std::string to_search = "\\textcolor[rgb]{0.5019608,0.5019608,0.5019608}";
	//std::string teplace_with_space = "\\textcolor[rgb]{0.7490196,0.0,0.25490198}{ }";
	//std::string teplace_with_space2 = "\\textcolor[rgb]{0.7490196,0.0,0.25490198}{ "; std::string teplace_actual = " \\textcolor[rgb]{0.7490196,0.0,0.25490198}{";
	//std::string original = buffer.str();
	//std::string new_file = "";

	//bool matching;
	//int last_size = -1;

	//for (int i = 0; i < original.size(); i)
	//{
	//	if (i + to_search.size() < original.size() &&
	//		original.substr(i, to_search.size()) == to_search)
	//	{
	//		last_size = brackets.size();
	//		brackets.push('{');
	//		i += to_search.size() + 1;
	//		continue;
	//	}
	//	if (i + teplace_with_space.size() < original.size() &&
	//		original.substr(i, teplace_with_space.size()) == teplace_with_space)
	//	{
	//		i += teplace_with_space.size();
	//		new_file += " ";
	//		continue;
	//	}
	//	if (i + teplace_with_space2.size() < original.size() &&
	//		original.substr(i, teplace_with_space2.size()) == teplace_with_space2)
	//	{
	//		i += teplace_with_space2.size();
	//		brackets.push('{');
	//		new_file += teplace_actual;
	//		continue;
	//	}

	//	if (original[i] == '{')
	//	{
	//		brackets.push(original[i]);
	//	}
	//	bool addspace = false;
	//	if (original[i] == '}')
	//	{
	//		brackets.pop();
	//		if (i + 1 < original.size() && std::isalpha(original[i + 1]) && !std::isspace(original[i + 1]))
	//			addspace = true;
	//		if (brackets.size() == last_size)
	//		{
	//			last_size = -1;
	//			if (addspace)
	//				new_file += " ";
	//			i++;
	//			continue;
	//		}
	//	}
	//	new_file += original[i];
	//	if (addspace)
	//		new_file += " ";
	//	i++;
	//}

	//std::ofstream out(in_tex.parent_path() / "Tesi-corretta-nuova.tex");
	//out << new_file;
	//out.close();

	//return 0;


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

	MainWindow w(InitializeHavok());
	w.show();
	int res = a.exec();
	CloseHavok();
	return res;
}
