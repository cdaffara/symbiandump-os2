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
TARGETDIR=$(EPOCROOT)EPOC32\RELEASE\$(PLATFORM)\$(CFG)\Z\Resource\Fonts
!else
TARGETDIR=$(EPOCROOT)EPOC32\DATA\Z\Resource\Fonts
!endif

$(TARGETDIR) :
	@perl -S emkdir.pl "$(TARGETDIR)"

FONTS=\
	$(TARGETDIR)\EON14.GDR \
	$(TARGETDIR)\DejaVuSans.ttf \
	$(TARGETDIR)\DejaVuSans-Bold.ttf \
	$(TARGETDIR)\DejaVuSans-Oblique.ttf \
	$(TARGETDIR)\DejaVuSans-BoldOblique.ttf \
	$(TARGETDIR)\DejaVuSans-ExtraLight.ttf \
	$(TARGETDIR)\DejaVuSansCondensed.ttf \
	$(TARGETDIR)\DejaVuSansCondensed-Bold.ttf \
	$(TARGETDIR)\DejaVuSansCondensed-Oblique.ttf \
	$(TARGETDIR)\DejaVuSansCondensed-BoldOblique.ttf \
	$(TARGETDIR)\DejaVuSansMono.ttf \
	$(TARGETDIR)\DejaVuSansMono-Bold.ttf \
	$(TARGETDIR)\DejaVuSansMono-Oblique.ttf \
	$(TARGETDIR)\DejaVuSansMono-BoldOblique.ttf \
	$(TARGETDIR)\DejaVuSerif.ttf \
	$(TARGETDIR)\DejaVuSerif-Bold.ttf \
	$(TARGETDIR)\DejaVuSerif-Italic.ttf \
	$(TARGETDIR)\DejaVuSerif-BoldItalic.ttf \
	$(TARGETDIR)\DejaVuSerifCondensed.ttf \
	$(TARGETDIR)\DejaVuSerifCondensed-Bold.ttf \
	$(TARGETDIR)\DejaVuSerifCondensed-Italic.ttf \
	$(TARGETDIR)\DejaVuSerifCondensed-BoldItalic.ttf

FONTSRC=..\bitmap
EON14SRCS=\
	$(FONTSRC)\squashed.bdf \
	$(FONTSRC)\calc35.bdf \
	$(FONTSRC)\calc13.bdf \
	$(FONTSRC)\calc18.bdf \
	$(FONTSRC)\calcinv.bdf \
	$(FONTSRC)\digit35.bdf \
	$(FONTSRC)\agnsym11.bdf \
	$(FONTSRC)\agnsym16.bdf \
	$(FONTSRC)\eiksym15.bdf \
	$(FONTSRC)\base.gd

$(TARGETDIR)\EON14.GDR : $(EON14SRCS)
	fnttran $(EON14SRCS) $@

$(TARGETDIR)\DejaVuSans.ttf : ..\truetype\DejaVuSans.ttf
	copy ..\truetype\DejaVuSans.ttf $@

$(TARGETDIR)\DejaVuSans-Bold.ttf : ..\truetype\DejaVuSans-Bold.ttf
	copy ..\truetype\DejaVuSans-Bold.ttf $@

$(TARGETDIR)\DejaVuSans-Oblique.ttf : ..\truetype\DejaVuSans-Oblique.ttf
	copy ..\truetype\DejaVuSans-Oblique.ttf $@

$(TARGETDIR)\DejaVuSans-BoldOblique.ttf : ..\truetype\DejaVuSans-BoldOblique.ttf
	copy ..\truetype\DejaVuSans-BoldOblique.ttf $@

$(TARGETDIR)\DejaVuSans-ExtraLight.ttf : ..\truetype\DejaVuSans-ExtraLight.ttf
	copy ..\truetype\DejaVuSans-ExtraLight.ttf $@

$(TARGETDIR)\DejaVuSansCondensed.ttf : ..\truetype\DejaVuSansCondensed.ttf
	copy ..\truetype\DejaVuSansCondensed.ttf $@

$(TARGETDIR)\DejaVuSansCondensed-Bold.ttf : ..\truetype\DejaVuSansCondensed-Bold.ttf
	copy ..\truetype\DejaVuSansCondensed-Bold.ttf $@

$(TARGETDIR)\DejaVuSansCondensed-Oblique.ttf : ..\truetype\DejaVuSansCondensed-Oblique.ttf
	copy ..\truetype\DejaVuSansCondensed-Oblique.ttf $@

$(TARGETDIR)\DejaVuSansCondensed-BoldOblique.ttf : ..\truetype\DejaVuSansCondensed-BoldOblique.ttf
	copy ..\truetype\DejaVuSansCondensed-BoldOblique.ttf $@

$(TARGETDIR)\DejaVuSansMono.ttf : ..\truetype\DejaVuSansMono.ttf
	copy ..\truetype\DejaVuSansMono.ttf $@

$(TARGETDIR)\DejaVuSansMono-Bold.ttf : ..\truetype\DejaVuSansMono-Bold.ttf
	copy ..\truetype\DejaVuSansMono-Bold.ttf $@

$(TARGETDIR)\DejaVuSansMono-Oblique.ttf : ..\truetype\DejaVuSansMono-Oblique.ttf
	copy ..\truetype\DejaVuSansMono-Oblique.ttf $@

$(TARGETDIR)\DejaVuSansMono-BoldOblique.ttf : ..\truetype\DejaVuSansMono-BoldOblique.ttf
	copy ..\truetype\DejaVuSansMono-BoldOblique.ttf $@

$(TARGETDIR)\DejaVuSerif.ttf : ..\truetype\DejaVuSerif.ttf
	copy ..\truetype\DejaVuSerif.ttf $@

$(TARGETDIR)\DejaVuSerif-Bold.ttf : ..\truetype\DejaVuSerif-Bold.ttf
	copy ..\truetype\DejaVuSerif-Bold.ttf $@

$(TARGETDIR)\DejaVuSerif-Italic.ttf : ..\truetype\DejaVuSerif-Italic.ttf
	copy ..\truetype\DejaVuSerif-Italic.ttf $@

$(TARGETDIR)\DejaVuSerif-BoldItalic.ttf : ..\truetype\DejaVuSerif-BoldItalic.ttf
	copy ..\truetype\DejaVuSerif-BoldItalic.ttf $@

$(TARGETDIR)\DejaVuSerifCondensed.ttf : ..\truetype\DejaVuSerifCondensed.ttf
	copy ..\truetype\DejaVuSerifCondensed.ttf $@

$(TARGETDIR)\DejaVuSerifCondensed-Bold.ttf : ..\truetype\DejaVuSerifCondensed-Bold.ttf
	copy ..\truetype\DejaVuSerifCondensed-Bold.ttf $@

$(TARGETDIR)\DejaVuSerifCondensed-Italic.ttf : ..\truetype\DejaVuSerifCondensed-Italic.ttf
	copy ..\truetype\DejaVuSerifCondensed-Italic.ttf $@

$(TARGETDIR)\DejaVuSerifCondensed-BoldItalic.ttf : ..\truetype\DejaVuSerifCondensed-BoldItalic.ttf
	copy ..\truetype\DejaVuSerifCondensed-BoldItalic.ttf $@

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

