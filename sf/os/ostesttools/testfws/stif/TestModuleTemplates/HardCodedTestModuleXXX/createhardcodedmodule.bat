::
:: Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
:: All rights reserved.
:: This component and the accompanying materials are made available
:: under the terms of "Eclipse Public License v1.0"
:: which accompanies this distribution, and is available
:: at the URL "http://www.eclipse.org/legal/epl-v10.html".
::
:: Initial Contributors:
:: Nokia Corporation - initial contribution.
::
:: Contributors:
:: 
:: Description: This file contains capsmodifier implementation.
::

@perl -x createhardcodedmodule.bat %1 %2 %3 %4 %5 %6 %7 %8 %9 
@goto end

#!perl
use strict;
use File::Find;

# Verify command line parameters
if   ($#ARGV == -1 || $#ARGV > 1 )
{
	PrintHelp();
}

# Take module name
my $moduleName = $ARGV[0];
my $MODULENAME = $moduleName;
$MODULENAME =~ tr/[a-z]/[A-Z]/;

# Take target path or use default
my $targetPath="\\";
if ( $#ARGV == 1 )
{
	$targetPath = $ARGV[1];
}


# Create directory
my $targetDir = $targetPath.$moduleName."\\";

print "Starting module creation to $targetDir\n";
mkdir $targetDir, 0777 || die ("Can't create directory $targetDir");

# Loop through the file structure
find(\&renamerename, '.');

unlink $targetDir."createhardcodedmodule.bat";
print "Module created to $targetDir\n";

# This function will be called for each file or directory
sub renamerename
{
	my $oldName = $_;
	print "Processing $oldName\n";

	# Construct new filename if that needed
	s/HardCodedTestModuleXXX/$moduleName/i;
	my $newName = $targetDir.$File::Find::dir."/".$_;  

	# Process directories
	if (opendir(DIR, $oldName))
	{
		closedir (DIR);
		mkdir $newName, 0777 || die ("Can't create directory $newName");
		return;
	}
                        
	# Open input file
	open (INFILE, $oldName ) || die ("Can not find $oldName");

	#Open output file
	my $newOutput = $newName."new";
	open (OUTFILE, ">".$newOutput ) || die ("Can not open $newOutput");

	# Replace text in files
	while (<INFILE>)
	{
	  s/HardCodedTestModuleXXX/$moduleName/g;
	  s/HARDCODEDTESTMODULEXXX/$MODULENAME/g;
	  s/XXX/$moduleName/g;
	  s/HardCodedTargetDirYYY/$targetDir/g;
	  print OUTFILE $_;
	}

	# Close filehandles
	close (INFILE);
	close (OUTFILE);

	# Rename result file
	rename $newOutput,$newName;
}

sub PrintHelp()
{
	print "createhardcodedmodule ModuleName [path]\n";
	print "\n";
	print "Creates a new test module\n";
	print "If [path] is not given, module is created to root of current drive.\n";
	print "If [path] is given, it must contain the final \'\\\' in path name.\n";
	print "Command must be executed in directory where the template exist.\n";
	exit;
}


__END__
:end
