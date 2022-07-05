#pragma once

#include <RootMovement.h>
#include <filesystem>
#include <utility>

namespace fs = std::filesystem;

namespace ckcmd
{
	namespace HKX
	{
		typedef std::pair<AnimData::root_movement_t, fs::path> AnimationInfo;

		class Conversion
		{
		public:

			static AnimationInfo convertFbxAnimationToHkx(
				const fs::path& skeleton_file,
				const fs::path& source_file,
				const fs::path& output_directory
			);

		};
	}
}