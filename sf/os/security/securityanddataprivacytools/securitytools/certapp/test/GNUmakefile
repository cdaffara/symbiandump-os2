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
SUBDIRS= tcertapp

INC=-I../store-- -I../utils -I../api -I.

ALLSRCS=
ALLOBJS=$(ALLSRCS:.cpp=.o)
DEPENDFILES=$(ALLSRCS:.cpp=.d)

.PHONY:targets
targets: $(SUBDIRS)

$(SUBDIRS) : % : force
	$(MAKE) -C $@ $@

$(TOPTARGETSNORUN): % :
	$(MAKE) -C.. $@


.PHONY: clean
clean:
	-rm -f $(TCERT_TARGET) $(TAPI_TARGET) $(ALLOBJS) $(DEPENDFILES)
	for d in $(SUBDIRS) ; do make -C $$d clean ; done

include $(wildcard $(DEPENDFILES))
