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

##################################################################################
# These variables can be customised as required
# define some global variables - customise as required
my $REPORTDIR;
my $MAINLINEZIPDIR;
my $RELSUBSYSROOT;
my $TECHVIEWROOT;

# set variables which can be set from the environment. If no environment
# equivalent is found, it will set to the default
SetVariable(\$REPORTDIR, "REPORTDIR", "L:\\AutoSecTests");
SetVariable(\$MAINLINEZIPDIR, "MAINLINEZIPDIR", "\\\\builds01\\Master");
SetVariable(\$RELSUBSYSROOT, "RELSUBSYSROOT", "security");

# To include Techview in build set environment variable:
# TECHVIEWROOT=Techview
SetVariable(\$TECHVIEWROOT, "TECHVIEWROOT", "");

# host platforms define which platforms are to be built and tests to run. Target
# platforms are those where roms are build
my @HOSTPLATFORMS = ("wins", "winscw");
my @TARGETPLATFORMS = ("arm4", "thumb");
my @ROMPLATFORMS = ("lubbock", "assabet");
my @BUILDTYPES = ("udeb", "urel");
#
# END of customisable variables
####################################################################################

my $P4CLIENTNAME = ConfigureDefaultClient();
my $TESTCONFIGFILE;
my $SUBSYSTEMROOT;
# beech or cedar 
my $BUILDPLATFORM; 

# first get the date
my @DATEARRAY = split(/[ \/]/, `date /t`);

# command line arguments
my %ARGS = ();

# Parse command line parameters
ProcessCommandLine();

# now, DATEARRAY[0] will be day, 1 will be date of month, 2 will be month, 3 is year
my $RESULTDIR = "$REPORTDIR\\$DATEARRAY[3]$DATEARRAY[2]$DATEARRAY[1]";

# define the file where the config.txt file is post-processed
my $POSTPROCESSED = "$RESULTDIR\\postprocessed.txt";

# create a new directory for today
system("mkdir \"$RESULTDIR\"");

# create a logfile and error file
open  (LOGFILE, "> $RESULTDIR\\log.txt") || die "error opening for writing";
open  (ERRFILE, "> $RESULTDIR\\error.txt") || die "error opening for writing";

# Derive rootdrive from perforce client. Set up variables
# which required the root drive to be configured. Note that
# the test config file is only overwritten if not configured by
# the command line. By default it looks for config.txt in the current
# directory
my $ROOTDRIVE = "L:";
GetRootDrive();
if (!$TESTCONFIGFILE)
{
	use Cwd;
	my $dir = cwd();
	$TESTCONFIGFILE = "$dir\\config.txt";
}
if (!$SUBSYSTEMROOT)
{ 
	$SUBSYSTEMROOT = "$ROOTDRIVE\\$RELSUBSYSROOT";
}

if ($TECHVIEWROOT)
{
	$TECHVIEWROOT = "$ROOTDRIVE\\$TECHVIEWROOT";
}

# epoc root directory
my $EPOCROOT = "$ROOTDRIVE\\epoc32";

# add techview to build if requested
my @BUILDDIRS = "";
if ( $TECHVIEWROOT )
{
	@BUILDDIRS = ( $SUBSYSTEMROOT, "$TECHVIEWROOT\\toolkit\\startup\\group", "$TECHVIEWROOT\\apps\\appinstui\\group", "$TECHVIEWROOT\\apps\\secui\\group");
}
else
{
	@BUILDDIRS = ( $SUBSYSTEMROOT );
}

print "BUILDIRS = @BUILDDIRS\n";

# call main
main();

