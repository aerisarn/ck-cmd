#include <gtest/gtest.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Common/Base/hkBase.h>
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

#include <src/filetypes/projectfile.h>

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

fs::path test_path = "D:\\git\\ck-cmd\\gui\\test\\res";

TEST(GoogleTest, Init) { ASSERT_TRUE(true); }

TEST(Project, read) {
	fs::path project_path = test_path / "character" / "defaultmale.hkx";
	auto projectFile = new ProjectFile(NULL, project_path.string().c_str());
	ASSERT_TRUE(projectFile->parseBinary());
}

TEST(Project, readWrite) {
	fs::path project_path = test_path / "character" / "defaultmale.hkx";
	auto projectFile = new ProjectFile(NULL, project_path.string().c_str());
	ASSERT_TRUE(projectFile->parseBinary());
	projectFile->setFileName("test.hkx");
	projectFile->writeBinary();
}

int main(int argc, char **argv) {
	InitializeHavok();
	::testing::InitGoogleTest(&argc, argv);
	if(argc == 2) // gtest leaved unparsed arguments for you
		test_path = argv[1];
	auto result = RUN_ALL_TESTS();
	CloseHavok();
	return result;
}