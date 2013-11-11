#
# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use integer;

BEGIN
	{
	my $perlScriptPath=$0;
	$perlScriptPath=~s/\//\\/g; # replace any forward-slashes with back-slashes
	$perlScriptPath=~s/\\?[^\\]+$//; # get rid of this Perl-script's file-name
	if ($perlScriptPath eq '')
		{
		$perlScriptPath='..\group';
		}
	else
		{
		$perlScriptPath=~s/(\\?)[^\\]+$/$1group/;
		}
	unshift(@INC, $perlScriptPath); # can't do "use lib $perlScriptPath" here as "use lib" only seems to work with *hard-coded* directory names
	}
use PARSER;

if ((@ARGV==0) || ($ARGV[0]=~/\?/i) || ($ARGV[0]=~/-h/i) || ($ARGV[0]=~/\/h/i) || ($ARGV[0]=~/help/i))
	{
	die("\nVersion 021\n\nCharacter-set conversion-table generating tool\nCopyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reservered.\n\nUsage:\n\n\tperl analyse.pl <source-file> <output-file> foreign|Unicode [options]\n\nwhere the following options are available (each has a short form and a long form which are shown below separated by a '|'):\n\n\t-c | -columns(<a>: <b>, <c>)\n\t-p | -cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed\n\t-u | -sourceFilesToSubtract(<a>, <b>, ...)\n\n");
	}
my @columns=(2, 1, 2);
my $cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed=0;
my @sourceFilesToSubtract=();
&extractCommandLineFlags(\@columns, \$cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed, \@sourceFilesToSubtract);
my $sourceFileName=shift;
my $outputFileName=shift;
my $columnToSortOn=shift;
my %characterCodesOfOtherColumn=();
my %linesSorted=();
open(SOURCE_FILE, "< $sourceFileName") or die("Error: could not open \"$sourceFileName\" for reading");
&readSourceFile(\*SOURCE_FILE, $sourceFileName, \%characterCodesOfOtherColumn, \%linesSorted, $columnToSortOn, \@columns, $cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed, 0);
close(SOURCE_FILE) or die("Error: could not close \"$sourceFileName\"\n");
my $sourceFileToSubtract;
foreach $sourceFileToSubtract (@sourceFilesToSubtract)
	{
	open(SOURCE_FILE_TO_SUBTRACT, "< $sourceFileToSubtract") or die("Error: could not open \"$sourceFileToSubtract\" for reading\n");
	&readSourceFile(\*SOURCE_FILE_TO_SUBTRACT, $sourceFileToSubtract, \%characterCodesOfOtherColumn, \%linesSorted, $columnToSortOn, \@columns, $cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed, 1);
	close(SOURCE_FILE_TO_SUBTRACT) or die("Error: could not close \"$sourceFileToSubtract\"\n");
	}
open(OUTPUT_FILE, "> $outputFileName") or die("Error: could not open \"$outputFileName\" for writing");
my $numberOfBreaks=0;
my $numberOfMissingSpaces=0;
my $numberOfLinesSorted=0;
my $previousKey="";
my $offset=0;
my $key;
foreach $key (sort {$a<=>$b} (keys(%linesSorted)))
	{
	if ($previousKey ne "")
		{
		$previousKey<$key or die("Error: there appears to be a mix up with the keys \"$previousKey\" and \"$key\"");
		if ($previousKey!=$key-1)
			{
			++$numberOfBreaks;
			$numberOfMissingSpaces+=$key-$previousKey;
			print(OUTPUT_FILE "# End of contiguous block - relationship between the columns in this block: ".((!defined $offset)? "RANDOM": ($offset==0)? "DIRECT": "OFFSET ($offset)")."\n\n");
			}
		}
	if (($previousKey eq "") || ($previousKey!=$key-1))
		{
		$offset=$characterCodesOfOtherColumn{$key}-$key;
		}
	elsif ((defined $offset) && ($offset!=$characterCodesOfOtherColumn{$key}-$key))
		{
		undef $offset;
		}
	print(OUTPUT_FILE "$linesSorted{$key}");
	++$numberOfLinesSorted;
	$previousKey=$key;
	}
print(OUTPUT_FILE "# End of contiguous block - relationship between the columns in this block: ".((!defined $offset)? "RANDOM": ($offset==0)? "DIRECT": "OFFSET ($offset)")."\n\n");
close(OUTPUT_FILE);
my $maximumNumberOfIterationsWhenBinarySearching=1;
while (($numberOfLinesSorted>>$maximumNumberOfIterationsWhenBinarySearching)>0)
	{
	++$maximumNumberOfIterationsWhenBinarySearching;
	}
print("The number of breaks was $numberOfBreaks\nThe number of missing spaces was $numberOfMissingSpaces\nThe number of lines sorted was $numberOfLinesSorted\nThe maximum number of iterations when binary searching would be $maximumNumberOfIterationsWhenBinarySearching");

