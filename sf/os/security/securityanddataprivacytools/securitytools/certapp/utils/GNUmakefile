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

include ../GNUmakefile.inc
TARGETLIB=utils.a

INC=-I../store-- -I.

SRCS=utils.cpp logger.cpp stringconv.cpp openssl_license.cpp filestream.cpp

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
