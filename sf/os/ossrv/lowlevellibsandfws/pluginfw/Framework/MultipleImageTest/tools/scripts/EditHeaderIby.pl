#!perl

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
#

use strict;
my $target = "\\epoc32\\rom\\include\\header.iby";

# Read header.iby to see if it has already been modified.
my @lines = ();
open(FH, $target) or die "Fail to open $target $!\n";
while(<FH>)
{
  push(@lines, $_);
  if (/rom_image\s+2\s+rofs2/i)
  {
	close(FH);
	exit(0);
  }
}
close(FH);

chmod(0666, $target);
open(FH, ">$target") or die "Fail to open $target for write $!\n";

foreach (@lines)
{
  print FH $_;
  if (/rom_image\s+1\s+rofs\b/i)
  {
	print FH "rom_image 2 rofs2 size=1000000 non-xip compress\n";
  }
}
close(FH);
