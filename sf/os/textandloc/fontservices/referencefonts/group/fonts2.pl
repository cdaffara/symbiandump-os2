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

$bitmapdatadir="/bitmap/fntfiles";
$bitmapdatadir = $parentdir . $bitmapdatadir;

if (-e "$bitmapdatadir")
{ 
  #proceed with your code
}
else
{
	eval { mkpath($bitmapdatadir) };
	if ($@) {
	    print "Couldn't create $dir: $@";
  	} 
}

system("fnttran bitmap/squashed.bdf bitmap/calc35.bdf bitmap/calc13.bdf bitmap/calc18.bdf bitmap/calcinv.bdf bitmap/digit35.bdf bitmap/agnsym11.bdf bitmap/agnsym16.bdf bitmap/eiksym15.bdf bitmap/base.gd $bitmapdatadir/eon14.gdr");

#Use binarycopy procedure defined below to copy the files instead if copy command.
sub binaryCopy
{
	$numOfParams = @_;
	my $buffer;
	
	open OUTF, ">$_[$numOfParams-1]" or die "\nCan't open $_[$numOfParams-1] for writing: $!\n";
	binmode OUTF;

	for($i=0;$i<$numOfParams-1;$i++)
	{
		open INF, $_[$i] or die "\nCan't open $_[$i] for reading: $!\n";	
		binmode INF;
		while	(
			read (INF, $buffer, 65536)	# read in (up to) 64k chunks, write
			and print OUTF $buffer	# exit if read or write fails
			) {};
			die "Problem copying: $!\n" if $!;
			close INF or die "Can't close $_[$i]: $!\n";
	}
	close OUTF or die "Can't close $_[$numOfParams-1]: $!\n";
}