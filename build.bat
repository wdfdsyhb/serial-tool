@echo off
REM 用 MSVC x64 编译 serial-tool（需已装 VS，路径不对就改下面一行）
call D:\VS\VC\Auxiliary\Build\vcvarsall.bat x64
cl /nologo /W4 /O2 /utf-8 /D_CRT_SECURE_NO_WARNINGS /Fe:serial-tool.exe src\main.c src\devices.c
