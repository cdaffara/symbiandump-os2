# Microsoft Developer Studio Project File - Name="basicfunctest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=basicfunctest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "basicfunctest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "basicfunctest.mak" CFG="basicfunctest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "basicfunctest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "basicfunctest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "basicfunctest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(EPOCROOT)epoc32\release\tools\rel"
# PROP Intermediate_Dir "$(EPOCROOT)EPOC32\BUILD\EZLib2\ZLib\Test\Release"
# PROP Ignore_Export_Lib 0
MTL=midl.exe
# ADD CPP /nologo /Zp4 /O1 /Op /GX /I "$(EPOCROOT)epoc32\include" /D "__SYMBIAN32__" /D "__VC32__" /D "__TOOLS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "__SUPPORT_CPP_EXCEPTIONS__" /GF /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "kernel32.lib" "$(EPOCROOT)epoc32\release\winc\REL\zlib.lib" /nologo /subsystem:console /machine:IX86 /fixed:no /ignore:4089 /ignore:4005 /ignore:4098 /WARN:3

!ELSEIF  "$(CFG)" == "basicfunctest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(EPOCROOT)epoc32\release\tools\deb"
# PROP Intermediate_Dir "$(EPOCROOT)EPOC32\BUILD\EZLib2\ZLib\Test\Debug"
# PROP Ignore_Export_Lib 0
MTL=midl.exe
# ADD CPP /nologo /Zp4 /Zi /Od /GX /I "$(EPOCROOT)epoc32\include" /D "__SYMBIAN32__" /D "__VC32__" /D "__TOOLS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "__SUPPORT_CPP_EXCEPTIONS__" /FR /Fd"$(EPOCROOT)epoc32\release\tools\deb\basicfunctest.PDB" /GF /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "kernel32.lib" "$(EPOCROOT)epoc32\release\winc\DEB\zlib.lib" /nologo /subsystem:console /debug /machine:IX86 /fixed:no /ignore:4089 /ignore:4005 /ignore:4098 /WARN:3
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "basicfunctest - Win32 Release"
# Name "basicfunctest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\basicfunctest\src\Basicfunctest.cpp
DEP_CPP_BASIC=\
	"$(EPOCROOT)epoc32\include\e32def.h"\
	"$(EPOCROOT)epoc32\include\stdapis\zconf.h"\
	"$(EPOCROOT)epoc32\include\zconf.h"\
	"$(EPOCROOT)epoc32\include\zlib.h"\
	
# End Source File
# Begin Source File

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# End Target
# End Project
