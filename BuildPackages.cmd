@rem 
@rem PROJECT:   Mouri Internal Library Essentials
@rem FILE:      BuildPackages.cmd
@rem PURPOSE:   Build Packages
@rem 
@rem LICENSE:   The MIT License
@rem 
@rem DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
@rem 

@setlocal
@echo off

rem Change to the current folder.
cd "%~dp0Output"

rem Remove the output folder for a fresh compile.
del Mile.Windows.Helpers.nupkg

rem Build NuGet Package
D:\Tools\nuget.exe pack -OutputFileNamesWithoutVersion ..\Mile.Helpers\Mile.Windows.Helpers.nuspec

@endlocal