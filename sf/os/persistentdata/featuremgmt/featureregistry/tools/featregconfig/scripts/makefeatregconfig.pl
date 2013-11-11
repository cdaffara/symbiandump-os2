# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Write a Feature Registry Configuration File
# 
#

use strict;

my $record = pack 'a4I3',
	'feat',		# Must be exactly this
	0,			# Must be exactly this
	3,			# feature entry count
	2;			# default-supported range count
# Feature Entries:
$record .= pack 'I6',
	1,			# UID1
	1,			# status of UID1 (bit 0 = 0x1 is set if feature is present)
	59,			# UID2
	3,			# status
	60,			# and so on
	0;			# ...
# Default Supported Ranges:	
$record .= pack 'I2',
	 50,		# Low UID
	 200;		# High UID
# Symbian Essential Default supported range
$record .= pack 'I2',
	 0x10279806,# Low UID
	 0x10281805;# High UID
#$record .= '?' x 66000; # to make the file larger than the limit

my $BINCONFIG;
open ($BINCONFIG, '>featreg.cfg') or die "Could not open binary config file for writing\n";
binmode $BINCONFIG;
syswrite $BINCONFIG, $record, length $record;
close($BINCONFIG);
exit;
