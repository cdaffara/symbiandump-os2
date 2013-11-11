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
# Created by Relja Arandjelovic
# relja.arandjelovic@symbian.com
# Used to check for occurrences of "C: , EDriveC and [<index>]='C' and report warnings about it into HTML file
# USAGE:
# cdrive.pl -exclude=exclude.txt -excludedir=/common/generic/security/caf2/ -log=logfilename.htm path1 path2 ...
# log is created in $ENV{EPOCROOT}epoc32\\winscw\\c\\
# add -mytest option if the log should be stored in the execution directory
# (for example, if you are not using the script during overnight builds)
# exclude contains items formated as as follows (new line corresponds to new line in file):
# filename
# line content
# comment about why the warning should be excluded (may be in several lines, but with no line with only whice characters)
# line with only white characters
# NB: paths in cdrive.pl argumetns and filename in exclude list should be absolute paths
#

use strict;
use Getopt::Std;
use Getopt::Long;
use File::Find;

my @ListOfFiles=();
my @FileIsTest=();
my $nTests=0;
my %Exclude=();

my $exclude="";
my $excludedir="_no_dir_";
my $LogFileName="";



################
########
sub TypeOfWarning($$$$){
	my ($FileName,$FileIsTest,$linetext,$Warning)=@_;
	
	$linetext=~ s/^\s*//; $linetext=~ s/\s*$//;

	$$Warning="warning";
	
	if ($FileIsTest==1){
		$$Warning="test";
	} else {
		# check if in include list
	
		my $ExcludeLine;
		foreach $ExcludeLine (@{$Exclude{$FileName}}){
			if ($linetext eq $ExcludeLine) {
				$$Warning="ignoredwarning";
				last;
			}
		}		
	}
	
}
	
	
	
################
########


my $FirstWarning=1;
my $PrevFileName="_no_file_";

sub printHTML($$$$){
	my ($FileName,$linenumber,$linetext,$Warning)=@_;

	
	$linetext=~ s/^\s*//; $linetext=~ s/\s*$//;

	# convert special characters to HTML format
	$linetext=~ s/&/&amp;/sg;
	$linetext=~ s/</&lt;/sg;
	$linetext=~ s/>/&gt;/sg;
	

	if ($FileName ne $PrevFileName){
		print HTML "
<tr><td colspan=\"4\" height=\"20\">&nbsp;</td></tr>
	<tr><td width=\"20\"></td><td colspan=\"3\">
		$FileName
	</td></tr>	
";
	}
	
	print HTML "
<tr><td colspan=\"4\" height=\"10\">&nbsp;</td></tr>
		<tr class=\"$Warning\"><td width=\"20\"></td><td width=\"60\"></td><td colspan=\"2\">
			Line number: $linenumber
		</td></tr>
			<tr class=\"$Warning\"><td width=\"20\"></td><td width=\"60\"></td><td width=\"80\"></td><td>
$linetext
			</td></tr>
";
	
	$PrevFileName=$FileName;
	$FirstWarning=0;
}

################
########
my $prevDir="_no_previous_dir_";
my $isTest=0;

sub MakeList(){
	

	if (lc($File::Find::dir) =~ /^$excludedir/) { return; }
	if (lc($File::Find::dir) =~ /test/i) { return; }
	if (lc($File::Find::dir) =~ /examples/i) { return; }
	if (lc($File::Find::dir) =~ /tpkcs10/i) { return; }
	if (lc($File::Find::dir) =~ /tocsp/i) { return; }
	if (lc($File::Find::dir) =~ /referencedrmagent/i) { return; }
	if (lc($File::Find::dir) =~ /dumpswicertstoretool/i) { return; }
	if (!(lc($File::Find::name) =~ /\.(cpp|cxx|c\+\+|c|h)$/)){ return; }
	
	# include in list of files to be searched only if not test code
	# ie distribution policy doesn't include Reference/Test
	if ($prevDir ne lc($File::Find::dir)){ # new dir => search for distribution.policy	
		$isTest=0;
		my $policy="".($File::Find::dir)."/distribution.policy";
		
		if (-e $policy){
			open (POLICY , "<$policy" );
			while (<POLICY>){
				if ($_ =~ /Reference\/Test/){
					$isTest=1;
					last;
				}
			}
			close (POLICY);
		}
	}
	
	push(@ListOfFiles,lc($File::Find::name));
	push(@FileIsTest,$isTest);	
	if ($isTest) { $nTests++; }
	
	$prevDir=lc($File::Find::dir);
}

