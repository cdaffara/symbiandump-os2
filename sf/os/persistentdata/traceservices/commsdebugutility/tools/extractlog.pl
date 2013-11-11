# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use strict;
use Getopt::Long;

# read in the command line parameters
my $verbose = 0;
my $singleLevel = 0;
my $fromBatchFile = 0;
my $help = 0;
GetOptions( 'verbose|v!' => \$verbose,
			'1!' => \$singleLevel,
			'help|h|?!' => \$help,
			'--fromBatchFile!' => \$fromBatchFile );
Usage() if $help;
my $file  = $ARGV[0];
if(!defined($file))
{
	$file = "$ENV{EPOCROOT}EPOC32\\WINSCW\\C\\LOGS\\LOG.TXT";
	$file = "$ENV{EPOCROOT}EPOC32\\WINS\\C\\LOGS\\LOG.TXT" if(!-r $file);
	print "No file specified, defaulting to $file\n";
}
print "Verbose mode on\n" if $verbose;

# open up the file for input
open(INPUT_FILE, "< $file") or die "ERROR: Couldn't open the file \"$file\" for input:";

# read file without intepretting CR/LF - not ideal on window's systems, but
# I introduced this to get around apparent problem when seeking back when reading binary data
# The seek back seemed to seek to somewhere other than where we had last "tell"'ed.
# binmode fixes this since CR/LF is always seen as 2 distinct chars.
binmode(INPUT_FILE);

# declare handy arrays
my %file_handles_text;   # a hash of all components seen for ascii and their associated file handle
my %file_handles_binary; # a hash of all components seen for binary and their associated file handle
my @comments;       # an array of all the comment lines seen so far

# iterate through each line of the file
my $filepos = tell(INPUT_FILE);
my $linenum = 1;
while (<INPUT_FILE>) {
    # strip off any CR/LF's since we will correctly terminate the line by one
    # if we find the line is ascii. Binary lines should also arrive with a CR/LF at their end.
    my $line = $_;
    print $line if $verbose;
    chomp($line);

    my $strlen = length($line);
    if (($strlen == 0) || (ord "$line" == 13)) {
       if ($verbose) { print "Line#: $linenum is an empty line.\n"; }
    } else {
		$line =~ s/\r$//;	# chomp CR
        if ($line =~ /^#/) {
            # we are dealing with a comment line
            $line .= "\n";
            push(@comments, "$line");
            while((my $component, my $file_handle) = each(%file_handles_text)) {
                print $file_handle "$line"
            }
        }
        else {
            #we are dealing with a component line so check the correct line format and determine if it is ascii or binary
            my @list = split("\t", $line);

            my $linetype = "$list[2]";

            if (ord "$linetype" == 97) {
                # this is ascii, so return a CR/LF to line
                $line .= "\n";
                my $component = lc($list[0] . ($singleLevel? "": "_$list[1]") . ".log");
                if (not exists $file_handles_text{$component}) {
                    # we haven't seen this component before
                    local *OUTFILE;
                    open(OUTFILE, "> $component") or die "ERROR. Line $linenum: can't open file \"$component\" for output: $!";
                    foreach my $comment (@comments) {
                        print OUTFILE "$comment";
                    }
                    $file_handles_text{$component} = *OUTFILE;
                }
                my $file_handle = $file_handles_text{$component};
                print $file_handle "$line";
            } elsif (ord "$linetype" == 98) {
                #assume it is binary
                my $component = "$list[0]_$list[1].bin";
                if (not exists $file_handles_binary{$component}) {
                    # we haven't seen this component before
                    local *OUTFILE;
                    open(OUTFILE, "> $component") or die "ERROR. Line $linenum: can't open file \"$component\" for output: $!";
                    binmode(OUTFILE);
                    $file_handles_binary{$component} = *OUTFILE;
                }

                # go back to start of line since CR/LF might even occur in the length field,
                # then read to each tab until the binary tag is found again
                seek(INPUT_FILE,$filepos,0);

                my $old_rs = $/;
                $/ = chr(9);
                do {
                    $_ = <INPUT_FILE>;
                } while ((ord "$_" != 98) && (length != 1));
                $/ = $old_rs;

                # read the length as a "little-endian" 4-byte unsigned integer
				my $datalenstr;
                my $numread = read(INPUT_FILE,$datalenstr,4);
                if ($numread != 4) { die "ERROR: did not read all 4 bytes binary data length on line $linenum"; }

                my $datatoread = unpack("v",$datalenstr);

                # read next char and ensure it is a tab
				my $onechar;
                $numread = read(INPUT_FILE,$onechar,1);
                if ($numread != 1) { die "ERROR: read error when reading tab char in binary line on line $linenum"; }
                if (ord $onechar != 9) { die "ERROR: No tab char in binary line at pos : $filepos where expected between length and data at line $linenum"; }

                if ($verbose) {
                    print "About to read binary data. Beginning of line is at pos: $filepos ";
                    printf " [%X] Tags: ",$filepos;
                    foreach my $d (@list) {
                       print "[$d]";
                    }
                    print "\nBytes to read: $datatoread\n";
                }

				my $datastr;
                $numread = read(INPUT_FILE,$datastr,$datatoread);
                if ($datatoread != $numread) { die "ERROR: only read $numread of expected $datatoread bytes binary data on line $linenum"; }

                if ($verbose) { print "Read Completed.\n"; }


                my $file_handle = $file_handles_binary{$component};
                print $file_handle "$datastr";

                # finally, read in the trailing CR/LF before reverting to textual reading
                $_ = <INPUT_FILE>;
        
            } else {
                if ($verbose) {
                    print "Line# $linenum at filepos $filepos is CORRUPT: linetype field is neither ascii nor binary so ignoring.\n";
                    print "Line fields: ";
                    foreach my $d (@list) {
                       print "[$d]";
                    }
                    print "\n     ... skipping line and continuing\n";
                }

            }
        } # comment line check
    } #empty line check
    $filepos = tell(INPUT_FILE);
    #print "filepos at end of line proc: $filepos . input rec #: $.";
    #printf "[%X]\n",$filepos;
    $linenum++;
}

