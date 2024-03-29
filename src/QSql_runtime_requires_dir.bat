@echo off
set srcFolder=D:\QT\6.5.3\msvc2019_64\plugins\
set targetFolder=D:\WORK\SynergySpot\src\cmake-build-debug\Debug\plugins\

echo Copy Folder %srcFolder% to %targetFolder% ...

xcopy "%srcFolder%" "%targetFolder%" /s/y

echo Folder copy successfully