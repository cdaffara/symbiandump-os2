# Microsoft Developer Studio Generated NMAKE File, Based on STATdll.dsp
!IF "$(CFG)" == ""
CFG=STATDll - Win32 Release
!MESSAGE No configuration specified. Defaulting to STATDll - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "STATDll - Win32 Release" && "$(CFG)" != "STATDll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STATdll.mak" CFG="STATDll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STATDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "STATDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "STATDll - Win32 Release"

OUTDIR=\epoc32\tools\stat
INTDIR=.\Release

ALL : "..\..\bin\STAT.dll" ".\Release\STATdll.pch" ".\Release\STATdll.bsc"


CLEAN :
	-@erase "$(INTDIR)\STATdll.pch"
	-@erase "$(INTDIR)\statdll.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\bin\STAT.dll"
	-@erase "..\..\lib\STATDll.exp"
	-@erase "..\..\lib\STATDll.lib"
	-@erase ".\Release\STATdll.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATdll.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\statdll.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"Release/STATdll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\StdAfx.sbr"

".\Release\STATdll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib ..\..\lib\Stat.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /nodefaultlib:"libcmtd.lib" /def:".\stat.def" /out:"..\..\bin\STAT.dll" /implib:"..\..\lib\STATDll.lib" /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\statdll.res"

"..\..\bin\STAT.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Copying browse data...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\bin\STAT.dll" ".\Release\STATdll.pch" ".\Release\STATdll.bsc"
   .\UpdateDll.bat
	copy release\statdll.bsc \epoc32\tools\stat\statdll.bsc
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "STATDll - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\bin\STATd.dll" "$(OUTDIR)\STATdll.pch" "$(OUTDIR)\STATdll.bsc"


CLEAN :
	-@erase "$(INTDIR)\STATdll.pch"
	-@erase "$(INTDIR)\statdll.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\STATd.pdb"
	-@erase "$(OUTDIR)\STATdll.bsc"
	-@erase "..\..\bin\STATd.dll"
	-@erase "..\..\lib\STATDlld.exp"
	-@erase "..\..\lib\STATDlld.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\inc" /I "..\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\statdll.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STATdll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\STATdll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib ..\..\lib\Statd.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\STATd.pdb" /debug /machine:I386 /def:".\statd.def" /out:"..\..\bin\STATd.dll" /implib:"..\..\lib\STATDlld.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\statdll.res"

"..\..\bin\STATd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("STATdll.dep")
!INCLUDE "STATdll.dep"
!ELSE 
!MESSAGE Warning: cannot find "STATdll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "STATDll - Win32 Release" || "$(CFG)" == "STATDll - Win32 Debug"
SOURCE=.\src\statdll.rc

!IF  "$(CFG)" == "STATDll - Win32 Release"


"$(INTDIR)\statdll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x809 /fo"$(INTDIR)\statdll.res" /i "src" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "STATDll - Win32 Debug"


"$(INTDIR)\statdll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x809 /fo"$(INTDIR)\statdll.res" /i "src" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "STATDll - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATdll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STATdll.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "STATDll - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\inc" /I "..\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATdll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STATdll.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

