#include <core/MathHelper.h>

double rad2deg(double rad)
{
	return rad * 180.0 / M_PI;
}

double deg2rad(double deg)
{
	return deg * M_PI / 180.0;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // ...
	}
}

void sanitizeString(string& to_sanitize)
{
	replaceAll(to_sanitize, " ", "_s_");
	replaceAll(to_sanitize, "[", "_ob_");
	replaceAll(to_sanitize, "]", "_cb_");
}

void find_files(fs::path startingDir, string extension, vector<fs::path>& results) {
	if (!exists(startingDir) || !is_directory(startingDir)) return;
	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(startingDir))
	{
		if (is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == extension) {
			results.push_back(dirEntry.path().string());
		}
	}
}

fs::path relative_to(const fs::path& p, const fs::path& base) {
	fs::path in = p;
	fs::path out;
	while (in.has_parent_path() && in != base) { out = in.filename() / out; in = in.parent_path(); }
	return out;
}