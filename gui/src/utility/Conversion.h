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
		typedef std::map<float, std::string> AnimationEvents;

		class Conversion
		{
		public:

			static AnimationInfo convertFbxAnimationToHkx(
				const fs::path& skeleton_file,
				const fs::path& source_file,
				const fs::path& output_directory
			);

			static bool convertHkxAnimationToFBX
			(
				const fs::path& skeleton_file,
				const fs::path& source_file,
				const fs::path& output_directory,
				const AnimData::root_movement_t& movements
			);

			static bool convertHkxSkeletonToFbx(
				const fs::path& skeleton_file,
				const fs::path& output_directory
			);

		};
	}
}