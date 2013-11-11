#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
# This script scans epocwind.out log files resulting from an automated emulator test run
# and checks for panics.  Files to skip or those with expected panics can be identified
# in the hash table "expectedPanicsHash".
# If looks for logs in %EPOCROOT%\logs\winscw - rhe DABS scripts will rename epocwind.out
# files for all tests to end in "_epocwind.txt" so this script scans only files matching
# this pattern.
# If run without arguments the output logfile will be
# %EPOCROOT%\epoc32\winscw\c\panicscanlog.txt but a log path/name can be specified as the
# only command line argument.
#


use Cwd;
use strict;
use Getopt::Std;
use File::Basename;
use File::Copy;     # for future portability

# Hash entries are in the form <skip check>, ([pattern1, expectedCount1], [pattern2, expectedCount2],...)
# where anything non-zero for the first entry will skip the check for the file.
# The match patterns are regular expressions.

my %expectedPanicsHash = ();

# TCAF test expects CafUtils 0 panics
push(@{$expectedPanicsHash{"TCAF_epocwind.txt"}}, (0,
	("Thread tcaf.exe::Worker.*Panic CafUtils 0",6)));

# tjavahelperserver test expects 4 kern-exec 0 panics
push(@{$expectedPanicsHash{"tjavahelperserver_epocwind.txt"}}, (0,
	("Thread tjavahelperserver.exe::Worker.*Panic KERN-EXEC 0", 5)));

# Authserver test should be uncommented when it is released.
# authserver related tests expect panics
#push(@{$expectedPanicsHash{"tauthexpr_epocwind.txt"}}, (0,
#	("Thread tauthcliserv.exe::Worker.*Panic AuthServer 3", 1)));
#push(@{$expectedPanicsHash{"tauthcliserv_debug_epocwind.txt"}}, (0,
#	("Thread tauthcliserv.exe::Worker.*Panic AUTHEXPR 64", 3)));
#push(@{$expectedPanicsHash{"tauthsvr2_epocwind.txt"}}, (0,
#	("Thread AuthServer.EXE::Main Panic AuthServer 5", 2)));

# crypto - padding related tests expect panics
push(@{$expectedPanicsHash{"tpaddingudeb_epocwind.txt"}}, (0,
	("Thread tpaddingserver.exe::Worker.*Panic CRYPTO-LIB 1", 1)));

# Expected UPS panics
push(@{$expectedPanicsHash{"tpolicycache_epocwind.txt"}}, (0,
	("Thread tupspolicies.exe::Worker.*Panic UPS-Policies 0", 2)));
push(@{$expectedPanicsHash{"tserviceconfig_epocwind.txt"}}, (0,
	("Thread tupspolicies.exe::Worker.*Panic UPS-Policies 0", 1)));
push(@{$expectedPanicsHash{"scstest_epocwind.txt"}}, 
    (0,
	   ("Thread scstestserver.exe::Main Panic SCS-Server 0", 1),
       ("Thread scstest.exe::ScsTestPanic Panic SCS-Client 0", 1),
       ("Thread scstest.exe::ScsTestPanic Panic SCS-Client 1", 1),
       ("Thread scstest.exe::ScsTestPanic Panic SCS-Client 2", 1)
    ));

die "EPOCROOT not defined, must specify directory" if !defined ($ENV{EPOCROOT});

# Searches for the most recently created Log directory
my @logDirectories = glob("$ENV{EPOCROOT}logs_*\\winscw");
@logDirectories = sort(@logDirectories);

my $emulatorLogDirectory = "$logDirectories[$#logDirectories]";

if ( ! -d $emulatorLogDirectory )
	{
	die "$emulatorLogDirectory is not a directory";
	}

my $outputFile;
if (@ARGV[0])
	{
	$outputFile = $ARGV[0];
	}
else
	{
	$outputFile = "$ENV{EPOCROOT}epoc32\\winscw\\c\\panicscanlog.txt";
	}

