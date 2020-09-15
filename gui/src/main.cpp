#include "src/ui/mainwindow.h"
#include <QApplication>

#include <QColor>
#include <QPushButton>

#include <commands/CommandBase.h>

#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>


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
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
	InitializeHavok();
    //w.setStyleSheet("QWidget {background: blue;font-weight: bold; color: red}QComboBox {background: yellow}");
    w.setWindowTitle("Skyrim Behavior Tool");
    w.show();
	int res = a.exec();
	CloseHavok();
	return res;
}
