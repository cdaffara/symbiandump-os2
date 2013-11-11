# Microsoft Developer Studio Generated NMAKE File, Based on dlltest_multithreaded.dsp
!IF "$(CFG)" == ""
CFG=dlltest_multithreaded - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dlltest_multithreaded - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dlltest_multithreaded - Win32 Release" && "$(CFG)" != "dlltest_multithreaded - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dlltest_multithreaded.mak" CFG="dlltest_multithreaded - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dlltest_multithreaded - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dlltest_multithreaded - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "dlltest_multithreaded - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\STATDllTestMT.exe" "$(OUTDIR)\dlltest_multithreaded.bsc"


CLEAN :
	-@erase "$(INTDIR)\dlltest_multithreaded.obj"
	-@erase "$(INTDIR)\dlltest_multithreaded.pch"
	-@erase "$(INTDIR)\dlltest_multithreaded.sbr"
	-@erase "$(INTDIR)\dlltest_worker.obj"
	-@erase "$(INTDIR)\dlltest_worker.sbr"
	-@erase "$(INTDIR)\STATTask.obj"
	-@erase "$(INTDIR)\STATTask.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dlltest_multithreaded.bsc"
	-@erase "$(OUTDIR)\STATDllTestMT.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest_multithreaded.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dlltest_multithreaded.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dlltest_multithreaded.sbr" \
	"$(INTDIR)\dlltest_worker.sbr" \
	"$(INTDIR)\STATTask.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\dlltest_multithreaded.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\lib\STATDll.lib ..\..\lib\STAT.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"$(OUTDIR)\STATDllTestMT.exe" /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\dlltest_multithreaded.obj" \
	"$(INTDIR)\dlltest_worker.obj" \
	"$(INTDIR)\STATTask.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\STATDllTestMT.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\STATDllTestMT.exe" "$(OUTDIR)\dlltest_multithreaded.bsc"
   copy release\STATDllTestMT.exe \epoc32\tools\stat\STATDllTestMT.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dlltest_multithreaded - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\STATDllTestMTd.exe" "$(OUTDIR)\dlltest_multithreaded.bsc"


CLEAN :
	-@erase "$(INTDIR)\dlltest_multithreaded.obj"
	-@erase "$(INTDIR)\dlltest_multithreaded.pch"
	-@erase "$(INTDIR)\dlltest_multithreaded.sbr"
	-@erase "$(INTDIR)\dlltest_worker.obj"
	-@erase "$(INTDIR)\dlltest_worker.sbr"
	-@erase "$(INTDIR)\STATTask.obj"
	-@erase "$(INTDIR)\STATTask.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dlltest_multithreaded.bsc"
	-@erase "$(OUTDIR)\STATDllTestMTd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GX /Zi /Od /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest_multithreaded.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dlltest_multithreaded.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dlltest_multithreaded.sbr" \
	"$(INTDIR)\dlltest_worker.sbr" \
	"$(INTDIR)\STATTask.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\dlltest_multithreaded.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\lib\STATDlld.lib ..\..\lib\STATd.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"$(OUTDIR)\STATDllTestMTd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dlltest_multithreaded.obj" \
	"$(INTDIR)\dlltest_worker.obj" \
	"$(INTDIR)\STATTask.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\STATDllTestMTd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dlltest_multithreaded.dep")
!INCLUDE "dlltest_multithreaded.dep"
!ELSE 
!MESSAGE Warning: cannot find "dlltest_multithreaded.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dlltest_multithreaded - Win32 Release" || "$(CFG)" == "dlltest_multithreaded - Win32 Debug"
SOURCE=.\src\dlltest_multithreaded.cpp

"$(INTDIR)\dlltest_multithreaded.obj"	"$(INTDIR)\dlltest_multithreaded.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\dlltest_multithreaded.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\dlltest_worker.cpp

"$(INTDIR)\dlltest_worker.obj"	"$(INTDIR)\dlltest_worker.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\dlltest_multithreaded.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\STATTask.cpp

"$(INTDIR)\STATTask.obj"	"$(INTDIR)\STATTask.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\dlltest_multithreaded.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "dlltest_multithreaded - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest_multithreaded.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\dlltest_multithreaded.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "dlltest_multithreaded - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GX /Zi /Od /I ".\inc" /I "..\..\source\lib\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dlltest_multithreaded.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\dlltest_multithreaded.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

