#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  
#

# which accompanies this distribution, and is available

#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

# Typically C:\apps\MSVC6\VC98\Bin
COMPILER=nmake

# Called with
#
# $(PLATFORM) = TOOLS
# $(CFG)      = DEB, REL

BLD	:  
	@echo BLD called with $(PLATFORM)$(CFG)
	
# release version
!if "$(PLATFORM)"=="TOOLS" && "$(CFG)" == "REL"
	-mkdir "$(EPOCROOT)epoc32\tools\STAT"

# Go up one level to access the makefile
	cd ".."

# Execute the command line compiler (DLL)
	"$(COMPILER)" /A /NOLOGO /f "STATdll.mak" CFG="STATDll - Win32 Release"
	
	echo ** STAT DLL (RELEASE) compiled successfully

!endif

# debug version
!if "$(PLATFORM)"=="TOOLS" && "$(CFG)" == "DEB"

# Go up one level to access the makefile
	cd ".."

# Execute the command line compiler (DLL)
	"$(COMPILER)" /A /NOLOGO /f "STATdll.mak" CFG="STATDll - Win32 Debug"
	
	echo ** STAT DLL (DEBUG) compiled successfully

!endif

CLEAN :
!if "$(PLATFORM)"=="TOOLS" && "$(CFG)" == "REL"
	-rmdir "..\Release" /q /s
!endif

CLEAN :
!if "$(PLATFORM)"=="TOOLS" && "$(CFG)" == "DEB"
	-rmdir "..\Debug" /q /s
!endif

RELEASABLES :
!if "$(PLATFORM)"=="TOOLS" && "$(CFG)" == "REL"
	@echo $(EPOCROOT)epoc32\tools\STAT\stat.dll
	@echo $(EPOCROOT)epoc32\tools\STAT\stat.exp
	@echo $(EPOCROOT)epoc32\tools\STAT\stat.lib
!endif

SAVESPACE : BLD

MAKMAKE LIB CLEANLIB RESOURCE FREEZE FINAL : 
