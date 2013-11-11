# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# runcmd log expected-result command
# 
#

use File::Basename;
use File::Path;

($#ARGV >= 3) or die "Usage: runcmd log expected-result command";

($log, $expected, @command) = @ARGV;

$cmd = "";
for my $c (@command) {$cmd = $cmd.$c." "};

print "Executing: ".$cmd."\n";
$ret = system($cmd) >> 8;

mkpath(dirname($log));
open LOG, ">>".$log;
print LOG "$cmd\n";
if ($ret == $expected) {
	print LOG "PASS\n";
}
else {
	print LOG "FAIL\n";
}
close LOG;
