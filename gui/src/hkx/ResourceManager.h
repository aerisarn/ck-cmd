#pragma once

#include <core/HKXWrangler.h>
#include <src/Log.h>

#include <map>

typedef std::pair< hkRootLevelContainer*, hkArray<hkVariant>> hkx_file_t;

namespace ckcmd {
	namespace HKX {
		class ResourceManager {

			std::map<fs::path, hkx_file_t> _files;
			const fs::path _workspace_folder;

		public:

			ResourceManager(const fs::path& workspace_folder);

			hkx_file_t& get(const fs::path& file);

			fs::path ResourceManager::open(const std::string& project);

			int findIndex(const fs::path& file, const void* object)
			{
				auto& _objects = _files[file].second;
				for (int i = 0; i < _objects.getSize(); i++) {
					if (_objects[i].m_object == object)
						return i;
				}
				return -1;
			}

			hkVariant* at(const fs::path& file, size_t index) {
				return &_files[file].second[index];
			}

		};
	}
}