################
########
sub FindC($$$$){
	my ($FileName,$FileIsTest,$count,$countunique)=@_;
			
	open(SOURCE,"<$FileName") or die ("Could not open file: $FileName");
	
	my $prevCount=$$count;
	my $line; my $templine; my $linenumber=1;
	my $MultiLineComment=0;
	my $MadeChangeDueToComments;
	my $FirstLine=0;
	
	my $HTMLFirstWarning=1;
	
	while ($line=<SOURCE>){
	
		$templine=$line;
		$linenumber++;
		$FirstLine=0;
		
		# process comments
		do {
		
			$MadeChangeDueToComments=0;
		
			if ($MultiLineComment==0){
		
				# remove text in // coments 
				$templine=~ s/(.*?)\/\/.*$/$1/;
				# remove /* */ comments found in one line
				$templine=~ s/\/\*.*\*\///g;
			
				# if only /* then remove text after it and mark the start of comment
				if ($templine=~ /^(.*?)\/\*/){
					$templine=$1;
					$MultiLineComment=1;
					$MadeChangeDueToComments=1;
					$FirstLine=1;
				}
			
			} else {	# $MultiLineComment==1
		
				# if */ end comment
				if ($templine=~ /\*\/(.*)$/){
					$templine=$1;
					$MultiLineComment=0;
					$MadeChangeDueToComments=1;				
				}
			
			}
			
		} while ($MadeChangeDueToComments==1);
		# end of processing comments
		
		
		if ($MultiLineComment==1 && $FirstLine==0) { next; } # skip checking if in comment
	
	
		if (
			$templine=~ /["'][cC]:/	|| # '" # need comment for correct highlighting in codewarrior
			$templine=~ /EDriveC/		||
			$templine=~ /\[.+\]\s*=\s*'[cC]':/
			){
			
				my $Warning;
				TypeOfWarning($FileName,$FileIsTest,$line,\$Warning);
				printHTML($FileName,$linenumber,$line,$Warning);
				if ($Warning eq "warning") { $$count++; }
				
			}
		
	}
	
	close(SOURCE);
	
	if ($prevCount<$$count) { $$countunique++; }
}

################
########
sub ReadExcludeList(){

	#print "\n";
	
	
	if ($exclude eq ""){
		#print "Exclude list file not specified\nCarrying on without an exclude list\n";
		return;
	} elsif (!(-e $exclude)){
		#print "Exclude list file doesn't exist!\nCarrying on without an exclude list\n";
		return;
	}
	
	
	my $line; my $FileName; my $LineText; my $justification;
	
	open (EXCLUDE,"<$exclude");
	while (1){
	
		$line=<EXCLUDE> or last;
		$line=~ s/^(\s*)//g; $line=~ s/(\s*)$//g;
		$line=~ s/\\/\//g;
		$FileName=lc($line);
		
		$line=<EXCLUDE>; $line=~ s/^(\s*)//g; $line=~ s/(\s*)$//g;
		$LineText=$line;
		
		$justification=0;
		while($line=<EXCLUDE>){
			$line=~ s/^(\s*)//g; $line=~ s/(\s*)$//g;
			if ($line eq "") { last;}
			$justification=1;
		}
		
		if ($justification==0){
			#print "Not added to the excludion list since no justification found for:\n$FileName\n$LineText\n";
		} else {		
			push(@{$Exclude{$FileName}},$LineText);
		}
		
	}
	close(EXCLUDE);
	
}

################
######## Main

	
	my $MyTest=0;
	
	GetOptions(
			"exclude=s" => \$exclude ,
			"excludedir=s" => \$excludedir, # one dir to be excluded from scaning (along wih its subdirs)
			"log=s" => \$LogFileName,
			"mytest" => \$MyTest # inteded for my personal testing (so that path is not the one for overnight testing)
				);
	
	$excludedir=~ s/\\/\//g; # convert \ into / so that it matches perl's path form
	
	ReadExcludeList();

	
	if ($MyTest==0){ # overnight
		$LogFileName = "$ENV{EPOCROOT}epoc32\\winscw\\c\\".$LogFileName;
	}

			
	my $iArgv;
	for ($iArgv=0;$iArgv<@ARGV;$iArgv++){
		$ARGV[$iArgv]=~ s/\\/\//g; # convert \ into / so that it matches perl's path form
		find(\&MakeList, ($ARGV[$iArgv]) );
	}

	
	open(HTML,">$LogFileName");
	print HTML "
<html>

<head>
	<title>CDrive warnings</title>
	
	<style type=\'text/css\'>
.warning {
	background-color: #FFAAAA;
}
.ignoredwarning {
	background-color: #90ffff;
}
.test{
	background-color: #ffc060;
}
	</style>
</head>
<body>	
	<table border=\"0\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">
";
	

	my $count=0; my $countunique=0;
	my $iList; my $nList=@ListOfFiles;
	for ($iList=0;$iList<$nList;$iList++){
		FindC($ListOfFiles[$iList],$FileIsTest[$iList],\$count,\$countunique);
	}

	
	print HTML "\n</table>\n";

	my $total=$nList-$nTests;
	# workaround in order to be reported to the standardised system	
	print HTML "<br><center>$countunique tests failed out of $total\n</center>\n";	
	
	print HTML "
</body>
</html>	
	";
	
	#print "\n\tNumber of files:\t$nList\n";
	#print "\n\tNumber of warnings:\t$count\n";
	#print "\n\tNumber of unique warnings:\t$countunique\n";
	
	close(HTML);

################
######## end-Main

