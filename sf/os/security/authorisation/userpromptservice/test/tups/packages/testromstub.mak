ifeq "$(PLATFORM)" "WINSCW"
ZDIR:=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
SISFILE=$(ZDIR)\system\install\testupsromstub.sis
endif

ifeq "$(PLATFORM)" "ARMV5"
ZDIR:=$(EPOCROOT)epoc32\data\z
SISFILE=$(ZDIR)\system\install\testupsromstub.sis
endif

#
# The targets invoked by abld...
#

MAKMAKE : DO_NOTHING
FREEZE : DO_NOTHING
LIB : DO_NOTHING
RESOURCE : DO_NOTHING
CLEANLIB : DO_NOTHING
FINAL : DO_NOTHING
SAVESPACE : BLD
BLD : MAKEDATA

DO_NOTHING:


ifndef SISFILE
#
# Not supported on this target so nothing to do
#
MAKEDATA :
	@echo ----------------
	@echo  Do nothing ...
	@echo ----------------

CLEAN:

RELEASABLES:

else
#
# WINSCW or ARM
#
MAKEDATA : $(SISFILE)

$(SISFILE) : testupsromstub.pkg
	@cd
	@echo ---------------------------
	@echo Building Test UPS ROM stub
	@echo ---------------------------
	@echo nb. The stub does not contain any exes, just mentions them.
	makesis -s $< $@

DO_NOTHING:
	rem do nothing

CLEAN :     
	-@erase $(SISFILE) /f

RELEASABLES :
	@echo $(SISFILE)
endif
