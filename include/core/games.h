#pragma once

#include "stdafx.h"

#include <map>
#include <core/bsa.h>
#include <fstream>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

namespace ckcmd {
	namespace info {

		class Games {
		public:
			enum Game
			{
				TES3,
				TES4,
				TES5,
				TES5SE,
				TES5VR,
				FO3,
				FONV,
				FO4,
				INVALID = -1
			};

			static bool isGameSupported(const TCHAR* keyName) {
				return
					!_tcscmp(keyName, _T("Fallout3")) ||
					!_tcscmp(keyName, _T("Fallout4")) ||
					!_tcscmp(keyName, _T("falloutnv")) ||
					!_tcscmp(keyName, _T("oblivion")) ||
					!_tcscmp(keyName, _T("Oblivion")) ||
					!_tcscmp(keyName, _T("skyrim")) ||
					!_tcscmp(keyName, _T("Skyrim Special Edition"));
			}

			static const std::string string(const Game& game) {
				switch (game) {
					case TES3: return "TES3";
					case TES4: return "TES4";
					case TES5: return "TES5";
					case TES5SE: return "TES5SE";
					case TES5VR: return "TES5VR";
					case FO3: return "FO3";
					case FONV: return "FONV";
					case FO4: return "FO4";
					return "INVALID";
				}
			}

			const inline bool isGameInstalled(const Game& game) {
				return pathMaps.find(game) != pathMaps.end() &&
					fs::is_directory(pathMaps.at(game));
			}

			const inline fs::path data(const Game& game) {
				return pathMaps.at(game) / "Data";
			}

			const inline std::list<fs::path> data_files(const Game& game,  const std::string& extension = "") {
				std::list<fs::path> list;
				if (isGameInstalled(game))
				{
					for (auto& file : fs::directory_iterator(data(game))) {
						if (file.path().has_extension() && !file.path().extension().compare(extension))
							list.push_back(file);
					}
				}
				return list;
			}

			const inline std::list<fs::path> bsas(const Game& game) {
				return data_files(game,".bsa");
			}

			const inline std::list<fs::path> esms(const Game& game) {
				return data_files(game, ".esm");
			}

			const inline std::list<fs::path> esps(const Game& game) {
				return data_files(game, ".esp");
			}

			const inline fs::path override(const Game& game) {
				return data(game);
			}

			Game from_string(const TCHAR* keyName) {
				if (!_tcscmp(keyName, _T("Fallout3")))
					return FO3;
				if (!_tcscmp(keyName, _T("Fallout4")))
					return FO4;
				if (!_tcscmp(keyName, _T("falloutnv")))
					return FONV;
				if (!_tcscmp(keyName, _T("oblivion")))
					return TES4;
				if (!_tcscmp(keyName, _T("Oblivion")))
					return TES4;
				if (!_tcscmp(keyName, _T("skyrim")))
					return TES5;
				if (!_tcscmp(keyName, _T("Skyrim Special Edition")))
					return TES5SE;
				return INVALID;
			}

			const inline void loadBsas(const Game& game)
			{
				for (const auto& bsa_filename : bsas(game))
				{
					opened_bsas.push_back(std::move(ckcmd::BSA::BSAFile(bsa_filename)));
				}
			}

			const std::list<ckcmd::BSA::BSAFile>& bsa_files()
			{
				return opened_bsas;
			}

			void load(const fs::path& path, std::vector<uint8_t>& result) {
				std::ifstream fss(path.c_str());
				//allocate
				fss.seekg(0, std::ios::end);
				result.reserve(static_cast<size_t>(fss.tellg()));
				//reset and assign
				fss.seekg(0, std::ios::beg);
				result.assign(std::istreambuf_iterator<char>(fss),
					std::istreambuf_iterator<char>());
				fss.close();
			}

			bool load(const Games::Game& game, const std::string& path, std::vector<uint8_t>& result) {
				//search in override
				fs::path override_path = override(game) / path;
				if (fs::exists(override_path) && fs::is_regular_file(override_path))
				{
					load(override_path, result);
					return true;
				}
				if (opened_bsas.empty())
					loadBsas(game);
				for (auto& bsa_file : opened_bsas) {
					if (bsa_file.find(path)) {
						size_t size = -1;
						const uint8_t* data = bsa_file.extract(path, size);
						result.resize(size);
						result.assign(data, data+size);
						delete data;
						return true;
					}
				}
				return false;
			}

			typedef std::map<Game, fs::path> GamesPathMapT;

		private:

			GamesPathMapT pathMaps;

