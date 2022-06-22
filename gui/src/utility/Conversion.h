#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace ckcmd
{
	namespace HKX
	{
		class Conversion
		{
		public:

			static fs::path convertFbxAnimationToHkx(const fs::path& source_file, const fs::path& output_directory);

		};
	}
}