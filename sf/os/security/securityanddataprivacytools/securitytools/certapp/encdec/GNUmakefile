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

include ../GNUmakefile.inc
TARGETLIB=encdec.a

INC=-I../store-- -I../utils -I.

SRCS=encdec.cpp certclients.cpp filecertstore.cpp certinfo.cpp appuidmap.cpp x509utils.cpp swicertstore.cpp capabilityset.cpp

OBJS=$(SRCS:.cpp=.o)
DEPENDFILES=$(SRCS:.cpp=.d)


$(TARGETLIB) : $(OBJS) $(LDLIBS)
	$(AR) $(ARFLAGS) $@ $^

$(TOPTARGETS): % :
	$(MAKE) -C.. $@

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGETLIB) $(DEPENDFILES)


include $(wildcard $(DEPENDFILES))