sub extractCommandLineFlags()
	{
	my $columns=shift;
	my $cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed=shift;
	my $sourceFilesToSubtract=shift;
	my $i;
	for ($i=0; $i<=$#ARGV;) # (i) not cache-ing $#ARGV into a variable as @ARGV may change length in this loop (ii) iterate forwards as some parameters may occupy more than one element in @ARGV
		{
		if (($ARGV[$i]=~/^-c\b(.*)$/i) || ($ARGV[$i]=~/^-columns\b(.*)$/i))
			{
			my $columnsData=$1;
			splice(@ARGV, $i, 1);
			for (;;)
				{
				if ($columnsData=~/^\s*\(\s*(\d+)\s*:\s*(\d+)\s*,\s*(\d+)\s*\)\s*$/)
					{
					@$columns=($1, $2, $3);
					last;
					}
				($#ARGV>=$i) or die("Error: bad \"-columns\" format\n");
				$columnsData.=(splice(@ARGV, $i, 1))[0];
				}
			}
		elsif (($ARGV[$i]=~/^-p$/i) || ($ARGV[$i]=~/^-cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed$/i))
			{
			splice(@ARGV, $i, 1);
			$$cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed=1;
			}
		elsif (($ARGV[$i]=~/^-u\b(.*)$/i) || ($ARGV[$i]=~/^-sourceFilesToSubtract\b(.*)$/i))
			{
			my $sourceFilesData=$1;
			splice(@ARGV, $i, 1);
			for (;;)
				{
				if ($sourceFilesData=~/^\s*\(\s*(.+)\)\s*$/)
					{
					my $sourceFilesData=$1;
					@$sourceFilesToSubtract=split(/,/, $sourceFilesData, -1);
					my $j;
					for ($j=$#$sourceFilesToSubtract; $j>=0; --$j)
						{
						$sourceFilesToSubtract->[$j]=~s/^\s+//;
						$sourceFilesToSubtract->[$j]=~s/\s+$//;
						($sourceFilesToSubtract->[$j] ne '') or die("Error: bad \"-sourceFilesToSubtract\" format (1)\n");
						}
					last;
					}
				($#ARGV>=$i) or die("Error: bad \"-sourceFilesToSubtract\" format (2)\n");
				$sourceFilesData.=(splice(@ARGV, $i, 1))[0];
				}
			}
		else
			{
			++$i;
			}
		}
	}

sub readSourceFile
	{
	my $fileHandle=shift;
	my $fileName=shift;
	my $characterCodesOfOtherColumn=shift;
	my $linesSorted=shift;
	my $columnToSortOn=shift;
	my $columns=shift;
	my $cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed=shift;
	my $subtract=shift;
	my $foreignCharacterCodeProcessingCode='';
	if (!(($columns->[0]>0) && ($columns->[1]>0) && ($columns->[2]>0) && ($columns->[1]<=$columns->[0]) && ($columns->[2]<=$columns->[0]) && ($columns->[1]!=$columns->[2])))
		{
		close($fileHandle);
		die("Error: bad \"-columns\" data\n");
		}
	my $patternOfLineContainingCharacterCodes=join('\s+', ('0x([0-9a-f]+)') x $columns->[0]);
	my $line;
	my $strippedDownLine;
	for (;;)
		{
		($line, $strippedDownLine)=&nextNonEmptyStrippedDownLine($fileHandle);
		if ($strippedDownLine eq '')
			{
			last;
			}
		if ($strippedDownLine=~/^SET_FOREIGN_CHARACTER_CODE_PROCESSING_CODE\s+(.*)$/i)
			{
			$foreignCharacterCodeProcessingCode=$1;
			}
		elsif ($strippedDownLine=~/^$patternOfLineContainingCharacterCodes$/i)
			{
			no strict 'refs'; # so that we can use symbolic references for $1, $2, etc
			my $foreignCharacterCode=hex(${$columns->[1]});
			my $unicodeCharacterCode=hex(${$columns->[2]});
			use strict 'refs';
			if ($foreignCharacterCodeProcessingCode ne '')
				{
				$foreignCharacterCode=eval($foreignCharacterCodeProcessingCode);
				}
			my $characterCodeOfColumnToSortOn;
			my $characterCodeOfOtherColumn;
			if ($columnToSortOn=~/^foreign$/i)
				{
				$characterCodeOfColumnToSortOn=$foreignCharacterCode;
				$characterCodeOfOtherColumn=$unicodeCharacterCode;
				}
			elsif ($columnToSortOn=~/^Unicode$/i)
				{
				$characterCodeOfColumnToSortOn=$unicodeCharacterCode;
				$characterCodeOfOtherColumn=$foreignCharacterCode;
				}
			else
				{
				die("Error: bad parameter \"$columnToSortOn\"");
				}
			if ((!$cutOutAnyPrivateUseUnicodeCharacterSlotsBeingUsed) || !((($unicodeCharacterCode>=0xe000) && ($unicodeCharacterCode<=0xf8ff)) || (($unicodeCharacterCode>=0xf0000) && ($unicodeCharacterCode<=0x10ffff))))
				{
				if ($subtract)
					{
					$linesSorted->{$characterCodeOfColumnToSortOn}='### '.$linesSorted->{$characterCodeOfColumnToSortOn};
					if ($characterCodesOfOtherColumn->{$characterCodeOfColumnToSortOn}!=$characterCodeOfOtherColumn)
						{
						printf(STDERR "Warning: 0x%x maps to 0x%x in the main source file, but to 0x%x in a source file to be extracted\n", $characterCodeOfColumnToSortOn, $characterCodesOfOtherColumn->{$characterCodeOfColumnToSortOn}, $characterCodeOfOtherColumn);
						}
					}
				else
					{
					$linesSorted->{$characterCodeOfColumnToSortOn}=$line;
					$characterCodesOfOtherColumn->{$characterCodeOfColumnToSortOn}=$characterCodeOfOtherColumn;
					}
				}
			}
		elsif ($line!~/^\s*0x([0-9a-f]+)\s*#\s*undefined.*$/i)
			{
			close($fileHandle);
			die("Error: unexpected line in \"$fileName\":\n    $line\n");
			}
		}
	}

