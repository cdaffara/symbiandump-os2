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

CPPFLAGS=-MMD -D_DEBUG -D_UNICODE -D__LINUX__ -D__TOOLS2_LINUX__ $(INC)
CXXFLAGS=-g -Wall -W -fstrict-aliasing
CFLAGS=-g -Wall -W -fstrict-aliasing
CXX=g++-3.4
CC=gcc-3.4

TOPTARGETSNORUN=all tags cleanall
TOPTARGETSRUN=run runtapi runapp1 runapp2 runapp3 runtcert runswi
TOPTARGETS=$(TOPTARGETSNORUN) $(TOPTARGETSRUN) 

.PHONY: force

