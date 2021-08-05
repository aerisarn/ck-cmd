#include <gtest/gtest.h>
//
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#include <Common/Base/hkBase.h>
//#include <core/hkxcmd.h>
//#include <core/hkfutils.h>
//#include <core/log.h>
//
#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif
//
//#include <algorithm>
//#include <iterator>
//#include <string>
//#include <fstream>
//
//template<typename InputIterator1, typename InputIterator2>
//bool
//range_equal(InputIterator1 first1, InputIterator1 last1,
//	InputIterator2 first2, InputIterator2 last2)
//{
//	while (first1 != last1 && first2 != last2)
//	{
//		if (*first1 != *first2) return false;
//		++first1;
//		++first2;
//	}
//	return (first1 == last1) && (first2 == last2);
//}
//
//bool compare_files(const std::string& filename1, const std::string& filename2)
//{
//	std::ifstream file1(filename1);
//	std::ifstream file2(filename2);
//
//	std::istreambuf_iterator<char> begin1(file1);
//	std::istreambuf_iterator<char> begin2(file2);
//
//	std::istreambuf_iterator<char> end;
//
//	return range_equal(begin1, end, begin2, end);
//}
//
//
//#include <src/filetypes/projectfile.h>
//
////Havok initialization
//
//static void HK_CALL errorReport(const char* msg, void*)
//{
//	Log::Error("%s", msg);
//}
//
//static void HK_CALL debugReport(const char* msg, void* userContext)
//{
//	Log::Debug("%s", msg);
//}
//
//static hkThreadMemory* threadMemory = NULL;
//static char* stackBuffer = NULL;
//static void InitializeHavok()
//{
//	// Initialize the base system including our memory system
//	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
//	hkBaseSystem::init(pMemoryRouter, errorReport);
//	LoadDefaultRegistry();
//}
//
//static void CloseHavok()
//{
//	hkBaseSystem::quit();
//	hkMemoryInitUtil::quit();
//}
//
//
fs::path test_path = "D:\\skywind\\Data\\meshes\\actors";
//
//TEST(GoogleTest, Init) { ASSERT_TRUE(true); }
//
//TEST(Project, read) {
//	fs::path project_path = test_path / "character" / "defaultmale.hkx";
//	auto projectFile = new ProjectFile(NULL, project_path.string().c_str());
//	ASSERT_TRUE(projectFile->parseBinary());
//}
//
//template <typename T>
//void readWriteTest(const fs::path& file) {
//	auto havokFile = std::make_unique<T>((MainWindow*)NULL, file.string().c_str());
//	ASSERT_TRUE(havokFile->parseBinary());
//	havokFile->setFileName("test.hkx");
//	havokFile->writeBinary();
//	ASSERT_TRUE(compare_files(file.string(), "test.hkx"));
//	fs::remove("test.hkx");
//}
//
//TEST(Project, readWrite) {
//	readWriteTest<ProjectFile>(test_path / "character" / "defaultmale.hkx");
//}
//
//TEST(Config, Serialization) {
//	QSettings settings("prova.ini", QSettings::IniFormat);
//	settings.beginGroup("general");
//		settings.setValue("version", 1);
//		settings.setValue("workspace", test_path.string().c_str());
//	settings.endGroup();
//}

int main(int argc, char **argv) {
//	InitializeHavok();
	::testing::InitGoogleTest(&argc, argv);
	if(argc == 2) // gtest leaved unparsed arguments for you
		test_path = argv[1];
	auto result = RUN_ALL_TESTS();
//	CloseHavok();
	return result;
}