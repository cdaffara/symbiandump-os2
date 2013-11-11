# Microsoft Developer Studio Project File - Name="STATDesktop" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=STATDesktop - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "STATDesktop.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STATDesktop.mak" CFG="STATDesktop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STATDesktop - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "STATDesktop - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "STATDesktop"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "STATDesktop - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu".\inc\stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\inc" /I "\epoc32\tools\stat\headers" /I "..\common\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin\STATDesktop.exe"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Performing Custom Build Step on STATDesktop
PostBuild_Cmds=$(ProjDir)\UpdateDesktop.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "STATDesktop - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu".\inc\stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\inc" /I "\epoc32\tools\stat\headers" /I "..\common\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 WS2_32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"..\..\bin\STATDesktopd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "STATDesktop - Win32 Release"
# Name "STATDesktop - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common\src\INI.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\STATDesktop.cpp
# End Source File
# Begin Source File

SOURCE=.\STATDesktop.dep
# End Source File
# Begin Source File

SOURCE=.\STATDesktop.mak
# End Source File
# Begin Source File

SOURCE=.\src\statdesktop.rc
# End Source File
# Begin Source File

SOURCE=.\src\STATDesktopDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STATManageConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\inc\INI.h
# End Source File
# Begin Source File

SOURCE=.\inc\LogMessage.h
# End Source File
# Begin Source File

SOURCE=.\inc\MessageReporterImp.h
# End Source File
# Begin Source File

SOURCE=.\inc\resource.h
# End Source File
# Begin Source File

SOURCE=.\inc\ScriptProgressMonitorImp.h
# End Source File
# Begin Source File

SOURCE=.\inc\STATDesktop.h
# End Source File
# Begin Source File

SOURCE=.\inc\STATDesktopDlg.h
# End Source File
# Begin Source File

SOURCE=.\inc\STATManageConnection.h
# End Source File
# Begin Source File

SOURCE=.\inc\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\inc\WindowMessages.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\STATDesktop.ico
# End Source File
# Begin Source File

SOURCE=.\res\STATDesktop.rc2
# End Source File
# End Group
# End Target
# End Project
