#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#


use strict;
use Getopt::Long;
use IO::Handle;
use IO::Seekable;
use File::Basename;

main();
exit(0);

sub main {
	if ($#ARGV != 0)
	{
		print "usage: newdigest.pl <logfile>\n";
		exit;
	}
	
	my ($log_file) = $ARGV[0];
	
	if (-f $log_file)
	{
		system ("del $log_file");
	}
	
	opendir( DIR, "." ) or die("Unable to open directory");
	my @dirs = readdir DIR;
	my $entry;
	
	my $nTotalTestCase = 0;
	my $nTotalPassedCase = 0;
	my $nTotalFailedCase = 0;
	my $nTotalPassedStep = 0;
	my $nTotalFailedStep = 0;
	my $scriptsRun = 0;
	open (SUMMARY, ">$log_file") || die "couldn't open digest file: $!";
	
	foreach $entry ( sort @dirs )
	{
		next if ( $entry eq ".." or $entry eq "." );
		next if ( -d $entry );

		my $failed = 0;
		my $passed = 0;
		my $ntestcasepassed = 0;
		my $ntestcasefailed = 0;

		if ( $entry =~ /\.log/ || $entry =~ /\.txt/)
		{
			parseLog($entry, \$failed, \$passed);
			#printf SUMMARY (" %45s: %d tests failed out of %d\r\n", $entry, $failed, $failed+$passed);			
		}
                
 		elsif ($entry =~ /\.htm/) 
 		{
		    open (HTML_LOG, "$entry" ) or die("open failed");
            my $Is_Tef_Htm = 1; 

            while(<HTML_LOG>)
		    {
			chomp;
	
			my $line  = $_;
			$line =~ s/\x0//g;

			if ($line =~ /tests failed/)
			{
              parseLog($entry, \$failed, \$passed);
 			  #printf SUMMARY (" %45s: %d tests failed out of %d\r\n", $entry, $failed, $failed+$passed);			
              $Is_Tef_Htm = 0;
            }

            }

	 	    close HTML_LOG;                    
	        if( $Is_Tef_Htm )
            {
	 	      parseHtm($entry, \$failed, \$passed, \$ntestcasefailed, \$ntestcasepassed);
            }        
		}
		if ( $passed > 0 or $failed > 0)
		{
			$scriptsRun++;
			$nTotalPassedStep += $passed;
			$nTotalFailedStep += $failed;

			if ( $ntestcasepassed > 0 || $ntestcasefailed > 0 )
			{
			$nTotalPassedCase += $ntestcasepassed;
			$nTotalFailedCase += $ntestcasefailed;
			}
			else
			{
			if ( $failed > 0 )
			{
			$nTotalFailedCase++;
			}
			else
			{
			$nTotalPassedCase++;
			}
			}
		}
	}
	my $ntestScripts = " Test Scripts Run \t";
	print SUMMARY "\n";	
	print SUMMARY ($scriptsRun, $ntestScripts);
	
	my $nTotalTestCases = $nTotalFailedCase+$nTotalPassedCase;	
	print SUMMARY "\n";	
	print SUMMARY "\r\nTest Cases\tPassed\tFailed\tTotal\r\n";
	printf SUMMARY ("\t\t%d\t%d\t%d\n", $nTotalPassedCase, $nTotalFailedCase, $nTotalTestCases);
	if ($nTotalTestCases > 0)
	{
	print SUMMARY "\r\n\t\t%Passed\t\t%Failed\r\n";	
	printf SUMMARY ("\t\t%.2f\t\t%.2f\r\n", $nTotalPassedCase/$nTotalTestCases*100, $nTotalFailedCase/$nTotalTestCases*100);	
	}
	my $nTotalTestSteps = $nTotalPassedStep+$nTotalFailedStep;
	print SUMMARY "\n";
	print SUMMARY "\r\nTest Steps\tPassed\tFailed\tTotal\r\n";
	printf SUMMARY ("\t\t%d\t%d\t%d\n", $nTotalPassedStep, $nTotalFailedStep, $nTotalTestSteps);
	if ($nTotalTestSteps > 0)
	{
	print SUMMARY "\r\n\t\t%Passed\t\t%Failed\r\n";
	printf SUMMARY ("\t\t%.2f\t\t%.2f\r\n", $nTotalPassedStep/$nTotalTestSteps*100, $nTotalFailedStep/$nTotalTestSteps*100);
	}
}


sub parseLog($$$) {
	my ($entry, $failed, $passed) = @_;

    open (LOG, "$entry" ) or die("open failed");
    my $found = "false";
	my $notests = 0;
    while(<LOG>)
    {
	chomp;
	
	my $line  = $_;
	$line =~ s/\x0//g;
	if ($line =~ /tests failed/)
	{
	    $line =~ /(\d+) tests failed out of (\d+)/;
	    printf SUMMARY (" %45s: %s\r\n", $entry, $&);
	    $found = "true";
	    $$failed = $1;
	    $notests = $2;
	}
    }
    close LOG;

    if ( $found eq "false" && $entry ne "buildid.txt")
    {
	printf SUMMARY ("WARNING: Could not parse file %s\r\n", $entry);
    }
	$$passed = $notests - $$failed;
}

sub parseHtm($$$$$) {
	my ($entry, $nfailed, $npassed, $nCasesFailed, $nCasesPassed) = @_;
    my ($textfile);
    $textfile = $entry;
    $textfile =~ s/\.htm/\.log/;
    next if ( -f $textfile);
	
    my $ncasestotal = 0;
	my $nstepstotal = 0;
	my $passed = 0;
	my $failed = 0;
	my $casepassed = 0;
	my $casefailed = 0;
	my $TEST_STEP_SECTION = 0;
	my $TEST_CASE_SECTION = 0;
	my $noTestCase = 0;
	
	
    open (LOG, $entry) || die "couldn't open $entry: $!";
    while (<LOG>)
    {
		my ($sLine) = $_;
	
		if ( $sLine =~ /TEST STEP SUMMARY:<\/font>/i) 
		{
			$TEST_STEP_SECTION = 1;
			$TEST_CASE_SECTION = 0;
		}
		elsif ( $sLine =~ /RUN PROGRAM SUMMARY:<\/font>/i) 
		{
			$TEST_STEP_SECTION = 0;
		}
	        elsif( $sLine =~ /TEST CASE SUMMARY:<\/font>/i)
		{
			$TEST_CASE_SECTION = 1;
			$TEST_STEP_SECTION = 0;
		}		
		elsif ( $sLine =~ /SUMMARY:<\/font>/i ) # To Keep Last
		{
			$TEST_STEP_SECTION = 1;
			$noTestCase = 1;
		}
	
		if ( $TEST_STEP_SECTION )
		{
			$passed += $1 if( $sLine =~ /<font.*color=00AF00> PASS = (\d+)<\/font>/i);
			$passed += $1 if( $sLine =~ /<font.*color=00AF00>PASS = (\d+)<\/font>/i);
			$failed += $1 if( $sLine =~ /<font.*color=FF0000>FAIL = (\d+)<\/font>/i);
			$failed += $1 if( $sLine =~ /<font.*color=0000FF>ABORT = (\d+)<\/font>/i);
			$failed += $1 if( $sLine =~ /<font.*color=0000FF>PANIC = (\d+)<\/font>/i);
			$failed += $1 if( $sLine =~ /<font.*color=0000FF>INCONCLUSIVE = (\d+)<\/font>/i);
			$failed += $1 if( $sLine =~ /<font.*color=0000FF>UNKNOWN = (\d+)<\/font>/i);
			$failed += $1 if( $sLine =~ /<font.*color=0000FF>UNEXECUTED = (\d+)<\/font>/i);
		}
		if ( $TEST_CASE_SECTION )
		{
			$casepassed += $1 if( $sLine =~ /<font.*color=00AF00> PASS = (\d+)<\/font>/i);
			$casepassed += $1 if( $sLine =~ /<font.*color=00AF00>PASS = (\d+)<\/font>/i);
			$casefailed += $1 if( $sLine =~ /<font.*color=FF0000>FAIL = (\d+)<\/font>/i);
			$casefailed += $1 if( $sLine =~ /<font.*color=0000FF>ABORT = (\d+)<\/font>/i);
			$casefailed += $1 if( $sLine =~ /<font.*color=0000FF>PANIC = (\d+)<\/font>/i);
			$casefailed += $1 if( $sLine =~ /<font.*color=0000FF>INCONCLUSIVE = (\d+)<\/font>/i);
			$casefailed += $1 if( $sLine =~ /<font.*color=0000FF>UNKNOWN = (\d+)<\/font>/i);
			$casefailed += $1 if( $sLine =~ /<font.*color=0000FF>UNEXECUTED = (\d+)<\/font>/i);
		}
    }
	$ncasestotal = $casefailed + $casepassed;
	$nstepstotal = $passed + $failed;
	
    close LOG;
	if($ncasestotal != 0 or $noTestCase == 1) 
	{
	  printf SUMMARY (" %45s: %d tests failed out of %d\r\n", $entry, $failed, $nstepstotal);
	}
	else
	{
	  printf SUMMARY ("WARNING: %45s: %d tests failed out of %d\r\n", $entry, $failed, $nstepstotal);
	}
	$$npassed = $passed;
	$$nfailed = $failed;
	$$nCasesPassed = $casepassed;
	$$nCasesFailed = $casefailed;
}






