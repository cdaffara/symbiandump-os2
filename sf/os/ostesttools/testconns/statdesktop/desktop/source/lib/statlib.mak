# Microsoft Developer Studio Generated NMAKE File, Based on STATLib.dsp
!IF "$(CFG)" == ""
CFG=STATLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to STATLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "STATLib - Win32 Release" && "$(CFG)" != "STATLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "STATLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\lib\STAT.lib" "$(OUTDIR)\STATLib.bsc"


CLEAN :
	-@erase "$(INTDIR)\cRepmisc.obj"
	-@erase "$(INTDIR)\cRepmisc.sbr"
	-@erase "$(INTDIR)\cReporter.obj"
	-@erase "$(INTDIR)\cReporter.sbr"
	-@erase "$(INTDIR)\cstatdataformatconverter.obj"
	-@erase "$(INTDIR)\cstatdataformatconverter.sbr"
	-@erase "$(INTDIR)\cstatimageverify.obj"
	-@erase "$(INTDIR)\cstatimageverify.sbr"
	-@erase "$(INTDIR)\cstatlogfile.obj"
	-@erase "$(INTDIR)\cstatlogfile.sbr"
	-@erase "$(INTDIR)\cstatreturncodes.obj"
	-@erase "$(INTDIR)\cstatreturncodes.sbr"
	-@erase "$(INTDIR)\INI.obj"
	-@erase "$(INTDIR)\INI.sbr"
	-@erase "$(INTDIR)\statbitmap.obj"
	-@erase "$(INTDIR)\statbitmap.sbr"
	-@erase "$(INTDIR)\STATComms.obj"
	-@erase "$(INTDIR)\STATComms.sbr"
	-@erase "$(INTDIR)\STATEngine.obj"
	-@erase "$(INTDIR)\STATEngine.sbr"
	-@erase "$(INTDIR)\statexp.obj"
	-@erase "$(INTDIR)\statexp.sbr"
	-@erase "$(INTDIR)\STATLib.pch"
	-@erase "$(INTDIR)\statlist.obj"
	-@erase "$(INTDIR)\statlist.sbr"
	-@erase "$(INTDIR)\statmember.obj"
	-@erase "$(INTDIR)\statmember.sbr"
	-@erase "$(INTDIR)\statscriptdecoder.obj"
	-@erase "$(INTDIR)\statscriptdecoder.sbr"
	-@erase "$(INTDIR)\statserial.obj"
	-@erase "$(INTDIR)\statserial.sbr"
	-@erase "$(INTDIR)\statsocket_block.obj"
	-@erase "$(INTDIR)\statsocket_block.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\STATLib.bsc"
	-@erase "..\..\lib\STAT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\common\inc" /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATLib.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STATLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cRepmisc.sbr" \
	"$(INTDIR)\cReporter.sbr" \
	"$(INTDIR)\cstatdataformatconverter.sbr" \
	"$(INTDIR)\cstatimageverify.sbr" \
	"$(INTDIR)\cstatlogfile.sbr" \
	"$(INTDIR)\cstatreturncodes.sbr" \
	"$(INTDIR)\INI.sbr" \
	"$(INTDIR)\statbitmap.sbr" \
	"$(INTDIR)\STATComms.sbr" \
	"$(INTDIR)\STATEngine.sbr" \
	"$(INTDIR)\statexp.sbr" \
	"$(INTDIR)\statlist.sbr" \
	"$(INTDIR)\statmember.sbr" \
	"$(INTDIR)\statscriptdecoder.sbr" \
	"$(INTDIR)\statserial.sbr" \
	"$(INTDIR)\statsocket_block.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\STATLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\STAT.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cRepmisc.obj" \
	"$(INTDIR)\cReporter.obj" \
	"$(INTDIR)\cstatdataformatconverter.obj" \
	"$(INTDIR)\cstatimageverify.obj" \
	"$(INTDIR)\cstatlogfile.obj" \
	"$(INTDIR)\cstatreturncodes.obj" \
	"$(INTDIR)\INI.obj" \
	"$(INTDIR)\statbitmap.obj" \
	"$(INTDIR)\STATComms.obj" \
	"$(INTDIR)\STATEngine.obj" \
	"$(INTDIR)\statexp.obj" \
	"$(INTDIR)\statlist.obj" \
	"$(INTDIR)\statmember.obj" \
	"$(INTDIR)\statscriptdecoder.obj" \
	"$(INTDIR)\statserial.obj" \
	"$(INTDIR)\statsocket_block.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\..\lib\STAT.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "STATLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\lib\STATd.lib" "$(OUTDIR)\STATLib.pch" "$(OUTDIR)\STATLib.bsc"


