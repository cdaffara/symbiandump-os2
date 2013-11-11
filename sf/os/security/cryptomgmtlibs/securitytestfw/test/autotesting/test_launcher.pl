#
# Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# This script runs security tests for chosen components on a local machine without DABS scripts and ONB. Follow the steps below
# in order to launch tests for a security component:
# 1. Choose a local drive to be used, e.g. p:
# 2. Download a CBR onto the chosen drive.
# 3. Download the security source tree onto the chosen drive, and ensure that your environment variable 
# SECURITYSOURCEDIR points to the relevant root directory.  It helps to use a drive independant value here 
# as this will allow concurrent builds and test runs on seperate drives (assuming the source code has the same path in each case).
# 4. Download ExecTimeOut.exe, which is stored at //PR/share/DABSRelease/buildscripts/, onto the relevant location
# (by default this is the chosen drive's root).  
# 5. Build the security code.  Note that builds require the SECURITYSOURCEDIR environment variable to be set.
# 6. Run or modify the relevant wrapper batch file for your component, which will be stored in the root of the test directory.
# Components Summary
# This script works fine with the following components: Asnpkcs, Caf2, Certman, Common, Crypto, Cryptospi, CryptoTokens,
# FileTokens, Sistools, Streamingcaf, Swi, Switools, Ups. Other components must be tested and therefore 
# further extensions may be needed.
#

use Cwd;
use Getopt::Long;

########################### Start of Input Reading Block ###########################

# Check environment variable SECURITYSOURCEDIR is correctly set
if ($ENV{'SECURITYSOURCEDIR'} eq "")
	{
	die "ERROR: The SECURITYSOURCEDIR environment variable has not been set.  Set this to point to the root of the security source tree on your drive and rebuild the components you want to test.  If you want to perform concurrent builds/tests use a drive relative value and use the same directory structure on each drive.";
	}
else
	{
	my $current_dir = getcwd();
	$current_dir =~ /^(\D):/;	# $1 = driveletter
	
	my $current_drive = lc $1;	

	$ENV{'SECURITYSOURCEDIR'} =~ /^(\D):/;	# $1 = driveletter if it exists
	my $sourcedir_drive =  lc $1;

	if (($sourcedir_drive ne "") && ($sourcedir_drive ne $current_drive))
		{
		print "WARNING: Your SECURITYSOURCEDIR environment variable is set to an absolute path and points to a different drive than the current one\n" . "SECURITYSOURCEDIR=  " . $ENV{'SECURITYSOURCEDIR'} . "\nAre you sure you want to continue (Y/N)?\n";
		my $user_decision = <STDIN>;
		$user_decision = lc $user_decision;
		$user_decision =~ /^(.*)\n/;
		if (($1 ne "y") && ($1 ne "yes"))
			{
			die "User decided not to continue, quiting...";
			}
		}
	}

# Read in command line arguments
my $testedComponentsRaw;
my @testedComponents;
my $testedReleasesRaw;
my @testedReleases;
my $ExecTimeOut;
my $resultsOutputDir;
my $skipCleanAfterFinalComponent;
my $suppressBrowserPopup;
my $displayHelp;

GetOptions("components=s" => \$testedComponentsRaw, 
	   "testReleases=s" => \$testedReleasesRaw,
	   "resultsoutput=s" => \$resultsOutputDir,
	   "exectimeout=s" => \$ExecTimeOut,
	   "suppressbrowser" => \$suppressBrowserPopup,
	   "skipfinalclean" => \$skipCleanAfterFinalComponent,
	   "help" => \$displayHelp);

if ($displayHelp)
{
	DisplayHelp();
}

# Ensure necessary arguments are present
$shouldIDie = 0;