# main function 
sub main()
{
	print "REPORTDIR = $REPORTDIR\n";
	print "MAINLINEZIPDIR = $MAINLINEZIPDIR\n";
	print "SUBSYSTEMROOT = $SUBSYSTEMROOT\n";
	print "TESTCONFIGFILE = $TESTCONFIGFILE\n";
	print "TECHVIEWROOT = $TECHVIEWROOT\n";
	print "BUILDPLATFORM = $BUILDPLATFORM\n";

	# Figure out the second-latest release if none had been specified on the
	# command line - we do not want the latest in case it has not yet been released. 
	my $releasedir = "";
	my $platformsuffix = "";
	if ($BUILDPLATFORM)
	{
		$platformsuffix = "_$BUILDPLATFORM";
	}

	if (!$ARGS{"mainline"})
	{
		my $filelist = `dir $MAINLINEZIPDIR\\0????$platformsuffix /ad /b | sort /r`;
		my @filearray = split "\n", $filelist;
		$releasedir = $filearray[1];
	}
	else
	{
		$releasedir = "$ARGS{\"mainline\"}$platformsuffix";
	}

	# Display what's to be done
	ReportToFiles("------ Script run on $DATEARRAY[1]/$DATEARRAY[2]/$DATEARRAY[3] with the following options\n");
	ReportToFiles("------       Report directory: $REPORTDIR\n");
	ReportToFiles("------       Subsystem root: $SUBSYSTEMROOT\n");
	ReportToFiles("------       Test config file: $TESTCONFIGFILE\n");
	ReportToFiles("------       Built against MCL platform: $BUILDPLATFORM\n");
	if ($ARGS{s}) { ReportToFiles("------      Perforce synchronisation using client spec \"$P4CLIENTNAME\" on drive \"$ROOTDRIVE\"\n"); }
	if ($ARGS{u}) { ReportToFiles("------      Mainline unzip using build $releasedir\n"); }
	if ($ARGS{b}) { ReportToFiles("------      Build of production and test code\n"); }
	if ($ARGS{t}) { ReportToFiles("------      Running of test code on the host using $TESTCONFIGFILE\n"); }
	if ($ARGS{f}) { ReportToFiles("------      Build rom first\n"); }
	if ($ARGS{t}) { ReportToFiles("------      Running of test code on the host using $TESTCONFIGFILE\n"); }

	if ($ARGS{r}) 
	{ 
		ReportToFiles("------      Building of target roms: "); 
		map { ReportToFiles("$_ "); } @ROMPLATFORMS;
		ReportToFiles("\n");
	}
	ReportToFiles("------      Host builds used: ");
	map { ReportToFiles("$_ "); } @HOSTPLATFORMS;
	ReportToFiles("\n------      Target processor builds used: ");
	map { ReportToFiles("$_ "); } @TARGETPLATFORMS;
	ReportToFiles("\n------      Build types used: ");
	map { ReportToFiles("$_ "); } @BUILDTYPES;
	ReportToFiles("\n\n");

	# Do a perforce sync
	if ($ARGS{s})
	{
		DoPerforceSync();
	}

	# Now unzip the mainline
	if ($ARGS{u})
	{
		DoMainlineUnzip($releasedir);
	}

	# Build the post-processed config file if the test flag is on
	if ($ARGS{t})
	{
		ParseTestConfigFile();
	}

    # Build the tools
	if ($ARGS{o})
	{
		foreach my $bt (@BUILDTYPES)
		{
			DoBuildTools($bt);
		}
	}

	# Build rom first?
	if ( $ARGS{f} ) 
	{
		# Now build the target platforms - no testing required
		if ($ARGS{b})
		{
			foreach my $platform (@TARGETPLATFORMS)
			{
				foreach my $bt (@BUILDTYPES)
				{
					# Must build dummy midp2installerclient for cedar
					if ( $BUILDPLATFORM eq "cedar" )
					{
						my @tempbuilddirs = @BUILDDIRS;
						@BUILDDIRS = ("$SUBSYSTEMROOT\\appinst\\tsrc");
						print "Building dummy midp2installerclient....on $platform/$bt\n";
						DoBuild($platform, $bt);
						# now build the rest
						@BUILDDIRS = @tempbuilddirs;
					}
					DoBuild($platform, $bt);
				}
			}	
		}
		# Now build the roms if required
		if ($ARGS{r})
		{
			DoBuildRoms();
		}
	}

	# Build and test host platforms. They get built and then tested in order.
	# This is required as the EPOC C drive is shared between udeb and urel.
	# If the build/test is run together as a set, then the state of the C
	# drive will be restored
	foreach my $platform (@HOSTPLATFORMS)
	{
		foreach my $bt (@BUILDTYPES)
		{
			if ($ARGS{b})
			{
				DoBuild($platform, $bt);
			}
			if ($ARGS{t})
			{
				DoRunTests($platform, $bt);
			}
		}
	}

	# Now build the target platforms (unless this has been done in advance) - no testing required
	if ( !$ARGS{f} ) 
	{
		if ($ARGS{b})
		{
			foreach my $platform (@TARGETPLATFORMS)
			{
				foreach my $bt (@BUILDTYPES)
				{
					# Must build dummy midp2installerclient for cedar
					if ( $BUILDPLATFORM eq "cedar" )
					{
						my @tempbuilddirs = @BUILDDIRS;
						@BUILDDIRS = ("$SUBSYSTEMROOT\\appinst\\tsrc");
						print "Building dummy midp2installerclient....on $platform/$bt\n";
						DoBuild($platform, $bt);
						# now build the rest
						@BUILDDIRS = @tempbuilddirs;
					}
					DoBuild($platform, $bt);
				}
			}	
		}
		# Now build the roms if required
		if ($ARGS{r})
		{
			DoBuildRoms();
		}
	}

	my $timeoutput = `time /t`;
	ReportToFiles("\n------------------- Job finished at $timeoutput\n\n");

}
sub ConfigureDefaultClient
{
	# this subroutine figures out what default perforce client to use
	my $clientoutput = `p4 client -o`;

	if ($clientoutput =~ /\nClient:\s*([^\s]*)/ )
	{
		return $1;
	}
}