unlink $outputFile;
die "\nUnable to open log $outputFile\n" if( not open( SCANLOG, ">$outputFile" ) );


print SCANLOG "\nScanning epocwind.txt files in $emulatorLogDirectory for panics.\n\n";

my $failureCount = 0;
my $skipCount = 0;
my @fileList = getFiles($emulatorLogDirectory, "_epocwind\.txt\$");
my $fileCount = @fileList;

foreach my $file (@fileList)
	{
	print (SCANLOG "$file: \n");
	my @matchPatterns=();
	if (exists $expectedPanicsHash{$file})
		{
		(my $skipFile, @matchPatterns) = @{$expectedPanicsHash{$file}};
		if ($skipFile)
			{
			print (SCANLOG "\tSkipping check.\n\n");
			$skipCount++;
			next;
			}
		}

	my @panicLines = grep(/Thread\s+.+\s+panic/i, ReadListFromFile("$emulatorLogDirectory/$file"));
	my $failed = 0;

	if (@panicLines)
		{
		if (@matchPatterns > 0)
			{
			print(SCANLOG "\tPanics found, checking against expected panics.\n");
			my $j;
			my @remainingCounts=();
			for ($j=1; $j < @matchPatterns; $j=$j+2)
				{
				push @remainingCounts,$matchPatterns[$j];
				}

PANICLOOP:		foreach my $panic (@panicLines)
				{
				chomp $panic;
				for ($j=0; $j < @matchPatterns; $j=$j+2)
					{
					if (grep(/$matchPatterns[$j]/, $panic))
						{
						print (SCANLOG "\t\"$panic\" matches expected pattern.\n");
						$remainingCounts[$j/2]--;
						next PANICLOOP;
						}
					}
				print (SCANLOG "\t\"$panic\" does not match expected patterns\n");
				$failed = 1;
				}

			for ($j=0; $j < @remainingCounts; $j++)
				{
				if ($remainingCounts[$j] != 0)
					{
					$failed = 1;
					my $expectedCount = $matchPatterns[$j*2+1];
					my $actualCount = $expectedCount - $remainingCounts[$j];
					print (SCANLOG "\tExpected $expectedCount occurrences of pattern \"$matchPatterns[$j*2]\", got $actualCount\n");
					}
				}
			}
		else
			{
			print (SCANLOG "\tPanics found and none expected.\n");
			$failed = 1;
			foreach my $panic (@panicLines)
				{
				print (SCANLOG "\t$panic");		
				}
			}
		}
	else
		{
		if (exists $expectedPanicsHash{$file})
			{
			print(SCANLOG "\tPanics expected but none found.\n");
			$failed = 1;
			}
		else
			{
			print(SCANLOG "\tNo panics expected and none found.\n");
			}
		}

	if ($failed)
		{
		print (SCANLOG "\tTest for $file FAILED.\n\n");
		$failureCount++;
		}
	else
		{
		print (SCANLOG "\tTest for $file PASSED.\n\n");
		}
	}

if ($skipCount)
	{
	print (SCANLOG "\nSkipped $skipCount files ($fileCount total.)");
	}
my $testedCount = $fileCount - $skipCount;
print (SCANLOG "\nTested $testedCount files.\n");
print (SCANLOG "$failureCount tests failed out of $testedCount \n");

close SCANLOG;

# --------------------------------------------------------------------------

# Return an array of files matching a regexp in a directory
sub getFiles($$) {
    my $dir = shift;
    my $regfiles = shift;
    my @files;

    if ( opendir DIR, $dir ) {
       @files = grep (/$regfiles/, readdir(DIR));
       closedir DIR;
    }
    return @files;
}

# --------------------------------------------------------------------------

# Read the contents of a file into an array and return it
sub ReadListFromFile ($) {
	my ($file) = @_;
	open FILE, "<$file" or die "Can't read from $file: $!";
	my @data = <FILE>;
	close FILE;
	return @data;
}

# --------------------------------------------------------------------------