while ($testedComponentsRaw =~ s/([^,]*),//)
{
	push(@testedComponents, $1);
}			
if ($testedComponentsRaw =~ /\S/)
{	
	push(@testedComponents, $testedComponentsRaw);
}
else
{
	print "ERROR: Components to be tested not listed (e.g. -components caf2,streamingcaf).\n";
	$shouldIDie = 1;
}

while ($testedReleasesRaw =~ s/([^,]*),//)
{
	push(@testedReleases, $1);
}
if ($testedReleasesRaw =~ /\S/)
{
	push(@testedReleases, $testedReleasesRaw);
}
else
{			
	print "ERROR: Releases to be tested not listed (e.g. -testReleases udeb,urel).\n";
	$shouldIDie = 1;		
}


if (!defined $ExecTimeOut)
{
	print "ERROR: Location of ExecTimeOut.exe not specified (e.g. -exectimeout \\ExecTimeOut.exe).\n";
	$shouldIDie = 1;
}
if (!defined $resultsOutputDir)
{
	print "ERROR: Directory to place test results not specified (e.g. -resultsoutput \\logs\\winscw).\n";
	$shouldIDie = 1;
}

if ($shouldIDie)
{
	die "Input parameters incorrect, exiting.";
}



my $buildTarget = 'winscw';	# At the moment the script only works for winscw
my $testSpec = "$ENV{SECURITYSOURCEDIR}\\testframework\\test\\autotesting\\test_spec.txt";
my $testResultsPath = "$resultsOutputDir\\TestResults_Summary.html";
############################ End of Input Reading Block ############################
 

use Text::ParseWords;
use File::Path 'rmtree';

# helper functions
sub ExecCmd;

#  helper functions taken from the TestUtilities.pm perl module
sub FullWin32Path;
sub FullPathToCDrive;
sub FullPathToZDrive;
sub DigestSecurityTestLog;
sub DigestTestExecuteTestLog;

# create/empty directory for epoc window output files
ExecCmd("rmdir /s /q $resultsOutputDir");
ExecCmd("mkdir $resultsOutputDir");

# set the location of epoc window output
my $tempDir=$ENV{'TMP'};
my $epocWndPath = $tempDir.'\epocwind.out';

ExecCmd('mkdir \epoc32\drive_d');
ExecCmd('mkdir \epoc32\drive_e');
ExecCmd('mkdir \epoc32\drive_r');

if (!grep(/PlatSecDisabledCaps SwEvent/, ReadFile('\epoc32\data\epoc.ini')))
	{
	# set up drives for emulator	
	ExecCmd('echo _epoc_drive_d \epoc32\drive_d >> \epoc32\data\epoc.ini');
	ExecCmd('echo _epoc_drive_e \epoc32\drive_e >> \epoc32\data\epoc.ini');
	ExecCmd('echo _epoc_drive_r \epoc32\drive_r >> \epoc32\data\epoc.ini');

	# disable the SwEvent to test SWI's handling of loader grantable capabilities
	ExecCmd('echo PlatSecDisabledCaps SwEvent >> \epoc32\data\epoc.ini');

	# turn off panic dialogs requiring user interaction
	ExecCmd('echo JustInTime none >> \epoc32\data\epoc.ini');
	}

# remove watchers
ExecCmd('del /Q /F /S \epoc32\release\wins\*watcher.dll');
ExecCmd('del /Q /F /S \epoc32\release\winscw\*watcher.dll');

# replace custom Java Installer with test version
my $cmiPath = "\\epoc32\\release\\$buildTarget\\$testedRelease";
if ( -f "$cmiPath\\custommidletinstall.dll" && -f "$cmiPath\\tcustommidletinstall.dll" )
{
    ExecCmd("ren $path\\custommidletinstall.dll custommidletinstall.std");
    ExecCmd("copy $path\\tcustommidletinstall.dll $path\\custommidletinstall.dll");
}

rmtree("$ENV{'EPOCROOT'}backup\\wins\\c");
rmtree("$ENV{'EPOCROOT'}backup\\winscw\\c");
rmtree("$ENV{'EPOCROOT'}backup\\drive_d");
rmtree("$ENV{'EPOCROOT'}backup\\drive_e");

# create backups of the current drive states
ExecCmd("xcopy /y/e/i/q $ENV{'EPOCROOT'}epoc32\\wins\\c $ENV{'EPOCROOT'}backup\\wins\\c");	
ExecCmd("xcopy /y/e/i/q $ENV{'EPOCROOT'}epoc32\\winscw\\c $ENV{'EPOCROOT'}backup\\winscw\\c");	
ExecCmd("xcopy /y/e/i/q $ENV{'EPOCROOT'}epoc32\\drive_d $ENV{'EPOCROOT'}backup\\drive_d");
ExecCmd("xcopy /y/e/i/q $ENV{'EPOCROOT'}epoc32\\drive_e $ENV{'EPOCROOT'}backup\\drive_e");

# clean the results folder
rmtree("$resultsOutputDir");
ExecCmd("mkdir $resultsOutputDir");

# open test result file
if (!open(TESTRESULTS, ">$testResultsPath"))
{
    die "Can't open output file $testResultsPath";
}

# create HTML header
print TESTRESULTS "<html>\n<head>\n<title>Title of page</title>\n</head>";
print TESTRESULTS "\n<body>\n<h2 align=\"center\">Test Results</h2>\n";
print TESTRESULTS "<p><b>target:</b> $buildTarget<br>\n";
print TESTRESULTS "<b>security sources:</b> $ENV{SECURITYSOURCEDIR}<br>\n";
print TESTRESULTS "<b>test spec:</b> $testSpec<br>\n";
print TESTRESULTS "<b>tested components:</b> @testedComponents</p>\n";
print TESTRESULTS "<table border=\"1\">\n<tr bgcolor=\"gray\"><th>#</th><th>Component</th><th>Release</th><th>Program</th><th>Script</th><th>Results</th><th>Log file</th><th>Epoc window</th></tr>";

# for the first component being tested there is no need to perform a restore on the drives, this variable is used to make this optimisation
my $firstTestedComponent = 1;

my $i = 0;

foreach my $testedRelease (@testedReleases)
{
	$logSuffix = '';
	if ($testedRelease eq 'urel')
	{
		$logSuffix = '1';	# so that the udeb results aren't overwritten by urel ones, matches ONB labelling
	}

	# open test spec file
	if (!open(TESTSPEC, "<$testSpec"))
	{
    	die "Can't open input file $testSpec";
	}

	# set current directory
	chdir("$ENV{'EPOCROOT'}epoc32\\release\\$buildTarget\\$testedRelease");

	# walk through the test spec file line-by-line and execute matching tests
	my $curComponent = '';

	my $testingCurrentComponent = 0;

	while (<TESTSPEC>)
	{
    	# skip comment lines
    	my $line = $_;
    	if ($line =~ /^\s*#/)
    	{  
        	next;
    	}
    
    	# set current component
    	if ($line =~ /^\s*\[\s*(\w+)\s*\]/) #find [component name] line
		{
			$curComponent = $1;
			print "\n[$curComponent]\n";
		
			# determine if we are testing this component.
			if (!grep(/^$curComponent$/i,@testedComponents)) 
			{
				$testingCurrentComponent = 0;
				next;
			}
		
			$testingCurrentComponent = 1;		
			# clean the system and mmc drives if necessary
			if ($firstTestedComponent)
			{
				$firstTestedComponent = 0;
			}
			else
			{
				ExecCmd("$ENV{SECURITYSOURCEDIR}\\testframework\\test\\autotesting\\restoredrivesstates.bat");	# For some reason when these commands are executed directly from Perl the call to xcopy to backup winscw\c causes xcopy to halt after copying one of the kanji SIS files, as a call to a batch file the commands work correctly.
			}
			# setup ethernet support on the emulator
			ExecCmd("$ENV{SECURITYSOURCEDIR}\\testframework\\test\\autotesting\\useautocfg.bat");
			next;
		}
    
    	# execute the test if we are testing this component
    	if ($testingCurrentComponent) 
    	{
    		my $program = '';
        	my $log = '';
        	my $commdb = '';
        	my $script = '';
        	my $timeout = '';
        	my $release = '';
        	my $preCommand = '';
        	my $postCommand = '';
        
                
        	# parse line from the test spec file
        	($program, $log, $commdb, $script, $timeout, $release, $preCommand, $postCommand) = quotewords(',', 1, $line);
        

        
        	$program =~ s/^\s*//;
        	$program =~ s/\s*$//;
        	$script =~ s/^\s*//;
        	$script =~ s/\s*$//;
        	$preCommand =~ s/^\s*//;
        	$preCommand =~ s/\s*$//;
        	$preCommand =~ s/%SECURITYSOURCEDIR%/$ENV{SECURITYSOURCEDIR}/;
        	$postCommand =~ s/^\s*//;
        	$postCommand =~ s/\s*$//;
        	$postCommand =~ s/%SECURITYSOURCEDIR%/$ENV{SECURITYSOURCEDIR}/;
        	$log =~ s/^\s*//;
        	$log =~ s/\s*$//;

        
       		# check release
        	if($release =! /$testedRelease/i)
        	{
            	next;
        	}
        	$i++;
     
        	# execute the pre-test command
        	if($preCommand ne "")
        	{
            	ExecCmd($preCommand);
        	}
        
        	# execute the test command
        	ExecCmd("$ExecTimeOut \"$program $script\" $timeout");
        	print "\n\nExecution of $program $script";

       
        	# store test results and epoc window output files    
        	my $bEpocWnd = 0;
    
    		$trimmedLog = $log;
    		$trimmedLog =~ s/\\.*\\//;	# perl does greedy pattern matching so strips off all directories in the path of the form \....\....\
			$trimmedLog =~ s/.*\\//;	# in case the path is relative strip of the leading directory
			$trimmedLog =~ /(.*)\.([^\.]*)/;	# extract the filename and the extension

      		$epocwindName = $1 . $logSuffix . "_epocwind.txt";
      		$targetLogName = $1 . $logSuffix. "\." . $2;
      	
        	ExecCmd("copy $epocWndPath $resultsOutputDir\\$epocwindName");
        	ExecCmd("del $epocWndPath");
        	$bEpocWnd = 1;
     
        
        	# process log files
        	my $sText = "";
        	my $iPass = 0;
        	my $iFail = 0;
        	my $iPass_case = 0;	# not used
        	my $iFail_case = 0;	# not used
        	my $bLogFile = 0;
        	my $logPath = FullWin32Path($buildTarget, $release, $log);
        	if (-f $logPath)
        	{
            	$bLogFile = 1;
            	ExecCmd("copy $logPath $resultsOutputDir\\$targetLogName");
    			# Test Execute
    			if ($sText eq "")
    			{
    				($sText, $iPass, $iFail, $iPass_case, $iFail_case) = DigestTestExecuteTestLog($logPath);
    			}
    	
    			# Security Test Logs
    			if ($sText eq "" )
    			{
    				# Check to see if this is an Security RTest test log
    				($sText, $iPass, $iFail) = DigestSecurityTestLog($logPath);
    			}
        
            	# get rid of surrounding html tags
            	if ($sText)
            	{
                	if ($sText =~ /<td.*>(.+)<\/td>/)
                	{
                    	$sText = $1;
                	}
            	}
        	}
        
        	# generate HTML results
        	my $htmlEpocWnd = "<a href=\"$resultsOutputDir\\$epocwindName\">here</a>";
        	if(!$bEpocWnd)
        	{
            	$htmlEpocWnd = "not found";
        	}
        	my $htmlColor = "white";
        	my $htmlLog = "<a href=\"$resultsOutputDir\\$targetLogName\">$log</a>";
        
        	if($bLogFile)
        	{
            	if ($sText eq "" )
            	{
                	$htmlColor = 'blue';
                	$sText = 'log file not parsed';
            	}
            	elsif(!$iPass or $iFail)
            	{
                	$htmlColor = "red";
            	}
        	}
        	else
        	{
            	$htmlColor = 'yellow';
            	$sText = 'log file not found';
            	$htmlLog = '';
        	}
           
        	print TESTRESULTS "\n<tr bgcolor=\"$htmlColor\">";
        	print TESTRESULTS "<td>$i</td><td>$curComponent</td><td>$testedRelease</td><td>$program</td><td>$script</td>";
        	print TESTRESULTS "<td>$sText</td><td>$htmlLog</td><td>$htmlEpocWnd</td></tr>";

        	print "\nResults: $sText";
        
        	# execute the post-test command
        	if($postCommand ne "")
        	{
        	    ExecCmd($postCommand);
        	}
    	}
	}
	close TESTSPEC;
}

# restore the drives' states
if (!$skipCleanAfterFinalComponent)
{
	ExecCmd("$ENV{SECURITYSOURCEDIR}\\testframework\\test\\autotesting\\restoredrivesstates.bat");
}	

# final html stuff
print TESTRESULTS "\n</table></body></html>";

close TESTRESULTS;

if (!$suppressBrowserPopup)
{
	# open test results in browser, this will halt script execution until the browser is closed
	`$testResultsPath`;
}	

######################################################################################
#  helper functions
######################################################################################
sub ExecCmd
{
	my $cmd = $_[0];
    print "\n".`$cmd`;
}

sub DisplayHelp {
	print "\n\n\nHELP DIALOG:";
	print "\nThe following arguments are required:\n\n";
	print "-components: A comma separated (no spaces) list of component names.\n\n";
	print "-testreleases: A comma separated (no spaces) list of the releases tested.\n\n";
	print "-exectimeout: The path location of ExecTimeOut.exe relative to this drive.  This is downloaded via Perforce from //PR/share/DABSRelease/buildscripts/ExecTimeOut.exe.\n\n";
	print "-resultsoutput: Directory used to store the test results and epocwind.out files from running the tests.  Current contents will be deleted.  Use a value of \\logs\\winscw if you want it to be compatible with the panicscan test.\n\n";
	print "-skipfinalclean: The script will restore the drives to their original state after each component has been tested.  If you do not want the drives to be restored after the final component has been tested (typically used when debugging just one component's tests) then add this flag and before re-running the tests manually restore the drives from the contents of the \\backup\\ folder.\n\n";
	print "-suppressbrowser: Add this flag to prevent the halting of execution and presentation of the results in a browser window after the tests have been run.";
	print "Example usage:\n";
	print "test_launcher.pl -components caf2,streamingcaf,swi -testreleases udeb,urel -exectimeout \\ExecTimeOut.exe resultsoutput \\logs\\winscw -skipfinalclean -suppressbrowser\n\n";
	die;
}

######################################################################################
# helper functions taken from the TestUtilities.pm perl module
######################################################################################

sub DigestTestExecuteTestLog
# Arg: [0] output log from a TestExecute run
# Returns: small HTML summary string of the run
{
	my $filename = $_[0];
	my $TestConfirmed = $_[1];
	my $sHTML = "";
 
	$filename =~ s/\s+$//;
	return "" unless( -f $filename and open( TEST, $filename )); 

	$/ = "\n";

	my $iFail = 0;
	my $iPass = 0;
	my $iCheck = 0;
	my $bTestExecute = 0;
	my $TEST_STEP_SECTION = 0;
	my $iFail_case = 0;
	my $iPass_case = 0;
	my $iCheck_case = 0;
	my $TEST_CASE_SECTION = 0;
	

	while( <TEST> )
	{
		chomp;
		my $sLine = $_;
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
		}
		if ( $TEST_STEP_SECTION )
		{
			$iPass += $1 if( $sLine =~ /<font.*color=00AF00> PASS = (\d+)<\/font>/i);
			$iPass += $1 if( $sLine =~ /<font.*color=00AF00>PASS = (\d+)<\/font>/i);
		
			$iFail += $1 if( $sLine =~ /<font.*color=FF0000>FAIL = (\d+)<\/font>/i);
			$iFail += $1 if( $sLine =~ /<font.*color=0000FF>ABORT = (\d+)<\/font>/i);
			$iFail += $1 if( $sLine =~ /<font.*color=0000FF>PANIC = (\d+)<\/font>/i);
			$iFail += $1 if( $sLine =~ /<font.*color=0000FF>INCONCLUSIVE = (\d+)<\/font>/i);
			$iFail += $1 if( $sLine =~ /<font.*color=0000FF>UNKNOWN = (\d+)<\/font>/i);
			$iFail += $1 if( $sLine =~ /<font.*color=0000FF>UNEXECUTED = (\d+)<\/font>/i);
		}
		if ( $TEST_CASE_SECTION )
		{

			$iPass_case += $1 if( $sLine =~ /<font.*color=00AF00> PASS = (\d+)<\/font>/i);
			$iPass_case += $1 if( $sLine =~ /<font.*color=00AF00>PASS = (\d+)<\/font>/i);
			$iFail_case += $1 if( $sLine =~ /<font.*color=FF0000>FAIL = (\d+)<\/font>/i);
			$iFail_case += $1 if( $sLine =~ /<font.*color=0000FF>ABORT = (\d+)<\/font>/i);
			$iFail_case += $1 if( $sLine =~ /<font.*color=0000FF>PANIC = (\d+)<\/font>/i);
			$iFail_case += $1 if( $sLine =~ /<font.*color=0000FF>INCONCLUSIVE = (\d+)<\/font>/i);
			$iFail_case += $1 if( $sLine =~ /<font.*color=0000FF>UNKNOWN = (\d+)<\/font>/i);
			$iFail_case += $1 if( $sLine =~ /<font.*color=0000FF>UNEXECUTED = (\d+)<\/font>/i);
			
		}
		$bTestExecute = 1 if( $sLine =~ /\*\*\*\s+TestExecute\s+Started/i);

	}
	close( TEST );
	
	if( $bTestExecute )
	{
		if(( $iPass == 0 and $iFail == 0) and ($TestConfirmed == 1))
		{
			$sHTML = "<td>$iPass passed, $iFail failed</td>";
		}
		elsif ( $iPass == 0 and $iFail == 0)
		{
			$sHTML = "<td $sFailedBGColor>test crashed</td>";
		}
		elsif( $iFail > 0 )
		{
			$sHTML = "<td $sFailedBGColor>$iPass passed, $iFail failed</td>";
		}
		else
		{
			$sHTML = "<td>$iPass passed, $iFail failed</td>";
		}
	}
	elsif( $iPass > 0 or $iFail > 0 )
	{
		#
		# Almost certainly a TEF log, but missing the "TestExecute Started" line.
		# Possibly because of a loss of part of the log from the hardware.
		#
		if( $iFail > 0 )
		{
			$sHTML = "<td $sFailedBGColor>$iPass passed, $iFail failed</td>";
		}
		else
		{
			$sHTML = "<td  $sWarningBCColor>$iPass passed, $iFail failed</td>";
		}
	}
	 	 
	return ($sHTML, $iPass, $iFail, $iPass_case, $iFail_case);
}


sub DigestSecurityTestLog
# Arg: [0] output log from an automated testcase run
# Returns: small HTML summary string of the run indicating test result extracted from log.
{	
	my $filename = $_[0];

	my $sHTML = "";
 
	my $sCmd = "type $filename |";
	$filename =~ s/\s+$//;
	return "" unless( -f $filename and open( TEST, $sCmd )); 

	$/ = "\n";

	# For these tests the result x tests failed out of y is displayed in the 2nd last line of the log file. 
    
	my $iFail = 0;
	my $iPass = 0;
	my $iTotal = 0;

	while( <TEST> )
	{
		chomp;
		my $sLine = $_;
		my $null = chr(0);
		$sLine =~ s/$null|\s//g;
		if( $sLine =~ /(\d+)testsfailedoutof(\d+)/i )
		{
			$iFail += $1;
			$iTotal += $2;
		}
	}
	close( TEST );
	
	if( $iTotal > 0 )
	{
		$sHTML = "<td>";
		$sHTML = "<td $sFailedBGColor>" if( $iFail > 0 );

		$iPass = $iTotal - $iFail;
		$sHTML .= "$iPass passed, $iFail failed</td>";
	}
	 	 
	return ($sHTML, $iPass, $iFail);
}

######################################################################################
# helper functions taken from the BuildSpecifications.pm perl module
######################################################################################

# Read the contents of a file into a string and return it
sub ReadFile ($)
{
	my ($file) = @_;
	open FILE, "<$file" or LogDie("Can't read file: $file", @StdLogs);
	local $/ = undef;
	my $data = <FILE>;
	close FILE;
	return $data;
}


###########################################################################
# Return Path to z Drive
# Arguments: [0] Build Target (e.g. arm4, wins, etc)
#            [1] Build Release (Optional) (e.g. UDEB/UREL)
# Returns Correspoonding Effective Z Drive
sub FullPathToZDrive
{
	my ($buildTarget, $buildRelease) = @_;
	$buildRelease="UDEB" unless( $buildRelease );

	return "$ENV{'EPOCROOT'}epoc32\\release\\$buildTarget\\$buildRelease\\z" if( $buildTarget =~ /wins/i );
	return "$ENV{'EPOCROOT'}epoc32\\data\\z";
}


###########################################################################
# Return Path to C Drive
# Arguments: [0] Build Target (e.g. arm4, wins, etc)
# Returns Correspoonding Effective C Drive
sub FullPathToCDrive
{
	my ($buildTarget) = @_;

	return "$ENV{'EPOCROOT'}epoc32\\$buildTarget\\c" if( $buildTarget =~ /wins/i );
	return "$ENV{'EPOCROOT'}epoc32\\data\\z";
}


###########################################################################
# EPOC paths can be drive-relative ("\scripts\bob") or absolute ("c:\scripts\bob"). 
# Return an appropriate Win32 path, where - an additional complication
# is that the EPOC c: and z: aren't sibling Win32 paths
# Arguments: [0] Build Target (e.g. arm4, wins, etc)
#            [1] Build Release (Optional) (e.g. UDEB/UREL)
#            [2] EPOC path
# Returns: full Win32 path
sub FullWin32Path
{
	my ($buildTarget, $buildRelease, $sPath) = @_;

	$sPath =~ s/^\s+//;
	if($sPath =~ /^(\w):(.*)/)
	{
		my $sDrive = uc($1);
		my $sDir = $2;
		$sDir =~ s/^\\//;
		if($sDrive eq "C")
		{
			return FullPathToCDrive($buildTarget) . "\\$sDir";
		}
		elsif($sDrive eq "Z")
		{
			return FullPathToZDrive($buildTarget,$buildRelease) . "\\$sDir";
		}
		else
		{
			# Maybe an actual pathname
			return $sPath;
		}
	}
	
	# Presumption is that path is relative to EPOC C:\
	$sPath =~ s/^\\//;
	return FullPathToCDrive($buildTarget) . "\\$sPath";
}