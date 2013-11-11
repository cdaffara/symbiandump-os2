#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

GENERATED_FILES= \
	$(EPOCROOT)epoc32\build\generated\t_decode\WspDummyConstants.cpp \
	$(EPOCROOT)epoc32\include\WspDummyConstants.h

$(EPOCROOT)epoc32\build\generated\t_decode\WspDummyConstants.cpp : ..\strings\WspDummyConstants.st
	@perl -S emkdir.pl $(EPOCROOT)epoc32\build\generated\t_decode
	copy ..\strings\WspDummyConstants.st $(EPOCROOT)epoc32\build\generated\t_decode
	perl $(EPOCROOT)epoc32\tools\stringtable.pl $(EPOCROOT)epoc32\build\generated\t_decode\WspDummyConstants.st

$(EPOCROOT)epoc32\include\WspDummyConstants.h : $(EPOCROOT)epoc32\build\generated\t_decode\WspDummyConstants.cpp
	copy $(EPOCROOT)epoc32\build\generated\t_decode\WspDummyConstants.h $(EPOCROOT)epoc32\include

ifeq "$(OS)" "Windows_NT"
ERASE := @erase 2>>nul
else
ERASE := @erase
endif

do_nothing:
	@rem do nothing

#
# The targets invoked by bld...
#

MAKMAKE : $(GENERATED_FILES)

BLD : MAKMAKE

SAVESPACE : MAKMAKE

CLEAN : 
	-$(ERASE) $(GENERATED_FILES) $(EPOCROOT)epoc32\build\generated\t_decode\WspDummyConstants.h

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES : $(GENERATED_FILES)

ROMFILE : do_nothing
