# ck-cmd

[![Build status](https://ci.appveyor.com/api/projects/status/pjtq91xk722dksny/branch/master?svg=true)](https://ci.appveyor.com/project/aerisarn/ck-cmd/branch/master)

Command line helper for executing some Creation Kit/Engine commands. Based upon hkxcmd project by Figment,
available at https://github.com/figment/hkxcmd

## Building

Requires CMake to be built.
##### Cloning the repo
```console
git clone https://github.com/aerisarn/ck-cmd.git ck-cmd
cd ck-cmd
```
##### Downloading modules: 
```console
git submodule update --init --recursive
```
##### Downloading libraries: 
Put all the libraries into ``rootDir/lib``
The required libraries are 
- bs : 
- bsa : [GitHub](https://github.com/Ortham/libbsa "GitHub")
- DirectXTex : [GitHub](https://github.com/Microsoft/DirectXTex/tree/master/DirectXTex "GitHub")
- fbx : [GitHub](https://github.com/jskorepa/fbx/tree/master/src "GitHub")
- loadorder : 
- loki : [Website](http://loki-lib.sourceforge.net/index.php?n=Main.Download "Website")
- nif : [GitHub](https://github.com/niftools/niflib "GitHub")
- xedit : [GitHub](https://github.com/matortheeternal/xedit-lib "GitHub")
- zlib : [GitHub](https://github.com/TES5Edit/TES5Edit/tree/dev/zlib "GitHub")

The expected file structure is [here](https://privatebin.net/?103e41356959809a#WAHHYkhZyGOV7c5tCmpaEHTzm1b458Z9KHWzjpVv5Vo= "here").

##### Running CMake:
```console
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A Win32 ..
```
Then use Visual studio to build the generated ck-cmd solutions (ck-cmd.sln)
