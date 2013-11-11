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

require Algorithm::Diff;
#ppm install Algorithm-Diff

# Globle variables
#	compare result counter
	{
		$PASSCounter	 = 0;
		$DIFFCounter	 = 0;
		$MISSCounter	 = 0; #only existed in BaseLine directory
		$NEWADDCounter	 = 0; #only existed in DABS directory
		$IGNCounter		 = 0; # this is some case cann't using this tool to comapre
	}
	
	$GlobleisLowCases = 0;
	$CmpDetail 		= 0;    			# this is detailed compare information switch
	$ShowPassCmp 	= 0;				# only output negtive compare result.
	$cmpResultF 	= "dailyregresultfile.txt";
	my $CurrentDir = ".";
  
sub compareBase2DABS_ReturnEmailBody
{
	my ( $baseLineDir, $DABSResultDir,$currentpath) = @_;
	$CurrentDir  = $currentpath; #save to inner pm current variable 
	compareBase2DABS( $baseLineDir, $DABSResultDir);
	my $mydir = "$currentpath\\$cmpResultF";
	print $mydir;
	my @fileArr = read_file($mydir);
	my $buf;
	foreach $item (@fileArr){
		$buf = $buf . $item . "<br>\n";# for lotus not mail.
	}	
	return $buf;
}
#main
sub compareBase2DABS
{
	my ( $baseLineDir, $DABSResultDir, $argv3, $argv4 ) = @_;
	$CurrentDir  = getcwd;
	#	print $argv1." VS ".$argv2."\n";
	#	my($GlobleisLowCases);


	
	if (!open( DailyRegResultOutPut, ">$cmpResultF" ))
	{
		print "open $DailyRegResultOutPut failed.";
	}
	print DailyRegResultOutPut "Start of Result compare\n";


	compare_BaseLineFiles_with_DABSResult( $baseLineDir, $DABSResultDir );
	checking_TestCase_no_BaseLine( $baseLineDir, $DABSResultDir );

	print DailyRegResultOutPut "End of Result compare\n";
	SummaryCounter2File();
	close(DailyRegResultOutPut);
}
#main
#	gerarate the baseline dir of tef code
#	this is utility function for DABS
sub makeBaseLineDir 
{
	my ( $device, $build ) = @_;
	my ( $device1, $build1 );
	my ($dirResult);
	$dirResult = "\\testexecute\\Test\\baseline";
	if ( $device =~ /arm/i ) {
		$device1 = "\\h4";
	}
	elsif ( $device =~ /winscw/i ) {
		$device1 = "\\emulator";
	}
	else {
		return "error_path";
	}

	if ( $build =~ /urel/i ) {
		$build1 = "\\urel";
	}
	elsif ( $build =~ /udeb/i ) {
		$build1 = "\\udeb";
	}
	else {
		return "error_path";
	}
	return $dirResult =
	  $dirResult . $device1 . $build1;    #\testexecute\Test\baseline\h4\udeb
}

sub SummaryCounter2File
{
	my($seperator,$eol);	
	$seperator =  " = ";
	$eol = "\n";
	
	print DailyRegResultOutPut $eol,"*******************" ,$eol,"Compare Summary: ",$eol;
		
			
	print DailyRegResultOutPut "Pass", $seperator, $PASSCounter,$eol;
	print DailyRegResultOutPut "Diff", $seperator, 	$DIFFCounter,$eol;
	print DailyRegResultOutPut "Existing only on Base Line"	, $seperator, $MISSCounter,$eol; 
	print DailyRegResultOutPut "Existing only on DABS", $seperator, $NEWADDCounter,$eol;
	print DailyRegResultOutPut "Needs to manual check", $seperator, $IGNCounter,$eol;
}

sub checking_TestCase_no_BaseLine {
	my ( $baseLineDir, $DABSResultDir ) = @_;

	#	read DABS file list
	my $thisdir;
	opendir( $thisdir, $DABSResultDir );
	my @DABSFiles = readdir($thisdir);
	closedir($thisdir);

	#	read base file list
	opendir( $thisdir, $baseLineDir );
	my @baseFiles = readdir($thisdir);
	closedir($thisdir);

	#	New test case without base line
	my ( $DABSfile, $basefile );

	#	compare all the file name
	foreach $DABSfile (@DABSFiles) {
		if ( $DABSfile =~ /\.htm/ ) {
			my ( $i, $match );
			for ( $i = 0, $match = 0 ; $i < @baseFiles ; $i++ ) {
				$basefile = @baseFiles[$i];
				if ( $DABSfile eq $basefile ) {
					$match = 1;
					last;
				}
			}

			#				the new test case file name found!
			if ( $match != 1 ) {
				print DailyRegResultOutPut
				 "$DABSfile is a new test case without baseline.\n";
				$NEWADDCounter++;
			}
		}

	}
	return;
}

