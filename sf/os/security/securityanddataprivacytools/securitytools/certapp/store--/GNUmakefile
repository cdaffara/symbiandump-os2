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
TARGETLIB=	store--.a

INC=-I../utils -I.

SRCS=e32cons.cpp us_func.cpp e32base.cpp s32file.cpp s32buf.cpp us_ucmp.cpp utf.cpp

OBJS=$(SRCS:.cpp=.o)
DEPENDFILES=$(SRCS:.cpp=.d)


$(TARGETLIB) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(TOPTARGETS): % :
	$(MAKE) -C.. $@


.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGETLIB) $(DEPENDFILES)


include $(wildcard $(DEPENDFILES))
