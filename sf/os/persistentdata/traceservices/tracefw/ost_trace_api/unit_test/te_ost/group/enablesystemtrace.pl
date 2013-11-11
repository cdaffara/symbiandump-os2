#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

use warnings;
use strict;

#print "This assumes the epoc32 folder is always at the root of the drive. If not then you have to edit this perl script.\n\n";

#my $root = "";
#if($ARGV[0])
#	{
#	$root = $ARGV[0];
#	}
#$root=~s/\//\\/g;
#$root=~s/\\+/\\/g;
#$root .= "\\" if($root!~m/\\$/);
#my @hrhFile = "$root\\epoc32\\include\\variant\\Symbian_OS.hrh";

my @hrhFile = "\\epoc32\\include\\variant\\Symbian_OS.hrh";
open OUT, ">>@hrhFile";
my $traceAddition = "\n\n/**\n\@publishedPartner\n\@prototype\n\nSYMBIAN_INCLUDE_SYSTEM_TRACE\n\nDescription:\nIf SYMBIAN_INCLUDE_SYSTEM_TRACE is defined, components wishing to output UTF trace \nrecords from their source will use an enabled version of UTF rather than a stubbed \nversion.\n*/\n#define SYMBIAN_INCLUDE_SYSTEM_TRACE\n";
print OUT $traceAddition;
print "Added $traceAddition \nto @hrhFile";