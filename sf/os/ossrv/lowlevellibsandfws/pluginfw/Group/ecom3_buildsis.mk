# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Params:
# SOURCES - list of .pkg files
# TARGET - not used
# OPTION OUTDIR - mandatory, it is tef_ecomswi
# OPTION INDIR - mandatory, path relative to bld.inf containing the 
# .pkg files.
# OPTION CERTPEM - mandatory
# OPTION CERTKEY - mandatory
# OPTION STUBLIST - mandatory, identify stubs in $(SOURCES)
# OPTION SCRIPTNAME - mandatory, perl script to build SIS. Must be in
# same dir as .pkg files.
# 
#

.SUFFIXES: .sis .pkg
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/


include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

CERTPEM := $(subst /,$(/),$(CERTPEM))
CERTKEY := $(subst /,$(/),$(CERTKEY))
OUTDIR := $(subst /,$(/),$(OUTDIR))
INDIR := $(subst /,$(/),$(INDIR))
BASEDIR := $(EXTENSION_ROOT)/$(INDIR)

TARGETS := $(shell perl $(BASEDIR)/$(SCRIPTNAME) -platform $(PLATFORM_PATH) -cfg $(CFG_PATH) -outdir $(OUTDIR) -maketrgt RELEASABLES -sources "$(SOURCES)" -stublist "$(STUBLIST)")
EXTRA := $(shell perl $(BASEDIR)/$(SCRIPTNAME) -platform $(PLATFORM_PATH) -cfg $(CFG_PATH) -outdir $(OUTDIR) -maketrgt EXTRATARGET -sources "$(SOURCES)" -stublist "$(STUBLIST)")

#
# The targets invoked by abld...
#
FINAL :
	$(call createdir,$(subst /,$(/),$(EPOCROOT)epoc32/data/z/$(OUTDIR)/$(CFG_PATH)))
	@perl $(call slash2generic, $(BASEDIR)/$(SCRIPTNAME)) -basedir $(BASEDIR) -platform $(PLATFORM_PATH) -cfg $(CFG_PATH) -certpem $(CERTPEM) -certkey $(CERTKEY) -maketrgt FINAL cachedresolverstub.sis
	$(CP) $(subst /,$(/),$(BASEDIR)/cachedresolverstub.sis) $(subst /,$(/),$(EPOCROOT)epoc32/data/z/$(OUTDIR)/cachedresolverstub.sis)
	@perl $(call slash2generic, $(BASEDIR)/$(SCRIPTNAME)) -basedir $(BASEDIR) -platform $(PLATFORM_PATH) -cfg $(CFG_PATH) -certpem $(CERTPEM) -certkey $(CERTKEY) -maketrgt FINAL cachedresolverupg2signed.sis
	$(CP) $(subst /,$(/),$(BASEDIR)/cachedresolverupg2signed.sis) $(subst /,$(/),$(EPOCROOT)epoc32/data/z/$(OUTDIR)/$(CFG_PATH)/cachedresolverupg2signed.sis)
	@perl $(call slash2generic, $(BASEDIR)/$(SCRIPTNAME)) -basedir $(BASEDIR) -platform $(PLATFORM_PATH) -cfg $(CFG_PATH) -certpem $(CERTPEM) -certkey $(CERTKEY) -maketrgt FINAL  cachedresolverupg9signed.sis
	$(CP) $(subst /,$(/),$(BASEDIR)/cachedresolverupg9signed.sis) $(subst /,$(/),$(EPOCROOT)epoc32/data/z/$(OUTDIR)/$(CFG_PATH)/cachedresolverupg9signed.sis)

BLD MAKMAKE SAVESPACE FREEZE LIB CLEANLIB RESOURCE :
	@echo do nothing

CLEAN : 
	-$(ERASE) $(TARGETS) $(EXTRA)

RELEASABLES : 
	@echo $(TARGETS) $(EXTRA)

