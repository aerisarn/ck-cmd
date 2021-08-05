#pragma once

#include <core/HKXWrangler.h>
#include <src/Log.h>

#include <map>

typedef std::pair< hkRootLevelContainer*, hkArray<hkVariant>> hkx_file_t;

namespace ckcmd {
	namespace HKX {
		class ResourceManager {

			std::vector<fs::path> _files;
			std::map<size_t, hkx_file_t> _contents;
			const fs::path _workspace_folder;

		public:

			ResourceManager(const fs::path& workspace_folder);

			hkx_file_t& get(const fs::path& file);

			fs::path ResourceManager::open(const std::string& project);

			size_t index(const fs::path& file) const;

			int findIndex(int file_index, const void* object) const;
			int findIndex(const fs::path& file, const void* object) const;

			hkVariant* at(const fs::path& file, size_t _index);

		};
	}
}