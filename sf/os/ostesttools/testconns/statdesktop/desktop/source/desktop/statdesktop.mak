# Microsoft Developer Studio Generated NMAKE File, Based on STATDesktop.dsp
!IF "$(CFG)" == ""
CFG=STATDesktop - Win32 Debug
!MESSAGE No configuration specified. Defaulting to STATDesktop - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "STATDesktop - Win32 Release" && "$(CFG)" != "STATDesktop - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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

!IF  "$(CFG)" == "STATDesktop - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\bin\STATDesktop.exe" "$(OUTDIR)\STATDesktop.bsc"


CLEAN :
	-@erase "$(INTDIR)\INI.obj"
	-@erase "$(INTDIR)\INI.sbr"
	-@erase "$(INTDIR)\STATDesktop.obj"
	-@erase "$(INTDIR)\STATDesktop.pch"
	-@erase "$(INTDIR)\statdesktop.res"
	-@erase "$(INTDIR)\STATDesktop.sbr"
	-@erase "$(INTDIR)\STATDesktopDlg.obj"
	-@erase "$(INTDIR)\STATDesktopDlg.sbr"
	-@erase "$(INTDIR)\STATManageConnection.obj"
	-@erase "$(INTDIR)\STATManageConnection.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\STATDesktop.bsc"
	-@erase "..\..\bin\STATDesktop.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\common\inc" /I "..\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATDesktop.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\statdesktop.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STATDesktop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\INI.sbr" \
	"$(INTDIR)\STATDesktop.sbr" \
	"$(INTDIR)\STATDesktopDlg.sbr" \
	"$(INTDIR)\STATManageConnection.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\STATDesktop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\STATDesktop.pdb" /machine:I386 /out:"..\..\bin\STATDesktop.exe" 
LINK32_OBJS= \
	"$(INTDIR)\INI.obj" \
	"$(INTDIR)\STATDesktop.obj" \
	"$(INTDIR)\STATDesktopDlg.obj" \
	"$(INTDIR)\STATManageConnection.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\statdesktop.res"

"..\..\bin\STATDesktop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Performing Custom Build Step on STATDesktop
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "..\..\bin\STATDesktop.exe" "$(OUTDIR)\STATDesktop.bsc"
   .\UpdateDesktop.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "STATDesktop - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\bin\STATDesktopd.exe" "$(OUTDIR)\STATDesktop.bsc"


CLEAN :
	-@erase "$(INTDIR)\INI.obj"
	-@erase "$(INTDIR)\INI.sbr"
	-@erase "$(INTDIR)\STATDesktop.obj"
	-@erase "$(INTDIR)\STATDesktop.pch"
	-@erase "$(INTDIR)\statdesktop.res"
	-@erase "$(INTDIR)\STATDesktop.sbr"
	-@erase "$(INTDIR)\STATDesktopDlg.obj"
	-@erase "$(INTDIR)\STATDesktopDlg.sbr"
	-@erase "$(INTDIR)\STATManageConnection.obj"
	-@erase "$(INTDIR)\STATManageConnection.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\STATDesktop.bsc"
	-@erase "$(OUTDIR)\STATDesktopd.pdb"
	-@erase "..\..\bin\STATDesktopd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\inc"  /I "..\common\inc" /I "..\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATDesktop.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\statdesktop.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STATDesktop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\INI.sbr" \
	"$(INTDIR)\STATDesktop.sbr" \
	"$(INTDIR)\STATDesktopDlg.sbr" \
	"$(INTDIR)\STATManageConnection.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\STATDesktop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=WS2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\STATDesktopd.pdb" /debug /machine:I386 /out:"..\..\bin\STATDesktopd.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\INI.obj" \
	"$(INTDIR)\STATDesktop.obj" \
	"$(INTDIR)\STATDesktopDlg.obj" \
	"$(INTDIR)\STATManageConnection.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\statdesktop.res"

"..\..\bin\STATDesktopd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("STATDesktop.dep")
!INCLUDE "STATDesktop.dep"
!ELSE 
!MESSAGE Warning: cannot find "STATDesktop.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "STATDesktop - Win32 Release" || "$(CFG)" == "STATDesktop - Win32 Debug"
SOURCE=..\common\src\INI.cpp

!IF  "$(CFG)" == "STATDesktop - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\common\inc" /I "..\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\INI.obj"	"$(INTDIR)\INI.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "STATDesktop - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\inc" /I "..\common\inc" /I "..\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\INI.obj"	"$(INTDIR)\INI.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\STATDesktop.cpp

"$(INTDIR)\STATDesktop.obj"	"$(INTDIR)\STATDesktop.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATDesktop.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statdesktop.rc

!IF  "$(CFG)" == "STATDesktop - Win32 Release"


"$(INTDIR)\statdesktop.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x809 /fo"$(INTDIR)\statdesktop.res" /i "src" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "STATDesktop - Win32 Debug"


"$(INTDIR)\statdesktop.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x809 /fo"$(INTDIR)\statdesktop.res" /i "src" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\src\STATDesktopDlg.cpp

"$(INTDIR)\STATDesktopDlg.obj"	"$(INTDIR)\STATDesktopDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATDesktop.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\STATManageConnection.cpp

"$(INTDIR)\STATManageConnection.obj"	"$(INTDIR)\STATManageConnection.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATDesktop.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "STATDesktop - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I ".\inc"  /I "..\common\inc" /I "..\lib\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATDesktop.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STATDesktop.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "STATDesktop - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\inc"  /I "..\common\inc" /I "..\lib\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATDesktop.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STATDesktop.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