			std::list<ckcmd::BSA::BSAFile> opened_bsas;

			LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue)
			{
				strValue;
				WCHAR szBuffer[512];
				DWORD dwBufferSize = sizeof(szBuffer);
				ULONG nError;
				nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
				if (ERROR_SUCCESS == nError)
				{
					strValue = szBuffer;
				}
				return nError;
			}

			void QueryKey(HKEY hKey, const TCHAR* keyName = NULL)
			{
				TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
				DWORD    cbName;                   // size of name string 
				TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
				DWORD    cchClassName = MAX_PATH;  // size of class string 
				DWORD    cSubKeys = 0;               // number of subkeys 
				DWORD    cbMaxSubKey;              // longest subkey size 
				DWORD    cchMaxClass;              // longest class string 
				DWORD    cValues;              // number of values for key 
				DWORD    cchMaxValue;          // longest value name 
				DWORD    cbMaxValueData;       // longest value data 
				DWORD    cbSecurityDescriptor; // size of security descriptor 
				FILETIME ftLastWriteTime;      // last write time 

				DWORD i, retCode;

				TCHAR  achValue[MAX_VALUE_NAME];
				DWORD cchValue = MAX_VALUE_NAME;

				// Get the class name and the value count. 
				retCode = RegQueryInfoKey(
					hKey,                    // key handle 
					achClass,                // buffer for class name 
					&cchClassName,           // size of class string 
					NULL,                    // reserved 
					&cSubKeys,               // number of subkeys 
					&cbMaxSubKey,            // longest subkey size 
					&cchMaxClass,            // longest class string 
					&cValues,                // number of values for this key 
					&cchMaxValue,            // longest value name 
					&cbMaxValueData,         // longest value data 
					&cbSecurityDescriptor,   // security descriptor 
					&ftLastWriteTime);       // last write time 

											 // Enumerate the subkeys, until RegEnumKeyEx fails.

				if (cSubKeys)
				{
					for (i = 0; i < cSubKeys; i++)
					{
						cbName = MAX_KEY_LENGTH;
						retCode = RegEnumKeyEx(hKey, i,
							achKey,
							&cbName,
							NULL,
							NULL,
							NULL,
							&ftLastWriteTime);
						if (retCode == ERROR_SUCCESS)
						{
							if (isGameSupported(achKey))
							{
								HKEY hTestKey;
								if (RegOpenKeyEx(hKey, achKey, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
									QueryKey(hTestKey, achKey);
							}
						}
					}
				}

				// Enumerate the key values. 

				if (cValues)
				{
					for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
					{
						cchValue = MAX_VALUE_NAME;
						achValue[0] = '\0';
						retCode = RegEnumValue(hKey, i,
							achValue,
							&cchValue,
							NULL,
							NULL,
							NULL,
							NULL);

						if (retCode == ERROR_SUCCESS)
						{
							Game thisGame = from_string(keyName);
							if (thisGame != INVALID && !_tcscmp(achValue, _T("installed path"))) {
								const std::wstring strValueName = L"installed path";
								std::wstring strValue = L"";
								retCode = GetStringRegKey(hKey, strValueName, strValue);
								if (retCode == ERROR_SUCCESS)
									pathMaps[thisGame] = fs::path(strValue);
							} else if (thisGame != INVALID && !_tcscmp(achValue, _T("Installed Path"))) {
								const std::wstring strValueName = L"Installed Path";
								std::wstring strValue = L"";
								retCode = GetStringRegKey(hKey, strValueName, strValue);
								if (retCode == ERROR_SUCCESS)
									pathMaps[thisGame] = fs::path(strValue);
							}
						}
					}
				}
			}

		public:
			static Games& Instance() {
				// it **is** thread-safe in C++11.
				static Games myInstance;

				// Return a reference to our instance.
				return myInstance;
			}

			const GamesPathMapT& getGames() {
				return pathMaps;
			}

			// delete copy and move constructors and assign operators
			Games(Games const&) = delete;             // Copy construct
			Games(Games&&) = delete;                  // Move construct
			Games& operator=(Games const&) = delete;  // Copy assign
			Games& operator=(Games &&) = delete;      // Move assign

		protected:
			Games() {
				//Open the registry and search for Creation Engine game. More or less
				HKEY hTestKey;

				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Bethesda Softworks"), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS ||
					RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\WOW6432Node\\Bethesda Softworks"), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
				{
					QueryKey(hTestKey);
					RegCloseKey(hTestKey);
				}
			}

			~Games() {
			}

		};

	}
}