sub GetRootDrive
{
	# this subroutine derives the rootdrive from the perforce client
	my $clientoutput = `p4 client -o`;

	if ($clientoutput =~ /\nRoot:\s*(.):/ )
	{
		$ROOTDRIVE = "$1:"
	}
}

sub ProcessCommandLine
{
	use Getopt::Long;
	&GetOptions (\%ARGS, "a", "h", "s", "u", "b", "t", "r", "o", "f", "client=s", "configfile=s", "workdir=s", "mainline=s", "subsystemroot=s", "nohost", "notgt", "platform=s", @HOSTPLATFORMS, @TARGETPLATFORMS, @ROMPLATFORMS, @BUILDTYPES);

	# if the -a option is specified, this means that sync, unzip, build, run
	# tests, and rom options should be set
	if ($ARGS{a})
	{
		$ARGS{o} = $ARGS{s} = $ARGS{u} = $ARGS{b} = $ARGS{t} = $ARGS{r} = $ARGS{a};
	}

	# if the help option is specified, or none of the other arguments are specified,
	# or both beech and cedar requested, then display the help
	if ($ARGS{h} || (!$ARGS{s} && !$ARGS{u} && !$ARGS{b} && !$ARGS{t} && !$ARGS{r} && !$ARGS{o}))
	{
		print "Command line switches: \n";
		print "    -s        Do perforce synchronisation\n";
		print "    -u        Do unzipping from the mainline\n";
		print "    -b        Build the appropriate targets\n";
		print "    -t        Run the test suite\n";
		print "    -r        Build the roms\n";
        print "    -o        Build the tools\n";
		print "    -a        Do all of the above\n";
		print "    -f        Build rom First (before wins winscw)\n";
		print "    -h        Display this screen\n";
		print "\nOther options:\n";
		print "    --wins    Use the wins platform for building and running of host tests\n";
		print "    --winscw  Use the winscw platform for building and running of host tests\n";
		print "    --nohost  Do not use any host platform\n";
		print "    --arm4    Use the arm4 processor for rom building\n";
		print "    --thumb   Use the thumb processor for rom building\n";
		print "    --notgt   Do not use any target platform\n";
		print "    --assabet Build roms for assabet platform\n";
		print "    --lubbock Build roms for lubbock platform\n";
		print "    --udeb    Build udeb version only\n";
		print "    --urel    Build urel version only\n";
		print "\nParameters:\n";
		print "    --client=<client>   Specifies the perforce client to use (default: $P4CLIENTNAME)\n";
		print "    --workdir=<dir>     Specifies the working directory (default: $REPORTDIR)\n";
		print "    --subsystemroot=<dir> Specifies the root directory to build\n";
		print "    --configfile=<file> Specifies the test config file\n";
		print "    --mainline=<release number> Specifies the release number to use\n";
        print "    --platform=<platform name>  Specifies MCL platform. Default is beech. Set to none for pre MCL builds\n";
		print "\nExamples:\n";
		print "    perl -w RunTests.pl -b -t -r --wins --lubbock --thumb --platform=cedar --client=lon-julianl\n\n";
		print "    Uses drive m, client lon-julianl, build against cedar and builds and runs tests on the wins\n";
		print "    platform and builds roms for lubbock\n\n";
		print "    perl -w RunTests.pl -u -b --winscw --urel --notgt --mainline=01038\n\n";
		print "    Unzips mainline 01038_beech and builds only winscw/urel. Uses default client\n";
		print "    perl -w RunTests.pl -u -b --winscw --urel --notgt --platform=none --mainline=01038\n\n";
		print "    Unzips mainline 01038 and builds only winscw/urel. Uses default client\n";
		exit(0);
	}

	# Now parse the client, drive, and working directory parameters
	if ($ARGS{"client"})
	{
		$P4CLIENTNAME = $ARGS{"client"};
		$ENV{"P4CLIENT"} = $P4CLIENTNAME;
	}
	if ($ARGS{"configfile"})
	{
		$TESTCONFIGFILE = $ARGS{"configfile"};
	}
	if ($ARGS{"workdir"})
	{
		$REPORTDIR = $ARGS{"workdir"};
	}
	if ($ARGS{"subsystemroot"})
	{
		$SUBSYSTEMROOT = $ARGS{"subsystemroot"};
	}

	use Getopt::Long;

	# now parse the host options
	if ($ARGS{"nohost"})
	{
		@HOSTPLATFORMS = ();
	}
	else
	{
		ParseArguments(\@HOSTPLATFORMS);
	}

	# now parse the target platforms
	if ($ARGS{"notgt"})
	{
		@TARGETPLATFORMS = ();
	}
	else
	{
		ParseArguments(\@TARGETPLATFORMS);
	}

	# now parse the rom platforms and the build types
	ParseArguments(\@ROMPLATFORMS);
	ParseArguments(\@BUILDTYPES);

	if ($ARGS{"platform"})
	{
		$BUILDPLATFORM=$ARGS{"platform"};
		if ($BUILDPLATFORM eq "none")
		{
			# set compatibity for pre MCL builds
			$BUILDPLATFORM="";
		}
	}
	else
	{
		# default
		$BUILDPLATFORM="beech"; 
	}
}

