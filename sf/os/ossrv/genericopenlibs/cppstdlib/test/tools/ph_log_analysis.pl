#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#!C:\Perl\bin

#run this from a same drive as your epoc.
# set EPOCROOT=\ before you run


#$infile_exe="Q:\epoc32\release\winscw\udeb\z\phexe_list.bat";
#open (EXELIST, "$infile_exe");
#@exelist=<EXELIST>;
#$execount = 0;

$logfile="\\epoc32\\winscw\\c\\PH.log";
open(INFILED, $logfile);	
@lines=<INFILED>;
#print "lines = @lines[0]\n";

$outfile="PH_report.txt";
open(OUTFILE, ">$outfile");

$successful = 0;
$errors = 0;
$skipped = 0;


#@results;
foreach $line (@lines)	
{
   #print "comparing..\n";
   #if ($line =~ /individual/)
   #{
      #no need to write this
      #print OUTFILE $line;      
   #}
   if ($line =~ /successful test cases/)
   {
      #starting of a result
      print OUTFILE $line;            
      @success = split(/ /, $line);
      $successful += @success[1];
      #print OUTFILE "@success[1]\n";
   }
   if ($line =~ /errors/)
   {
      #starting of a result
      print OUTFILE $line;      
      @failures = split(/ /, $line);
      $errors += @failures[1];
   }
   if ($line =~ /skipped/)
   {
      #starting of a result
      print OUTFILE $line;    
      @skippedline = split(/ /, $line);
      $skipped += @skippedline[1];      
   }
   
}
print OUTFILE "************************ Final Summary ************************\n";
print OUTFILE "Total successful = $successful\n";
print OUTFILE "Total errors     = $errors\n";
print OUTFILE "Total skipped    = $skipped\n";
$totalcases = $successful+ $errors + $skipped;
print OUTFILE "Total test cases = $totalcases\n";
$passpercentage = $successful/$totalcases * 100;
print OUTFILE "Pass percentage  = $passpercentage\n";

print "\n\n************************ Final Summary ************************\n";
print "Total successful = $successful\n";
print "Total errors     = $errors\n";
print "Total skipped    = $skipped\n";
print "Total test cases = $totalcases\n";
print "Pass percentage  = $passpercentage\n";
print "***************************************************************\n";
#close (EXELIST);
close (INFILED);	
close (OUTFILE);
