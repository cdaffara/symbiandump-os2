# Microsoft Developer Studio Generated NMAKE File, Based on basicfunctest.dsp
!IF "$(CFG)" == ""
CFG=basicfunctest - Win32 Debug
!MESSAGE No configuration specified. Defaulting to basicfunctest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "basicfunctest - Win32 Release" && "$(CFG)" != "basicfunctest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "basicfunctest - Win32 Release"

OUTDIR=$(EPOCROOT)epoc32\release\tools\rel
INTDIR=$(EPOCROOT)EPOC32\BUILD\EZLib2\ZLib\Test\Release
# Begin Custom Macros
OutDir=$(EPOCROOT)epoc32\release\tools\rel
# End Custom Macros

ALL : "$(OUTDIR)\basicfunctest.exe"


CLEAN :
	-@erase "$(INTDIR)\Basicfunctest.obj"
	-@erase "$(OUTDIR)\basicfunctest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

MTL=midl.exe
CPP_PROJ=/nologo /Zp4 /ML /O1 /Op /GX /I "$(EPOCROOT)epoc32\include" /D "__SYMBIAN32__" /D "__VC32__" /D "__TOOLS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "__SUPPORT_CPP_EXCEPTIONS__" /Fo"$(INTDIR)\\" /GF /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(EPOCROOT)epoc32\release\tools\rel/basicfunctest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS="kernel32.lib" "$(EPOCROOT)epoc32\release\winc\REL\zlib.lib" /nologo /subsystem:console /incremental:no /pdb:"$(EPOCROOT)epoc32\release\tools\rel/basicfunctest.pdb" /machine:IX86 /out:"$(EPOCROOT)epoc32\release\tools\rel/basicfunctest.exe" /fixed:no /ignore:4089 /ignore:4005 /ignore:4098 /WARN:3 
LINK32_OBJS= \
	"$(INTDIR)\Basicfunctest.obj"

"$(OUTDIR)\basicfunctest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "basicfunctest - Win32 Debug"

OUTDIR=$(EPOCROOT)epoc32\release\tools\deb
INTDIR=$(EPOCROOT)EPOC32\BUILD\EZLib2\ZLib\Test\Debug
# Begin Custom Macros
OutDir=$(EPOCROOT)epoc32\release\tools\deb
# End Custom Macros

ALL : "$(OUTDIR)\basicfunctest.exe" "$(OUTDIR)\basicfunctest.bsc"


CLEAN :
	-@erase "$(INTDIR)\Basicfunctest.obj"
	-@erase "$(INTDIR)\Basicfunctest.sbr"
	-@erase "$(OUTDIR)\basicfunctest.bsc"
	-@erase "$(OUTDIR)\basicfunctest.exe"
	-@erase "$(OUTDIR)\basicfunctest.ilk"
	-@erase "$(OUTDIR)\basicfunctest.pdb"
	-@erase ".\$(EPOCROOT)epoc32\release\tools\deb\basicfunctest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

MTL=midl.exe
CPP_PROJ=/nologo /Zp4 /MLd /Zi /Od /GX /I "$(EPOCROOT)epoc32\include" /D "__SYMBIAN32__" /D "__VC32__" /D "__TOOLS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "__SUPPORT_CPP_EXCEPTIONS__" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(EPOCROOT)epoc32\release\tools\deb\basicfunctest.PDB" /GF /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(EPOCROOT)epoc32\release\tools\deb/basicfunctest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Basicfunctest.sbr"

"$(OUTDIR)\basicfunctest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS="kernel32.lib" "$(EPOCROOT)epoc32\release\winc\DEB\zlib.lib" /nologo /subsystem:console /incremental:yes /pdb:"$(EPOCROOT)epoc32\release\tools\deb/basicfunctest.pdb" /debug /machine:IX86 /out:"$(EPOCROOT)epoc32\release\tools\deb/basicfunctest.exe" /fixed:no /ignore:4089 /ignore:4005 /ignore:4098 /WARN:3 
LINK32_OBJS= \
	"$(INTDIR)\Basicfunctest.obj"

"$(OUTDIR)\basicfunctest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("basicfunctest.dep")
!INCLUDE "basicfunctest.dep"
!ELSE 
!MESSAGE Warning: cannot find "basicfunctest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "basicfunctest - Win32 Release" || "$(CFG)" == "basicfunctest - Win32 Debug"
SOURCE=..\basicfunctest\src\Basicfunctest.cpp

!IF  "$(CFG)" == "basicfunctest - Win32 Release"


"$(INTDIR)\Basicfunctest.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "basicfunctest - Win32 Debug"


"$(INTDIR)\Basicfunctest.obj"	"$(INTDIR)\Basicfunctest.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

