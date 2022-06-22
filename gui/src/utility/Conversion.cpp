#include <src/utility/Conversion.h>

using namespace ckcmd::HKX;

fs::path Conversion::convertFbxAnimationToHkx(const fs::path& source_file, const fs::path& output_directory)
{
	//todo
	return  output_directory / source_file.filename().replace_extension(".hkx");
}