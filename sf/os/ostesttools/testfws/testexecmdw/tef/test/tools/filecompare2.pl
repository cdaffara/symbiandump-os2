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


#main program starts
my ($srcdir, $dstdir) =@ARGV;
if (@ARGV != 2)
{
 print ("Usage: perl filecompare2.pl <template_dir> <test_result_dir> \n");
 print ("E.g. perl filecompare2.pl d:\template d:\testresult \n");
 print ("the compare result file is located at d:\compareresult.txt \n");
 exit;
}
print "$srcdir \n";
print "$dstdir \n";
open (CompareResultFile, ">d:\\compareresult.txt") || die ("open compareresult.txt failed");
chdir ($srcdir) || die "chdir to $srcdir faield.\n";
my $thisdir ;
opendir ($thisdir, $srcdir);
my @files = readdir($thisdir);
closedir($thisdir);
$count =1;
my @srcfiles;
my @dstfiles;

my $count2=1;
while ($count < @files) 
{
 if ( (!-d @files[$count] )&&  (@files[$count] =~/dt-tef-command-line-/)) # (@files[$count] =~/\.htm/) to select all *.htm files
 {
   @srcfiles[$count2] =$srcdir . "\\" . "@files[$count]";
   print "@srcfiles[$count] \n";
   @dstfiles[$count2] = $dstdir ."\\" . "@files[$count]";
   print "@dstfiles[$count] \n" ;
   $count2++;
 }
  $count++;
}
$count = 1;
while ($count < @srcfiles)
{
	&filecompare(@srcfiles[$count], @dstfiles[$count]);
	$count++;
}
close( CompareResultFile);
# end of the main program

sub filecompare
{
my($srcfile, $dstfile) = @_;
#open (BASELINEFILE, "$srcfile") || die ("Could not open Baseline file $srcfile"); 
#open (TERESULTFILE, "$dstfile") || die ("Could not open Test Result file $dstfile"); 
 if ( ! -e $srcfile)
 {
   print  ("$srcfile does not exist \n");
   print CompareResultFile ("$srcfile does not exist \n");
   return;
 }
 elsif ( !-e $dstfile)
 {
  print ("$dstfile does not exist \n");
    print CompareResultFile ("$dstfile does not exist \n");
  return;
 } 
 if (open (BASELINEFILE, "$srcfile") && open (TERESULTFILE, "$dstfile") )
 {
	my @baselinearray = <BASELINEFILE>;
	my @testresultarray = <TERESULTFILE>;
	my $MisMatchNum = 0;
	close (BASELINEFILE);
	close(TERESULTFILE);
	my $linecount = 1; 
	my $BaselineRowContent;
	my $BaselineRowContentChop;
	my $TestresultRowContent;
	my $TestresultRowContentChop;
	my $choplength = 25; # the first 26 characters contains time information and thread no which change every run.
    my $flag = 0;        
		
	while ($linecount <= @baselinearray) 
	  {
		  $BaselineRowContent =  lc(@baselinearray[$linecount-1]);
		  $TestresultRowContent = lc(@testresultarray[$linecount-1]);
		 
		  if ($flag) # onlly the line after the "TestExecute Started " are to be compared, before that line are report title
		  {
			  $BaselineRowContentChop = $BaselineRowContent;
			  $TestresultRowContentChop = $TestresultRowContent;
		     
			  substr ($BaselineRowContentChop,0,$choplength)="";
			  substr ($TestresultRowContentChop,0,$choplength)="";
			
			   if ($BaselineRowContentChop ne $TestresultRowContentChop)
			   { 
					print ("$srcfile line $linecount does not match \n");
					print CompareResultFile ("$srcfile line $linecount does not match \n");
					$MisMatchNum++;
			   }
			   else #if two lines match
			   {
			    # comment the below because they print too much information, they can be uncommented when debugging
				#print ("$srcfile line $linecount match \n");
				#print CompareResultFile (" $srcfile line $linecount match \n");
				#print CompareResultFile ("  $srcfile:    line $linecount: $BaselineRowContentChop \n");
				#print CompareResultFile ("  $dstfile:    line $linecount: $TestresultRowContentChop \n");
			   }
			}
			else
			{
			  if ( $BaselineRowContent =~/TestExecute Started/i)  # set flag if it meets the line containing "TestExecute Started " , the lines after that line are to be compared, before that line are report title
			  {
			   $flag++;
			  # print "the flag is: $flag \n";
			  	   
			  }
			}
		  $linecount++;
	  
	  }
	  if ($MisMatchNum == 0)
	  {
	    print ("$srcfile and $dstfile match \n");
		print CompareResultFile (" $srcfile and $dstfile match \n");
	  }
  }
  else
   {
     print ("$srcfile or $dstfile can not open \n");
	 print CompareResultFile ("$srcfile or $dstfile can not open \n");
	 close (BASELINEFILE);
	 close(TERESULTFILE);
   }

}
