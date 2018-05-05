# ck-cmd
command line helper for executing some Creation Kit/Engine commands. Based upon hkxcmd project by Figment,
available at https://github.com/figment/hkxcmd

Requires cmake to be built, from the top directory
```console
mkdir build
cd build
cmake -G "Visual Studio 15 2017" ..
```

Then use Visual studio to build the genrated ck-cmd solutions.
Requires a number of libraries not included due to licensing still to be sorted (see CMakeLists.txt for the list)
and the original Havok 2010.2.0 