sub ParseArguments
{
	# common parsing of arguments in an array - returns another array
	# which have the options specified.
	# If none specified, it returns ALL the options.
	#
	# eg. If -wins specified, and options = ("wins", "winscw"), then
	# return array is just ("wins"). If neither wins or winscw specified,
	# then returns both
	(my $options) = @_;
	my @retoptions = ();

	foreach my $opt (@$options)
	{
		if ($ARGS{$opt})
		{
			push(@retoptions, $opt);
		}
	}

	# change if we have at least one option specified
	if (scalar(@retoptions) > 0)
	{
		@$options = @retoptions;
	}
}

# subroutine to do a perforce sync 
sub DoPerforceSync
{
	print "Doing a Perforce synchronisation....\n";
	ExecAndOutputToLog("p4 sync -f $SUBSYSTEMROOT\\...");
    if ( $TECHVIEWROOT )
    {
        ExecAndOutputToLog("p4 sync -f $TECHVIEWROOT\\...");
    }
}

# subroutine to do the unzipping
sub DoMainlineUnzip
{
	my ($dir) = @_;
	print "Doing an unzip of mainline $MAINLINEZIPDIR\\$dir...\n";

	my @zipfiles = ("techview");

	# firstly, delete anything already there
	ExecAndOutputToLog("del \/F \/S \/Q $EPOCROOT");

	# unzip all the zipfiles
	map { ExecAndOutputToLog("t:\\tools\\unzip -o $MAINLINEZIPDIR\\$dir\\zips\\$_.zip -d $ROOTDRIVE\\"); } @zipfiles;
}

# subrountine to build or clean release or test build
# call RunAbld("<Directory>", "<Command>", "<Target>", "test" or "")
sub RunAbld
{
	my ($dir, $cmd, $target, $test, $bldtype) = @_;
	chdir ($dir);

	my $bldcommand = "abld $test $cmd -k $target $bldtype";
	my $bldoutput = ExecAndOutputToLog($bldcommand);	
	
	if ($cmd eq "build")
	{
		# match "error:" or "error(s)" or fatal error
		my @errout = grep(/(error(\(s\)|\:))|fatal error/i,  split("\n", $bldoutput));

		if (scalar(@errout) > 0)
		{
			print ERRFILE "-------- Errors found when running $bldcommand\n";
			map { print ERRFILE "$_\n"; } @errout;
			print ERRFILE "-------- End of errors for $bldcommand\n\n";
		}
	}
}

