#pragma once

#include <iostream>
#include <cmath> 

#define _USE_MATH_DEFINES
#include <math.h>
#include <filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

double rad2deg(double rad);

double deg2rad(double deg);


void replaceAll(std::string& str, const std::string& from, const std::string& to);

void sanitizeString(string& to_sanitize);
string& unsanitizeString(string& to_unsanitize);

void find_files(fs::path startingDir, string extension, vector<fs::path>& results);

fs::path relative_to(const fs::path& p, const fs::path& base);

bool ends_with(std::string const & value, std::string const & ending);