CLEAN :
	-@erase "$(INTDIR)\cRepmisc.obj"
	-@erase "$(INTDIR)\cRepmisc.sbr"
	-@erase "$(INTDIR)\cReporter.obj"
	-@erase "$(INTDIR)\cReporter.sbr"
	-@erase "$(INTDIR)\cstatdataformatconverter.obj"
	-@erase "$(INTDIR)\cstatdataformatconverter.sbr"
	-@erase "$(INTDIR)\cstatimageverify.obj"
	-@erase "$(INTDIR)\cstatimageverify.sbr"
	-@erase "$(INTDIR)\cstatlogfile.obj"
	-@erase "$(INTDIR)\cstatlogfile.sbr"
	-@erase "$(INTDIR)\cstatreturncodes.obj"
	-@erase "$(INTDIR)\cstatreturncodes.sbr"
	-@erase "$(INTDIR)\INI.obj"
	-@erase "$(INTDIR)\INI.sbr"
	-@erase "$(INTDIR)\statbitmap.obj"
	-@erase "$(INTDIR)\statbitmap.sbr"
	-@erase "$(INTDIR)\STATComms.obj"
	-@erase "$(INTDIR)\STATComms.sbr"
	-@erase "$(INTDIR)\STATEngine.obj"
	-@erase "$(INTDIR)\STATEngine.sbr"
	-@erase "$(INTDIR)\statexp.obj"
	-@erase "$(INTDIR)\statexp.sbr"
	-@erase "$(INTDIR)\STATLib.pch"
	-@erase "$(INTDIR)\statlist.obj"
	-@erase "$(INTDIR)\statlist.sbr"
	-@erase "$(INTDIR)\statmember.obj"
	-@erase "$(INTDIR)\statmember.sbr"
	-@erase "$(INTDIR)\statscriptdecoder.obj"
	-@erase "$(INTDIR)\statscriptdecoder.sbr"
	-@erase "$(INTDIR)\statserial.obj"
	-@erase "$(INTDIR)\statserial.sbr"
	-@erase "$(INTDIR)\statsocket_block.obj"
	-@erase "$(INTDIR)\statsocket_block.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\STATLib.bsc"
	-@erase "..\..\lib\STATd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common\inc" /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STATLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cRepmisc.sbr" \
	"$(INTDIR)\cReporter.sbr" \
	"$(INTDIR)\cstatdataformatconverter.sbr" \
	"$(INTDIR)\cstatimageverify.sbr" \
	"$(INTDIR)\cstatlogfile.sbr" \
	"$(INTDIR)\cstatreturncodes.sbr" \
	"$(INTDIR)\INI.sbr" \
	"$(INTDIR)\statbitmap.sbr" \
	"$(INTDIR)\STATComms.sbr" \
	"$(INTDIR)\STATEngine.sbr" \
	"$(INTDIR)\statexp.sbr" \
	"$(INTDIR)\statlist.sbr" \
	"$(INTDIR)\statmember.sbr" \
	"$(INTDIR)\statscriptdecoder.sbr" \
	"$(INTDIR)\statserial.sbr" \
	"$(INTDIR)\statsocket_block.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\STATLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\STATd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cRepmisc.obj" \
	"$(INTDIR)\cReporter.obj" \
	"$(INTDIR)\cstatdataformatconverter.obj" \
	"$(INTDIR)\cstatimageverify.obj" \
	"$(INTDIR)\cstatlogfile.obj" \
	"$(INTDIR)\cstatreturncodes.obj" \
	"$(INTDIR)\INI.obj" \
	"$(INTDIR)\statbitmap.obj" \
	"$(INTDIR)\STATComms.obj" \
	"$(INTDIR)\STATEngine.obj" \
	"$(INTDIR)\statexp.obj" \
	"$(INTDIR)\statlist.obj" \
	"$(INTDIR)\statmember.obj" \
	"$(INTDIR)\statscriptdecoder.obj" \
	"$(INTDIR)\statserial.obj" \
	"$(INTDIR)\statsocket_block.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\..\lib\STATd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("STATLib.dep")
!INCLUDE "STATLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "STATLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "STATLib - Win32 Release" || "$(CFG)" == "STATLib - Win32 Debug"
SOURCE=.\src\cRepmisc.cpp

"$(INTDIR)\cRepmisc.obj"	"$(INTDIR)\cRepmisc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\cReporter.cpp

"$(INTDIR)\cReporter.obj"	"$(INTDIR)\cReporter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\cstatdataformatconverter.cpp

"$(INTDIR)\cstatdataformatconverter.obj"	"$(INTDIR)\cstatdataformatconverter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\cstatimageverify.cpp

"$(INTDIR)\cstatimageverify.obj"	"$(INTDIR)\cstatimageverify.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\cstatlogfile.cpp

"$(INTDIR)\cstatlogfile.obj"	"$(INTDIR)\cstatlogfile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\cstatreturncodes.cpp

"$(INTDIR)\cstatreturncodes.obj"	"$(INTDIR)\cstatreturncodes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\src\INI.cpp

!IF  "$(CFG)" == "STATLib - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\common\inc" /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\INI.obj"	"$(INTDIR)\INI.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "STATLib - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common\inc" /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\INI.obj"	"$(INTDIR)\INI.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\statbitmap.cpp

"$(INTDIR)\statbitmap.obj"	"$(INTDIR)\statbitmap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\STATComms.cpp

"$(INTDIR)\STATComms.obj"	"$(INTDIR)\STATComms.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\STATEngine.cpp

"$(INTDIR)\STATEngine.obj"	"$(INTDIR)\STATEngine.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statexp.cpp

"$(INTDIR)\statexp.obj"	"$(INTDIR)\statexp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statlist.cpp

"$(INTDIR)\statlist.obj"	"$(INTDIR)\statlist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statmember.cpp

"$(INTDIR)\statmember.obj"	"$(INTDIR)\statmember.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statscriptdecoder.cpp

"$(INTDIR)\statscriptdecoder.obj"	"$(INTDIR)\statscriptdecoder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statserial.cpp

"$(INTDIR)\statserial.obj"	"$(INTDIR)\statserial.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\statsocket_block.cpp

"$(INTDIR)\statsocket_block.obj"	"$(INTDIR)\statsocket_block.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STATLib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "STATLib - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\common\inc" /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATLib.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STATLib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "STATLib - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common\inc" /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STATLib.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STATLib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