# builds the release and test code for a specified platform and mode
# eg WINS/UDEB
sub DoBuild
{
	my ($platform, $bt) = @_;

	RemoveWatchers($platform, $bt);

	foreach my $bd (@BUILDDIRS)
	{

		chdir ($bd);
		system("bldmake bldfiles");

		print "Building build targets.... $bd on $platform/$bt\n";
		ReportToFiles("\n------------------- Now building $bd on $platform/$bt\n\n");

		RunAbld($bd, "clean", $platform, "", $bt);
		RunAbld($bd, "clean", $platform, "test", $bt);
		RunAbld($bd, "build", $platform, "", $bt);
		RunAbld($bd, "build", $platform, "test", $bt);
	}
}

# builds the tools for deb or rel
sub DoBuildTools
{
	my ($bt) = @_;

    #adjust for unicode
    if ($bt eq "udeb")
    {
        $bt = "deb";
    }
    if ($bt eq "urel")
    {
        $bt = "rel";
    }

	foreach my $bd (@BUILDDIRS)
	{
		chdir ($bd);
		system("bldmake bldfiles");

		print "Building tools.... $bd on $bt\n";
		ReportToFiles("\n------------------- Now building $bd for tools on $bt\n\n");

		RunAbld($bd, "clean", "tools", "", $bt);
		RunAbld($bd, "build", "tools", "", $bt);
	}
}

# this subroutine executes the given command and outputs to the log file
sub ExecAndOutputToLog
{
	my ($cmd) = @_;
	my $timeoutput = `time /t`;
	my $cmdoutput = `$cmd 2>&1`;

	# capture output to log file
	print LOGFILE "-------- Output of $cmd run at $timeoutput";
	if ($cmdoutput) 
	{
		print LOGFILE "$cmdoutput\n";
	}
	else
	{
		print LOGFILE "(NO OUTPUT)\n";
	}

	if ($? != 0)
	{
		print ERRFILE "\n-------- Error code $? returned when executing:\n";
		print ERRFILE "-------- \"$cmd\".\n";

		if ($cmdoutput)
		{
			print ERRFILE "-------- Output:\n";
			print ERRFILE "$cmdoutput";
		}
	}
	return $cmdoutput;
}