sub compare_BaseLineFiles_with_DABSResult {
	my ( $baseLineDir, $DABSResultDir ) = @_;
	#chdir($baseLineDir) ;
	if ( !-e $baseLineDir)
	{
		print DailyRegResultOutPut
		  "[Fatal Error]: Can not open $baseLineDir.\n";
		return;
	}
	my $thisdir;
	opendir( $thisdir, $baseLineDir );
	my @baseLineFiles = readdir($thisdir);
	closedir($thisdir);
	my $i = 0;
	my @BaseFileSet;
	my @DABSFileSet;

#	generate the file path
	my $count2 = 0;	
	while ( $i < @baseLineFiles ) {
		if (   ( !-d @baseLineFiles[$i] )
			&& ( @baseLineFiles[$i] =~ /\.htm/ )
		  )    # (@files[$count] =~/\.htm/) to select all *.htm files
		{
			@BaseFileSet[$count2] = $baseLineDir . "\\" . "@baseLineFiles[$i]";

			#	print "@srcfiles[$i] \n";
			@DABSFileSet[$count2] =
			  $DABSResultDir . "\\" . "@baseLineFiles[$i]";

			#	print "@dstfiles[$i] \n";
			$count2++;
		}
		$i++;
	}

	#	file context compare
	$i = 2; #ignore .. and . files
	while ( $i < @BaseFileSet ) {
		if ( @DABSFileSet[$i] =~ /dt-tef-concurrent/
			||  @DABSFileSet[$i] =~ /dt-tef-performance-0001/
			||  @DABSFileSet[$i] =~ /dt-tef-command-line/
			) {
			#skip concurrent compare
			print DailyRegResultOutPut @DABSFileSet[$i],
			  " compare has being ignored. Plese checke your self.\n";
			$IGNCounter++;
		}
		elsif ( !-e @BaseFileSet[$i] ) {
#			this case will not TRUE because only baseline's file name will be compared. 
#			see the upper while {....}
			print DailyRegResultOutPut
			  "@BaseFileSet[$i] does not exist in baseline.\n";# is a new test case without baseline
			  #baseLineFiles[$i] only contain file name.
			$NEWADDCounter++;  
		}
		elsif ( !-e @DABSFileSet[$i] ) {
			print DailyRegResultOutPut
			  "$DABSFileSet[$i] does not exist in DABS.\n";
			  #baseLineFiles[$i] only contain file name.
			  $MISSCounter++;
		}
		else {
			FileCompare( @BaseFileSet[$i], @DABSFileSet[$i] );
		}
		$i++;
	}
	#chang back to the orginal dir.
	chdir($CurrentDir); # don't die this fail not change
}

sub FileCompare {
	my ( $argv1, $argv2 ) = @_;
	@array1 = file_array( $argv1, $GlobleisLowCases );
	@array2 = file_array( $argv2, $GlobleisLowCases );

	# This example produces traditional 'diff' output:
	my $diff =
	  Algorithm::Diff->new( \@array1, \@array2 );    # this must done in main
	my ($result);
	$result = TEF_Compare_Diff($diff);
	if ( $result == 0 )                              #file diff
	{
		print DailyRegResultOutPut $argv2, " different with baseline.\n";
		$DIFFCounter++;
	}
	else {
		if ( $ShowPassCmp == 1 )
		{
			print DailyRegResultOutPut $argv2, " PASS\n";
		}
		$PASSCounter++;
	}

	#	if ( $argv4 == "-D" ) {
	#		Save_Tailored_to_file( "PR1", @array1 );
	#		Save_Tailored_to_file( "PR2", @array2 );
	#	}

	#	print @file1[0].@file2[0];
	return;
}

sub file_array  {
	my ( $filename, $isLowCase ) = @_;
	#	if ($isLowCase  == 1)
	#	{
	#		print "file_arrayisLowCases = 1";
	#		print "\n";
	#	}
	#	else
	#	{
	#		print "file_arrayisLowCases = 0";
	#		print "\n";
	#	}
	my @readarray;
	@readarray = read_file($filename);
	return @readarray = remove_prefix_from_arrary( @readarray, $isLowCase );
}

