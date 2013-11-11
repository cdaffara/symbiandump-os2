# Microsoft Developer Studio Generated NMAKE File, Based on dlltest.dsp
!IF "$(CFG)" == ""
CFG=dlltest - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dlltest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dlltest - Win32 Release" && "$(CFG)" != "dlltest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dlltest.mak" CFG="dlltest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dlltest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dlltest - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "dlltest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\STATDllTest.exe" "$(OUTDIR)\dlltest.bsc"


CLEAN :
	-@erase "$(INTDIR)\dlltest.obj"
	-@erase "$(INTDIR)\dlltest.pch"
	-@erase "$(INTDIR)\dlltest.res"
	-@erase "$(INTDIR)\dlltest.sbr"
	-@erase "$(INTDIR)\dlltestDlg.obj"
	-@erase "$(INTDIR)\dlltestDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dlltest.bsc"
	-@erase "$(OUTDIR)\STATDllTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\dlltest.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dlltest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dlltest.sbr" \
	"$(INTDIR)\dlltestDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\dlltest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=..\..\lib\STATDll.lib ..\..\lib\STAT.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"$(OUTDIR)\STATDllTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlltest.obj" \
	"$(INTDIR)\dlltestDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\dlltest.res"

"$(OUTDIR)\STATDllTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\STATDllTest.exe" "$(OUTDIR)\dlltest.bsc"
   copy release\STATDllTest.exe \epoc32\tools\stat\STATDllTest.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dlltest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\STATDllTestd.exe" "$(OUTDIR)\dlltest.bsc"


CLEAN :
	-@erase "$(INTDIR)\dlltest.obj"
	-@erase "$(INTDIR)\dlltest.pch"
	-@erase "$(INTDIR)\dlltest.res"
	-@erase "$(INTDIR)\dlltest.sbr"
	-@erase "$(INTDIR)\dlltestDlg.obj"
	-@erase "$(INTDIR)\dlltestDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dlltest.bsc"
	-@erase "$(OUTDIR)\STATDllTestd.exe"
	-@erase "$(OUTDIR)\STATDllTestd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GX /Zi /Od /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\dlltest.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dlltest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dlltest.sbr" \
	"$(INTDIR)\dlltestDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\dlltest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=..\..\lib\STATDlld.lib ..\..\lib\STATd.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\STATDllTestd.pdb" /debug /machine:I386 /out:"$(OUTDIR)\STATDllTestd.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dlltest.obj" \
	"$(INTDIR)\dlltestDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\dlltest.res"

"$(OUTDIR)\STATDllTestd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dlltest.dep")
!INCLUDE "dlltest.dep"
!ELSE 
!MESSAGE Warning: cannot find "dlltest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dlltest - Win32 Release" || "$(CFG)" == "dlltest - Win32 Debug"
SOURCE=.\src\dlltest.cpp

"$(INTDIR)\dlltest.obj"	"$(INTDIR)\dlltest.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\dlltest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dlltest.rc

"$(INTDIR)\dlltest.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\src\dlltestDlg.cpp

"$(INTDIR)\dlltestDlg.obj"	"$(INTDIR)\dlltestDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\dlltest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "dlltest - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\dlltest.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "dlltest - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GX /Zi /Od /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\dlltest.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\utils.cpp

"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\dlltest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

