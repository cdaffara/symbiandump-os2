# Microsoft Developer Studio Project File - Name="STATLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=STATLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "STATLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STATLib.mak" CFG="STATLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STATLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "STATLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "STATLib"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "STATLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W2 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\common\inc" /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\STAT.lib"

!ELSEIF  "$(CFG)" == "STATLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common\inc" /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\STATd.lib"

!ENDIF 

# Begin Target

# Name "STATLib - Win32 Release"
# Name "STATLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\cRepmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cReporter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cstatdataformatconverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cstatimageverify.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cstatlogfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cstatreturncodes.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\INI.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\statbitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STATComms.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STATEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\src\statexp.cpp
# End Source File
# Begin Source File

SOURCE=.\STATLib.dep
# End Source File
# Begin Source File

SOURCE=.\STATLib.mak
# End Source File
# Begin Source File

SOURCE=.\src\statlist.cpp
# End Source File
# Begin Source File

SOURCE=.\src\statmember.cpp
# End Source File
# Begin Source File

SOURCE=.\src\statscriptdecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\statserial.cpp
# End Source File
# Begin Source File

SOURCE=.\src\statsocket_block.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\cReporter.h
# End Source File
# Begin Source File

SOURCE=.\inc\cstatdataformatconverter.h
# End Source File
# Begin Source File

SOURCE=.\inc\cstatimageverify.h
# End Source File
# Begin Source File

SOURCE=.\inc\cstatlogfile.h
# End Source File
# Begin Source File

SOURCE=.\inc\cstatreturncodes.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\INI.h
# End Source File
# Begin Source File

SOURCE=.\inc\MessageReporter.h
# End Source File
# Begin Source File

SOURCE=.\inc\messagetypes.h
# End Source File
# Begin Source File

SOURCE=.\inc\ScriptProgressMonitor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\inc\SerialPacketSize.h
# End Source File
# Begin Source File

SOURCE=.\inc\stat.h
# End Source File
# Begin Source File

SOURCE=.\inc\statbitmap.h
# End Source File
# Begin Source File

SOURCE=.\inc\statcommon.h
# End Source File
# Begin Source File

SOURCE=.\inc\statcomms.h
# End Source File
# Begin Source File

SOURCE=.\inc\STATEngine.h
# End Source File
# Begin Source File

SOURCE=.\inc\statexp.h
# End Source File
# Begin Source File

SOURCE=.\inc\statlist.h
# End Source File
# Begin Source File

SOURCE=.\inc\statmember.h
# End Source File
# Begin Source File

SOURCE=.\inc\statscriptdecoder.h
# End Source File
# Begin Source File

SOURCE=.\inc\statserial.h
# End Source File
# Begin Source File

SOURCE=.\inc\statsocket_block.h
# End Source File
# Begin Source File

SOURCE=.\inc\stattransport.h
# End Source File
# Begin Source File

SOURCE=.\inc\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