# and finally clean up
close(INPUT_FILE) or die "ERROR: failed to close the input log file: $!";
my $firstFile = 1;
while((my $component, my $file_handle) = each(%file_handles_text)) 
{
	printf "%s%s", $firstFile? "Generated: ": ", ", $component;
	$firstFile = 0;
    close($file_handle) or die "ERROR: failed to close the output log file for \"$component\": $!";
}
while((my $component, my $file_handle) = each(%file_handles_binary)) 
{
	printf "%s%s", $firstFile? "Generated: ": ", ", $component;
	$firstFile = 0;
    close($file_handle) or die "ERROR: failed to close the output log file for \"$component\": $!";
}
print "\n";


sub Usage () {
print $fromBatchFile? "Common Usage: splitlog [logfile] [-v][-1]\n": "Args: [logfile] [-v][-1]\n";
    print <<ENDHERESTRING;
Where:
  [logfile] = CDU log file (default: {EPOCROOT}\\EPOC32\\WINS[CW]\\C\\LOGS\\LOG.TXT)
  [-v]      = verbose output
  [-1]      = split only at component level (one file per unique first tag)
   
Input Log file format:
 as specified in the FLOGGER documentation.  Briefly, there are three
 types of log messages:
   1) Comment lines.  These contain a '#' character at the beginning of
      the line.
   2) Ascii Log lines.  Contains tab separated fields - the first being the 
      name of the component that logged that particular message.
   3) Binary Log lines. Same fields as an ascii line but then contains
      a specified number of bytes of raw data. 

Output:
 This script will create a number of files in the current 
 directory - one for each component found in the input log file.  Each
 file will have the same name as the first two tags separated by an underscore.
 If the component is logging ascii test, its output log file will contain all
 comment lines, and all log lines relating to that particular component.
 If the component is logging binary data, only the actual data is placed in
 the output log file.
 Ascii output files are given the extension *.log while binary files have
 extension *.bin

"Out of Memory":
  error may mean the file is corrupt and the script tried to
  read a section of binary data which isn't there. Try rerunning with the "-v"
  flag to help diagnose. File position numbers reported should correlate exactly
  with the byte position if displaying file in a hex editor.
ENDHERESTRING

    exit 1;
}