# this subroutine executes all the tests as specified in the configuration file.
# It will run the tests for the specified platform/built type
#
# The configuration file has the following valid commands:
# 
# #include <filename> - includes another config file
# SYSTEMCMD <command and parameters> - executes a DOS command eg SYSTEMCMD copy a.txt b.txt
# DOTEST <testname> <scriptname> <logname> ... - executes a test
# COMP <filea> <fileb> - compares two files and logs result
#
# For SYSTEMCMD and COMP, the following variables get substituted
# - $C - the current EPOC C drive (eg o:\epoc32\wins\c)
# - $Z - the current EPOC Z drive (eg o:\epoc32\release\wins\udeb\z)
# - $RELEASE - the epoc release directory for this build (eg o:\epoc32\release\wins\udeb)
# - $EPOC - the epoc root (eg o:\epoc32)
# - $ROOT - the root drive (eg o:)
sub DoRunTests
{
	# platform and build type
	my ($p, $bt) = @_; 
	print "Running tests on platform $p/$bt\n";
	ReportToFiles("\n------------------- Now running tests on platform $p/$bt\n\n");

	# set the default directory to be the subsystem root
	chdir($SUBSYSTEMROOT);

	# Make the results directory
	my $testlogs = "$RESULTDIR\\testlogs";
	system("mkdir $testlogs");
	system("mkdir $testlogs\\$p");
	system("mkdir $testlogs\\$p\\$bt");

	# open the post processed file
	open (POSTFILE, "< $POSTPROCESSED") || die "error opening $POSTPROCESSED for reading";
	# open a separate report file for test results
	open (TESTREPFILE, "> $RESULTDIR\\testresults.$p.$bt.txt") || die "error opening for writing";

	my $timeoutput = `time /t`;
	print TESTREPFILE "-------- Test log file when running on platform $p/$bt at $timeoutput";

	my $cdrive = "$EPOCROOT\\$p\\c";
	while (<POSTFILE>)
	{
		my $line = $_;

		# check for a line with SYSTEMCMD <command>
		if ( $line =~ /^SYSTEMCMD[ ]+(.*)/ )
		{
			# Substitute $C for the cdrive, $Z for the zdrive, and $RELEASE
			# for the release path, etc
			my $cmd = $1;
			SubstitutePaths($p, $bt, \$cmd);

			print "  - executing system command: $cmd\n";
			ExecAndOutputToLog($cmd);
		}
		elsif ( $line =~ /^\s*$/ )
		{
			# if we have an empty line then do nothing
		}
		elsif ( $line =~ /^DOTEST\s+([^\s]+)[\s]+([^\s]+)[\s]+([^\s]+)/ )
		{
			# if this pattern matches DOTEST <cmd> <scriptfile> <logfile>,
			# then we execute this test - extra parameters can be supplied after the logfile
			print "  - running test: $1 $2 $3\n";
			ExecAndOutputToLog("$EPOCROOT\\release\\$p\\$bt\\$1 $2 $3 $'");

			# log files are assumed to be stored in \epoc32\wins\c
			my $outfile = "$cdrive\\$3";

			print TESTREPFILE "\n---- Result of test \"$1 $2 $3\"\n";
			if (-e $outfile)
			{
				# also, copy the log file into the results dir
				copy ($outfile, "$testlogs\\$p\\$bt\\$3");
				
				# check to see if there is a failure
				my $output = `type $outfile`;
				if ($output =~ /(\d+) tests failed out of (\d+)/)
				{
					print TESTREPFILE "     $1 tests failed out of $2\n";
					if ($1 ne "0") 
					{
						# if there is a fail, then print it in the test log
						print ERRFILE "-------- Logfile $outfile produced a failure result!\n";
						print ERRFILE "$output\n";
						print TESTREPFILE "     There was a test FAILURE\n";
					}
					else
					{
						print TESTREPFILE "     All tests SUCCEEDED\n";
					}
				}
				else
				{
					# if it doesn't conform to standard format, then need to notify
					print TESTREPFILE "     WARNING: non-standard log file format - check log\n";
				}
			}
			else
			{
				# if we get here, the output file didn't exist
				print ERRFILE "-------- Logfile $outfile does not exist - did $1 fail to build?\n";
				print TESTREPFILE "     ERROR! Log file $outfile does not exist\n";
			}

		}
		elsif ( $line =~ /^COMP\s+([^\s]+)[\s]+([^\s]+)/ )
		{
			# Parse our paths
			my $file1 = $1;
			my $file2 = $2;

			# substitute variables
			SubstitutePaths($p, $bt, \$file1);
			SubstitutePaths($p, $bt, \$file2);

			print "  - doing comparison on $file1 and $file2\n";
			print TESTREPFILE "\n---- Comparing $file1 and $file2\n";
			my $output = ExecAndOutputToLog("diff -q $file1 $file2");

			if ( $output =~ /differ/ )
			{
				print TESTREPFILE "     Comparison FAILED\n";
			}
			else
			{
				print TESTREPFILE "     Comparison SUCCESSFUL\n";
			}
		}

		else
		{
			ReportToFiles("**** Error test config line: $line");
		}
	}
	close (POSTFILE);
	close (TESTREPFILE);
}

# - $C - the current EPOC C drive (eg o:\epoc32\wins\c)
# - $Z - the current EPOC Z drive (eg o:\epoc32\release\wins\udeb\z)
# - $RELEASE - the epoc release directory for this build (eg o:\epoc32\release\wins\udeb)
# - $EPOC - the epoc root (eg o:\epoc32)
# - $ROOT - the root drive (eg o:)
# - $SUBSYSROOT - subsystem root e.g. security
sub SubstitutePaths
{
	my ($platform, $buildtype, $cmdref) = @_;

	my $cdrive = "$EPOCROOT\\$platform\\c";
	my $releasedir = "$EPOCROOT\\release\\$platform\\$buildtype";
	my $zdrive = "$releasedir\\z";

	${$cmdref} =~ s/\$C/$cdrive/g;
	${$cmdref} =~ s/\$Z/$zdrive/g;
	${$cmdref} =~ s/\$RELEASE/$releasedir/g;
	${$cmdref} =~ s/\$EPOC/$EPOCROOT/g;
	${$cmdref} =~ s/\$ROOT/$ROOTDRIVE/g;
	${$cmdref} =~ s/\$SUBSYSROOT/$RELSUBSYSROOT/g;
}


