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

# Execute the command line compiler (Desktop)
	"$(COMPILER)" /A /NOLOGO /f "STATDesktop.mak" CFG="STATDesktop - Win32 Release"
	
	echo ** STAT Desktop (RELEASE) compiled successfully

!endif

# debug version
!if "$(PLATFORM)"=="TOOLS" && "$(CFG)" == "DEB"

# Go up one level to access the makefile
	cd ".."

# Execute the command line compiler (Desktop)
	"$(COMPILER)" /A /NOLOGO /f "STATDesktop.mak" CFG="STATDesktop - Win32 Debug"
	
	echo ** STAT Desktop (DEBUG) compiled successfully

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
	@echo $(EPOCROOT)epoc32\tools\STAT\STATDesktop.exe
!endif

SAVESPACE : BLD

MAKMAKE LIB CLEANLIB RESOURCE FREEZE FINAL : 
