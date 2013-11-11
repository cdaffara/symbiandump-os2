# Microsoft Developer Studio Generated NMAKE File, Based on stat2perl.dsp
!IF "$(CFG)" == ""
CFG=stat2perl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to stat2perl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "stat2perl - Win32 Release" && "$(CFG)" != "stat2perl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stat2perl.mak" CFG="stat2perl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stat2perl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "stat2perl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stat2perl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\bin\STAT2Perl.exe" "$(OUTDIR)\stat2perl.bsc"


CLEAN :
	-@erase "$(INTDIR)\CommandLine.obj"
	-@erase "$(INTDIR)\CommandLine.sbr"
	-@erase "$(INTDIR)\stat2perl.obj"
	-@erase "$(INTDIR)\stat2perl.pch"
	-@erase "$(INTDIR)\stat2perl.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\stat2perl.bsc"
	-@erase "..\..\bin\STAT2Perl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I ".\src" /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\stat2perl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\stat2perl.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CommandLine.sbr" \
	"$(INTDIR)\stat2perl.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\stat2perl.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\STAT2Perl.pdb" /machine:I386 /out:"..\..\bin\STAT2Perl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CommandLine.obj" \
	"$(INTDIR)\stat2perl.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\..\bin\STAT2Perl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Performing Custom Build Step on stat2perl
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "..\..\bin\STAT2Perl.exe" "$(OUTDIR)\stat2perl.bsc"
   .\UpdateStat2Perl.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "stat2perl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\bin\STAT2Perld.exe" "$(OUTDIR)\stat2perl.bsc"


CLEAN :
	-@erase "$(INTDIR)\CommandLine.obj"
	-@erase "$(INTDIR)\CommandLine.sbr"
	-@erase "$(INTDIR)\stat2perl.obj"
	-@erase "$(INTDIR)\stat2perl.pch"
	-@erase "$(INTDIR)\stat2perl.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\stat2perl.bsc"
	-@erase "$(OUTDIR)\STAT2Perld.pdb"
	-@erase "..\..\bin\STAT2Perld.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I ".\src" /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\stat2perl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\stat2perl.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CommandLine.sbr" \
	"$(INTDIR)\stat2perl.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\stat2perl.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\STAT2Perld.pdb" /debug /machine:I386 /out:"..\..\bin\STAT2Perld.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CommandLine.obj" \
	"$(INTDIR)\stat2perl.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\..\bin\STAT2Perld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("stat2perl.dep")
!INCLUDE "stat2perl.dep"
!ELSE 
!MESSAGE Warning: cannot find "stat2perl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "stat2perl - Win32 Release" || "$(CFG)" == "stat2perl - Win32 Debug"
SOURCE=.\src\CommandLine.cpp

"$(INTDIR)\CommandLine.obj"	"$(INTDIR)\CommandLine.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\stat2perl.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\stat2perl.cpp

"$(INTDIR)\stat2perl.obj"	"$(INTDIR)\stat2perl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\stat2perl.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "stat2perl - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I ".\src" /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\stat2perl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\stat2perl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "stat2perl - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /Zi /Od /I ".\src" /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\stat2perl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\stat2perl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

