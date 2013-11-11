# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

include GNUmakefile.inc

INC=-Iutils -Istore-- -Iencdec -Iapi -I.

CERTAPP_TARGET=	certapp
TARGETS=$(CERTAPP_TARGET) 

CERTAPP_SRCS=certapp.cpp
CERTAPP_OBJS=$(CERTAPP_SRCS:.cpp=.o)

ALLSRCS=$(CERTAPP_SRCS)
ALLOBJS=$(ALLSRCS:.cpp=.o)

LDLIBS_GEN=encdec/encdec.a store--/store--.a utils/utils.a 
LDLIBS_API=api/libcertapp-api.a

DEPENDFILES=$(ALLOBJS:.o=.d)


all: $(TARGETS)
	make -C test


$(LDLIBS_GEN) $(LDLIBS_API): % : force
	$(MAKE) -C $(dir $@) $(notdir $@)


tags:
	etags *.cpp *.h *.inl */*.cpp */*.h */*.inl */*/*.cpp */*/*.h
	ls -l TAGS


$(CERTAPP_TARGET) : $(CERTAPP_OBJS) $(LDLIBS_GEN)
	$(LINK.C) $^ $(LOADLIBES) $(LDLIBS_GEN) -o $@ -lcrypto

.PHONY: clean
clean: cleanall

.PHONY: cleanall
cleanall:
	-$(MAKE) -C utils clean
	-$(MAKE) -C store-- clean
	-$(MAKE) -C encdec clean
	-rm -f $(TSTORE_OBJS) $(CERTAPP_OBJS) $(TARGETS) $(DEPENDFILES) core


include $(wildcard $(DEPENDFILES))
