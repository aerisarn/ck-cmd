#pragma once

#include <libbsa/libbsa.h>
#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace ckcmd {
namespace BSA {

	//TODO: exceptions
	class BSAFile {
		bsa_handle bh;
	public:

		BSAFile(const fs::path& p) {
			open(p);
		}

		BSAFile(const BSAFile& other) = delete;
		BSAFile(const BSAFile&& other) = delete;

		BSAFile(BSAFile&& other) {
			bh = other.bh;
			other.bh = NULL;
		}

		void open(const fs::path& p) {
			unsigned int ret = bsa_open(&bh, p.string().c_str());
		}

		void close() {
			bsa_close(bh);
		}

		const std::vector<std::string> assets(const std::string& regex = ".*") const {
			const char * const * assetPaths;
			size_t size = -1;
			bsa_get_assets(bh, regex.c_str(), &assetPaths, &size);
			return std::vector<std::string>(assetPaths, assetPaths + size);
		}

		bool find(const std::string& regex = "") const {
			bool result = false;
			bsa_contains_asset(bh, regex.c_str(), &result);
			return result;
		}

		const uint8_t * extract(const std::string& asset_path, size_t& size) const {
			const uint8_t* data;
			bsa_extract_asset_to_memory(bh, asset_path.c_str(), &data, &size);
			return data;
		}

		std::string extract(const std::string& asset_path) const {
			std::string load;
			size_t size;
			const uint8_t * content = extract(asset_path, size);
			load.assign((char*)content, (char*)content + size);
			delete content;
			return load;
		}

		~BSAFile() {
			bsa_close(bh);
		}

	};

}
}