sub read_file {
	my ($read_file1) = @_;
	if(!open( IN1, $read_file1 ) )
	{
	  print DailyRegResultOutPut "[Fatal Error]: Can not open the $read_file1 file !\n" ;
	}
	my @content = <IN1>;
	close(IN1);
	return @content;
}
sub TEF_Compare_Diff {

	# This example produces traditional 'diff' output:
	#	my $diff = Algorithm::Diff->new( \@arr1, \@arr2 );

	my ($diff) = @_;
	my ($BoolSame);
	$BoolSame = 1;     #file same
	$diff->Base(1);    # Return line numbers, not indices
	while ( $diff->Next() ) {
		next if $diff->Same();
		if ( $CmpDetail == 1 ) {
			my $sep = '';
			if ( !$diff->Items(2) ) {
				printf DailyRegResultOutPut "%d,%dd%d\n",
				  $diff->Get(qw( Min1 Max1 Max2 ));
			}
			elsif ( !$diff->Items(1) ) {
				printf DailyRegResultOutPut "%da%d,%d\n",
				  $diff->Get(qw( Max1 Min2 Max2 ));
			}
			else {
				$sep = "---\n";
				printf DailyRegResultOutPut "%d,%dc%d,%d\n",
				  $diff->Get(qw( Min1 Max1 Min2 Max2 ));
			}
			print DailyRegResultOutPut "< $_" for $diff->Items(1);
			print DailyRegResultOutPut $sep;
			print DailyRegResultOutPut "> $_" for $diff->Items(2);
		}
		$BoolSame = 0;    #file diff
	}
	return $BoolSame;
}

sub Save_Tailored_to_file {
	my ( $write_file1, @array ) = @_;

 	if (!open( OUT, "| cat >$write_file1" ) )
	{
	  print DailyRegResultOutPut "[Fatal Error]: Can NOT open the $write_file1 file !\n" ;
	  return ;
	}	
	my $i = 0;
	my $item;
	while ( $i < @array ) {
		$item = @array[ $i++ ];
		print OUT $item;
	}
	close(OUT);
	return;
}

sub output_arrary {
	my (@array) = @_;
	my $i = 0;
	while ( $i < @array ) {
		$item = @array[ $i++ ];
		print $item ;
	}
}

sub remove_prefix_from_arrary {
	my ( @_array, $LowCase ) = @_;
	my $i = 0;

	my $item;
	while ( $i < @_array ) {
		$item       = @_array[$i];
		$item       = remove_time_stamp($item);
		$item       = remove_INFOPRINT($item);
		$item       = remove_remote_panic($item);
		$item       = remove_time_infomation($item);
		@_array[$i] = remove_script_linenum($item);

		if ( $GlobleisLowCases == 1 ) {
			@_array[$i] = lc( @_array[$i] );
		}
		$i++;
	}
	return @_array;
}

sub remove_script_linenum {
	my ($srcstring) = @_;

	#	search for
	# ....\dt-tef-testclient-0009.script Line = 22 Command = END_TESTCASE...
	$_ = $srcstring;
	if (/[.]*.script [Ll]ine = [\d]+ [Cc]ommand =[.]*/) {

		#		print ":)\n";
		$srcstring =~
s/[.]*.script [Ll]ine = [\d]+ [Cc]ommand =[.]*/\.script line = command =/;
	}
	return $srcstring;
}

sub remove_remote_panic {
	my ($srcstring) = @_;
	$_ = $srcstring;
	if (/[Rr]emote [Pp]anic \d/) {

		#		print ":)\n";
		return $srcstring = "";    # remove panic string
	}
	else {

		#		print ":(\n";
		return $srcstring;
	}
}

sub remove_INFOPRINT {
	my ($srcstring) = @_;
	$_ = $srcstring;

#	if (/(INFO|ERROR|WARN|HIGH|MEDIUM|LOW)[ ]-  [0-9]+[ ][a-zA-Z0-9_-]+[.][a-zA-Z]* [0-9]+/) {
#	the file name to strict for "03:39:57:980 INFO -  2607 DIR\DIR\ 221 GetCPPModuleName"
#	"DIR\DIR\" not a legeal file name,
#	so I change the regx
	if (
/(INFO|ERROR|WARN|HIGH|MEDIUM|LOW)[ ]-  [0-9]+[ ][a-zA-Z0-9_.-\\]* [0-9]+/
	  )
	{

		#		print ":)\n";
		$srcstring =~ s/[ ]-  [0-9]+[ ][a-zA-Z0-9_.-\\]* [0-9]+//;
	}
##	now else no use.
	#	else
	#	{
	#		print ":(\n";
	#	}
	return $srcstring;
}
sub remove_time_stamp {
	my ($srcstring) = @_;
	$_ = substr( $srcstring, 0, 12 );

	if (/[0-9]+:[0-9]+:[0-9]+:[0-9]+/) {
		return substr( $srcstring, 12 );
	}
}
sub remove_time_infomation {
	my ($srcstring) = @_;
	if ($srcstring =~ /[.]*[0-9]+\/[0-9]+\/[0-9]+   [0-9]+\:[0-9]+\:[0-9]+  [apAP][.]*/) {
	   #print ":)\n";
		$srcstring =~ s/[.]*[0-9]+\/[0-9]+\/[0-9]+   [0-9]+\:[0-9]+\:[0-9]+  [apAP][.]*/$1/;
	}
#	else
#	{
#	  print ":(\n";
#	}
	return $srcstring;
}


1;