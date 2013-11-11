#
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
#

require tefcmp;
( $argv1, $argv2) = @ARGV;

$_ = @ARGV;
if ( ( @ARGV < 2 ) || (/(\-h|\-\-help|\/\?|\?)/) ) {
	Usage();
}

compareBase2DABS($argv1, $argv2);
print "\nDone." . "\nChecking dailyregresultfile.txt at current dir.\n"; 


sub Usage () {
	print <<ENDHERESTRING;
Author : Shuo Huang
	
Common usage : TEFCmp [BaseDir] [DABSDir]
   Compare BaseDir to DABSDir. The compare result output at curernt directory, \
   file name is \"dailyregresultfile.txt\". The file will be overwrite eachtime.

ENDHERESTRING
	exit 1;
}