# This subroutine acts as a preprocessor for a config file. It
# expands the test config file and produced a file in POSTPROCESSED
sub ParseTestConfigFile
{
	use File::Copy;

	# Firstly, copy the configuration file into the postprocessed file
	copy($TESTCONFIGFILE, $POSTPROCESSED);

	# set up boolean to indicate if preprocessing of #include is done
	my $keepgoing = "1";

	while ($keepgoing)
	{
		# reset the keepgoing flag
		$keepgoing = "";

		# if we have to keep going, copy the postprocessed file into a temp
		my $tempfile = "$RESULTDIR\\temp.txt";
		move ($POSTPROCESSED, $tempfile);

		open  (TEMPFILE, "< $tempfile") || die "error opening $tempfile for reading";
		open  (POSTFILE, "> $POSTPROCESSED") || die "error opening $POSTPROCESSED for writing";

		# Now search the temp file for instances of #include
		while (<TEMPFILE>)
		{
			my $line = $_;
			# check for comments - use C++ style //
			if ( $line =~ /(.*)\/\/.*/ )
			{
				# set the keepgoing flag
				$keepgoing = "1";
				print POSTFILE "$1\n";
			}
			elsif ( $line =~ /\#include[ ]+\"([^\"]*)\"/ )
			{
				# set the keepgoing flag because a #include has been found
				$keepgoing = "1";

				# now write the entire contents of this file into the
				# the postprocessed file
				my $contents = `type $SUBSYSTEMROOT\\$1`;
				print POSTFILE "$contents\n";
			}
			elsif ( $line =~ /^\s*$/ )
			{
				# remove if we have only whitespaces
			}
			else 
			{
				print POSTFILE $line;
			}
		}
		close (TEMPFILE);
		close (POSTFILE);

		# delete the temp file
		unlink($tempfile);
	}
}

sub ReportToFiles
{
	# this function just prints a string to both the log file and the error file
	my ($str) = @_;

	print LOGFILE $str;
	print ERRFILE $str;
}

sub DoBuildRoms
{
	ReportToFiles("\n------------------- Now building roms\n\n");

	chdir ("$EPOCROOT\\rom");

	# create a new directory for the roms
	my $romdir = "$RESULTDIR\\roms";
	system("mkdir \"$romdir\"");

	# define the oby file to use
	my $oby_file = "techview.oby";

	# firstly, make sure the SecurityTests.iby file has been included from the techview.oby
	if (open(OBY, ">> include\\$oby_file"))
	{
		print OBY "\n#include <SecurityTests.iby>\n";
		close(OBY);
	}

	# Now go through the list of rom targets and build
	foreach my $rom (@ROMPLATFORMS)
	{
		ExecAndOutputToLog("tools\\buildrom -D__NOWATCHER__ -o$romdir\\$rom.img $rom $oby_file");
	}
}

sub SetVariable
{
	# Sets a variable from an environment variable or a default value if the
	# environment variable has not been set
	my ($variable, $env_name, $default) = @_;

	${$variable} = $ENV{$env_name};
	if (!${$variable})
	{
		${$variable} = $default;
	}
}

# remove the watchers (prevents connection intermittently to NTRas)
sub RemoveWatchers
{
	my ($platform, $bt) = @_;

	ReportToFiles("------------------- Removing watchers\n");

	if ( ($platform eq "wins") or ($platform eq "winscw") )
	{
		# emulator build, delete the dlls for beech and exes for cedar
		system("del $EPOCROOT\\release\\$platform\\$bt\\watcher.dll");
		system("del $EPOCROOT\\release\\$platform\\$bt\\z\\system\\libs\\watcher.dll");
		system("del $EPOCROOT\\release\\$platform\\$bt\\watcher.exe");
		system("del $EPOCROOT\\release\\$platform\\$bt\\z\\system\\libs\\watcher.exe");
	}
	else
	{
		# hardware. remove the watchers from startup script
		if ( $TECHVIEWROOT )
		{
			system("attrib -r $TECHVIEWROOT\\toolkit\\startup\\group\\start.rss");
			system("copy $SUBSYSTEMROOT\\testframework\\test\\autotesting\\startnowatchers.rss $TECHVIEWROOT\\toolkit\\startup\\group\\start.rss");
		}
	}
}
