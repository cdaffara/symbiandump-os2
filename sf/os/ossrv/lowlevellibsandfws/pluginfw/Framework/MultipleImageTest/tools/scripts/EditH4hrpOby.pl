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
my $target = "\\epoc32\\rom\\include\\h4hrp.oby";

# Read h4hrp.oby to see if it has already been modified.
my @lines = ();
my $lastEndIf;

open(FH, $target) or die "Fail to open $target $!\n";
while(<FH>)
{
  push(@lines, $_);
  $lastEndIf = scalar(@lines) - 1 if (/#endif/i);
  if (/\bLANGUAGE_CODE\s+0\d/i)
  {
    close(FH);
	exit(0);
  }
}
close(FH);

chmod(0666, $target);
open(FH, ">$target") or die "Fail to open $target for write $!\n";

for (my $i = 0; $i < scalar @lines; $i++)
{
  if ($i == $lastEndIf)
  {
	print FH "\nDEFAULT_LANGUAGE 01\n";
	print FH "LANGUAGE_CODE 02\n";
	print FH "LANGUAGE_CODE 03\n\n";
  }
  print FH $lines[$i];
}
close(FH);
