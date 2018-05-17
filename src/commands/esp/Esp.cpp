#include <commands\esp\Esp.h>

Esp::Esp(std::string fName, fs::path fPath, bool fIsMaster)
	: name(fName), path(fPath), isMaster(fIsMaster)
{
	if (!isMaster)
	{
		CreatePlugin();
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
	EspWriter w(fPath + ext);

	esp->Write(w);
}

void Esp::SetDescription(const std::string desc)
{
	esp->GetHeader()->SetDesc(desc);
	esp->GetHeader()->UpdateSize();
}

void Esp::SetAuthor(const std::string author)
{
	esp->GetHeader()->SetAuthor(author);
	esp->GetHeader()->UpdateSize();
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
