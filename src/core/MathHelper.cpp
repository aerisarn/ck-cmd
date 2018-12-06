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