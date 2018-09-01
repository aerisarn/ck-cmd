#pragma once

#include <libbsa/libbsa.h>
#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace ckcmd {
namespace BSA {

	//TODO: exceptions
	class BSAFile {
		bsa_handle bh;
		fs::path path;
	public:

		BSAFile(const fs::path p) {
			open(p);
		}

		void open(const fs::path p) {
			unsigned int ret = bsa_open(&bh, p.string().c_str());
			path = p;
		}

		void close() {
			bsa_close(bh);
			path.clear();
		}

		const std::vector<std::string> assets(const std::string& regex = ".*") {
			const char * const * assetPaths;
			size_t size = -1;
			bsa_get_assets(bh, regex.c_str(), &assetPaths, &size);
			return std::vector<std::string>(assetPaths, assetPaths + size);
		}

		bool find(const std::string& regex = "") {
			bool result = false;
			bsa_contains_asset(bh, regex.c_str(), &result);
			return result;
		}

		const uint8_t * extract(const std::string& asset_path, size_t& size) {
			const uint8_t* data;
			bsa_extract_asset_to_memory(bh, asset_path.c_str(), &data, &size);
			return data;
		}

		~BSAFile() {
			bsa_close(bh);
		}

	};

}
}

