// SPT.cpp : Defines the entry point for the console application.
//
//#include <dirent.h>
#include <SpeedTreeRT.h>
#include <string>
#include <vector>
#include <iostream>
#include <tchar.h>

//#include "Export.h"
//#include "Common.h"

bool IsSptFile(std::wstring const &fullString) 
{
  std::wstring suffix(L".spt");
  if (fullString.length() >= suffix.length()) 
  {
    return (0 == fullString.compare (fullString.length() - suffix.length(), suffix.length(), suffix));
  }
  else
  {
    return false;
  }
}

extern void ExportTree(CSpeedTreeRT* tree, std::wstring const& path);

bool ProcessTree(std::wstring const &sptFilePath)
{

  FILE *f = NULL;
  if (_wfopen_s(&f, sptFilePath.c_str(), L"rb"))
  {
    //std::cerr << "Failed to open: " << sptFilePath).c_str() << std::endl;
    return false;
  }
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  if (!size)
  {
    //std::wcerr << L"File corrupted: " << sptFilePath.c_str() << std::endl;
    fclose(f);
    return false;
  }
  rewind(f);
  unsigned char *buf = static_cast<unsigned char*>(malloc(size));
  fread(buf, sizeof(unsigned char), size, f);
  fclose(f);
  CSpeedTreeRT *tree = new CSpeedTreeRT;
  if (!tree->LoadTree(buf, static_cast<unsigned int>(size)))
  {
    //std::cerr << "Couldn't load the tree: " << w2a(sptFilePath).c_str() << std::endl;
    free(buf);
    delete tree;
  }

  // make sure SpeedTreeRT generates normals
  tree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
  tree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
  tree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);

  tree->Compute(0, tree->GetSeed());

  ExportTree(tree, sptFilePath);

  free(buf);
  delete tree;
  return true;
}

//int _tmain(int argc, _TCHAR* argv[])
//{
//  std::vector<std::wstring> sourcePaths;
//  if (argc == 1)
//  {
//    std::wstring path(argv[0]);
//    path = path.substr(0, path.find_last_of(L"\\/"));
//    std::cout << "Looking for SPTs in: " << w2a(path) << std::endl;
//    //ParseDir(path, sourcePaths);
//  }
//  else
//  {
//    for(int idx = 1; idx < argc; ++idx)
//    {
//      std::wstring path(argv[idx]);
//      if (IsSptFile(path))
//      {
//        sourcePaths.push_back(path);
//      }
//      else
//      {
//        std::cout << "Looking for SPTs in: " << w2a(path).c_str() << std::endl;
//        //ParseDir(path, sourcePaths);
//      }
//    }
//  }
//  
//
//  if (sourcePaths.empty())
//  {
//    std::wcerr << "No input! Provide a path to an SPT file or a directory containing SPTs" << std::endl;
//    system("pause");
//    return EXIT_FAILURE;
//  }
//
//  std::wcout << "Found " << sourcePaths.size() << " items" << std::endl;
//  for (int i = 0; i < sourcePaths.size(); ++i)
//  {
//    std::cout << i + 1 << "/" << sourcePaths.size() << ". ";
//    ProcessTree(sourcePaths[i]);
//  }
//  std::wcout << "Finished" << std::endl;
//  std::cout.flush();
//  std::system("pause");
//  return EXIT_SUCCESS;
//}

