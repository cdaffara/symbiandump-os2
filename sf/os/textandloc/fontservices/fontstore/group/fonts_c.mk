#
# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Build font files
#

!if "$(PLATFORM)"=="WINS" || "$(PLATFORM)"=="WINSCW"
TARGETDIR=$(EPOCROOT)EPOC32\RELEASE\$(PLATFORM)\$(CFG)\Z\Test
!else
TARGETDIR=$(EPOCROOT)EPOC32\DATA\Z\System\Test
!endif


$(TARGETDIR) :
	@perl -S emkdir.pl "$(TARGETDIR)"

FONTS=$(TARGETDIR)\TSECTION.GDR 
FONTSRC=..\tfs
TSECTION_BDF=$(FONTSRC)\TSECTION.BDF
TSECTION_GD=$(FONTSRC)\TSECTION.GD

$(TARGETDIR)\TSECTION.GDR : $(TSECTION_BDF) $(TSECTION_GD)
	fnttran $(TSECTION_BDF) $(TSECTION_GD) $@


do_nothing :
	rem do_nothing

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : $(TARGETDIR) $(FONTS)

SAVESPACE : BLD

RESOURCE : BLD

CLEAN : 
	-erase $(FONTS)

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

RELEASABLES : 
	@echo $(FONTS)

