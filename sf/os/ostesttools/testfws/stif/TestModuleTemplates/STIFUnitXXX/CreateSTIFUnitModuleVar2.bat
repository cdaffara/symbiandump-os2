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

@perl -x CreateSTIFUnitModule.bat %1 %2 %3 %4 %5 %6 %7 %8 %9 
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
my $targetDir = $targetPath;

unless(-d $targetDir)
{
  die ("Specified target path does not exists. Cannot create test module.\n");
}

print "Starting module creation to $targetDir\n";
mkdir $targetDir, 0777 || die ("Can't create directory $targetDir");

# Loop through the file structure
find(\&renamerename, '.');

unlink $targetDir."CreateSTIFUnitModuleVar2.bat";
unlink $targetDir."CreateSTIFUnitModule.bat";
unlink $targetDir."/group/$moduleName"."_nrm.mmp";

ModifyBldInf();

print "Module created to $targetDir\n";

# This function will be called for each file or directory
sub renamerename
{
	my $oldName = $_;
	print "Processing $oldName\n";

	# Construct new filename if that needed
	s/STIFUnitXXX/$moduleName/i;
	my $newName = $targetDir.$File::Find::dir."/".$_;  

	# Process directories
	if (opendir(DIR, $oldName))
	{
		closedir (DIR);
		mkdir $newName, 0777 || die ("Can't create directory $newName");
		return;
	}
  
  if (uc($oldName) eq "BLD.INF")
  {
    print "Skipping $oldName\n";
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
	  s/STIFUnitXXX/$moduleName/g;
	  s/STIFUNITXXX/$MODULENAME/g;
	  s/XXX/$moduleName/g;
	  s/STIFUnitYYY/$targetDir/g;
	  print OUTFILE $_;
	}

	# Close filehandles
	close (INFILE);
	close (OUTFILE);

	# Rename result file
	rename $newOutput,$newName;
}

sub ModifyBldInf()
{
  print "Modifying bld.inf of existing project\n";
  my $bldName = $targetDir."group/Bld.inf";
  my $newBldName = $targetDir."group/Bld.inf.new";
  my $found = 0;
  
  open INFILE, $bldName or die "Cannot open $bldName: $!";
  open (OUTFILE, ">".$newBldName) or die "Cannot create $newBldName: $!";
  
  while (<INFILE>) 
  {
    print OUTFILE $_ unless /^warning:/i;
    if ($found eq 0 && trim(uc($_)) eq "PRJ_TESTMMPFILES")
    {
      print OUTFILE "$moduleName.mmp\n";
      $found = 1;
    }
  }
  if($found eq 0)
  {
    print OUTFILE "PRJ_TESTMMPFILES\n$moduleName.mmp\n";
  }
  
  close INFILE;
  close OUTFILE;
  
  rename $bldName, "$bldName.old" or die "Could not rename $bldName to $bldName.old\n";
  rename $newBldName, $bldName or die "Could not rename $newBldName to $bldName\n";
  
  print "Bld.inf file has been modified.\n";
  print "Original file is stored as $bldName.old\n";
}

sub PrintHelp()
{
	print "createSTIFUnitModule ModuleName [path]\n";
	print "\n";
	print "Creates a new test module\n";
	print "If [path] is not given, module is created to root of current drive.\n";
	print "If [path] is given, it must contain the final \'\\\' in path name.\n";
	print "Command must be executed in directory where the template exist.\n";
	exit;
}

sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

__END__
:end
