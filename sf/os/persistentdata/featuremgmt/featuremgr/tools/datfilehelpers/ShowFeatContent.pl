#!perl -w
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This simple script shows how to display the content of a feature manager
# data file.
# 
#

use strict;
use fmcreate;

#
# Find out what file the user is interested in..
# Make sure it's specified and exists.
#
my $datfile = $ARGV[-1];
die "Usage: showfeatcontent.pl <featmanager-data-filename>"
    unless(defined($datfile));
die "Specify an existing file" unless(-f $datfile);

#
# Create an object that represents a feature data file.
#
my $fmc = FMCreate->new();

#
# Load the content of the data file into our FMCreate object.
# Note that this will die if the content does not seem to be a feature set
# file. This can happen if the first four bytes aren't 'feat' or if reading
# the file fails at any point. This will also happen if the file is the wrong
# size.
#
$fmc->LoadUp($datfile);

#
# Display the content.
#
$fmc->ShowALL();
