#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Build font files
#

use Cwd;
use File::Path;

$dir = getcwd();
chdir "..";
$parentdir = getcwd();

$datadir="/tfs/fntfiles";
$datadir = $parentdir . $datadir;

if (-e "$datadir")
{ 
  #proceed with your code
}
else
{
	eval { mkpath($datadir) };
	if ($@) {
	    print "Couldn't create $dir: $@";
  	} 
}

system("fnttran tfs/tsection.bdf tfs/tsection.gd $datadir/tsection.gdr");
