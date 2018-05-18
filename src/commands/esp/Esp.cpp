#include "stdafx.h"
#include <commands\esp\Esp.h>
#include <core\hkxcmd.h>
#include <core\log.h>
#include <iostream>

Esp::Esp(std::string fName, fs::path fPath, bool fIsMaster)
	: name(fName), path(fPath), isMaster(fIsMaster)
{
	if (!isMaster)
	{
		Log::Info("Creating plugin %s.esp", std::string(path.string() + name).c_str());
		CreatePlugin();
	}
	else 
	{
		Log::Info("Creating master %s.esm", std::string(path.string() + name).c_str());
		CreateMaster();
	}
}

#include <core\games.h>
using namespace ckcmd::info;

Esp::Esp(std::string fName, bool fIsMaster)
	: name(fName), isMaster(fIsMaster)
{
	Games& games = Games::Instance();
	const Games::GamesPathMapT& installations = games.getGames();
	
	for (const auto& installation : installations)
	{
		const Games::Game& game = installation.first;
		if (game == Games::TES5 && !isMaster)
		{
			Log::Info("Skyrim installation detected.");
			path = installation.second / "Data\\";
			Log::Info("Creating plugin %s.esp", std::string(path.string() + name).c_str());
			CreatePlugin();
		}
		else if (game == Games::TES5)
		{
			Log::Info("Skyrim installation detected.");
			path = installation.second / "Data\\";
			Log::Info("Creating master %s.esm", std::string(path.string() + name).c_str());
			CreateMaster();
		}

		break;
	}
}

Esp::~Esp()
{
	delete esp;
}

void Esp::Save()
{
	std::string ext(".esp");
	if (isMaster) 
	{
		ext = ".esm";
	}

	std::string fPath = path.generic_string() + name;
	Log::Info("Attempting to write file...");
	
	try
	{
		EspWriter w(fPath + ext);
		esp->Write(w);
	}
	catch (...)
	{
		Log::Error("Unexpected error writing file!");
	}
}

void Esp::SetDescription(const std::string desc)
{
	esp->GetHeader()->SetDesc(desc);
	Log::Info("Description updated: \"%s\"", desc.c_str());
}

void Esp::SetAuthor(const std::string author)
{
	esp->GetHeader()->SetAuthor(author);
	Log::Info("Author updated: \"%s\"", author.c_str());
}

void Esp::CreatePlugin()
{
	EspFormHeader fileHeader('TES4', 16, 0x0, 0);
	EspTES4Hedr hedr(0.94f, 0, 0);
	EspUInt32 intv = 0;
	esp = new EspFile(fileHeader, hedr, intv);
}

void Esp::CreateMaster()
{
	EspFormHeader fileHeader('TES4', 16, 0x1, 0);
	EspTES4Hedr hedr(0.94f, 0, 0);
	EspUInt32 intv = 0;
	esp = new EspFile(fileHeader, hedr, intv);
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
		case hkxcmd::htShort: 
			Log::Info("CreateEsp - Create a TEST plugin file."); 
			break;
		case hkxcmd::htLong: {
			char fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_splitpath(fullName, NULL, NULL, exeName, NULL);
			Log::Info("Usage: %s about", exeName);
			Log::Info("  CreateEsp");
		}
		break;
	}
}

static bool ExecuteCmd(hkxcmdLine &cmdLine) {
	Esp esp("CmdTest", false);
	esp.SetAuthor("ck-cmd");
	esp.SetDescription("Test plugin file.");
	esp.Save();
	Log::Info("File saved.");
	return true;
}

REGISTER_COMMAND(CreateEsp, HelpString, ExecuteCmd);
