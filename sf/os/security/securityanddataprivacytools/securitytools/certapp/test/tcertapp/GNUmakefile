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

include ../../GNUmakefile.inc
TARGET=tcertapp

INC=-I../../api -I../../utils -I.

SRCS=tcertapp.cpp tcertapp_bad.cpp tcertapp_good.cpp badconfigwriter.cpp goodconfigwriter.cpp
LOADLIBES=../../api/libcertapp-api.a ../../utils/utils.a

OBJS=$(SRCS:.cpp=.o)
DEPENDFILES=$(SRCS:.cpp=.d)


$(TARGET) : $(OBJS)
	$(LINK.C) $^ $(LOADLIBES) -o $@


$(TOPTARGETS): % :
	$(MAKE) -C../.. $@


.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET) $(DEPENDFILES)


include $(wildcard $(DEPENDFILES))
