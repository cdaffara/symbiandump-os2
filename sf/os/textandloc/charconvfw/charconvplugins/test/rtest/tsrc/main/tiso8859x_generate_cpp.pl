#
# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

my $extensionRoot = $ARGV[0];
my @iso8859TestData=();
my $inputFileName;

chdir($extensionRoot);	# change directory from EPOC32\BUILD\development\personal\andrew\MCL\charconv\ongoing\group\wrappermakefiles to charconv\ongoing\group
chdir('../data');		# change directory from charconv\plugins\group to charconv\plugins\data

foreach $inputFileName (glob('ISO8859*.TXT'))
	{
	my $item=[];
	($inputFileName=~/^ISO8859(\d+)\.TXT$/i) or die;
	$item->[0]=$1;
	open(INPUT_FILE, "< $inputFileName") or die;
	my $line;
	while ($line=<INPUT_FILE>)
		{
		$line=~s/#.*$//;
		$line=~s/^\s*//;
		$line=~s/\s*$//;
		if ($line ne '')
			{
			($line=~/(0x[0-9a-f]{2})\s+(0x[0-9a-f]{4})/i) or die;
			push(@{$item->[1]}, lc($1));
			push(@{$item->[2]}, lc($2));
			}
		}
	close(INPUT_FILE) or die;
	push(@iso8859TestData, $item);
	}
@iso8859TestData=sort({$a->[0]<=>$b->[0]} @iso8859TestData);
my $outputFileName='g_tiso8859x.cpp';
open(OUTPUT_FILE, '>', "../test/rtest/tsrc/main/$outputFileName") or die; # we're currently standing in 'charconv\plugins\data' and we want the generated file to go into 'charconv\plugins\test\rtest\tsrc\main'.
print(OUTPUT_FILE "// $outputFileName\n//\n// Copyright (c) Nokia Corporation ".(1900+(gmtime(time))[5]).". All rights reserved.\n//\n\n");
print(OUTPUT_FILE "#include <e32std.h>\n#include <charconv.h>\n#include \"tiso8859x.h\"\n\n");
my $numberOfItems=@iso8859TestData;
my $i;
for ($i=0; $i<$numberOfItems; ++$i)
	{
	my $item=$iso8859TestData[$i];
	my $x=$item->[0];
	(scalar(@{$item->[1]})==scalar(@{$item->[2]})) or die;
	my $textLength=@{$item->[1]};
	my $j;
	print(OUTPUT_FILE "LOCAL_D const TUint16 iso8859_".$x."_unicode\[\]=\n\t\{\n");
	for ($j=0; $j<$textLength; ++$j)
		{
		print(OUTPUT_FILE "\t$item->[2]->[$j]");
		if ($j<$textLength-1)
			{
			print(OUTPUT_FILE ',');
			}
		print(OUTPUT_FILE "\n");
		}
	print(OUTPUT_FILE "\t\};\n\n");
	print(OUTPUT_FILE "LOCAL_D const TUint8 iso8859_".$x."_iso8859\[\]=\n\t\{\n");
	for ($j=0; $j<$textLength; ++$j)
		{
		print(OUTPUT_FILE "\t$item->[1]->[$j]");
		if ($j<$textLength-1)
			{
			print(OUTPUT_FILE ',');
			}
		print(OUTPUT_FILE "\n");
		}
	print(OUTPUT_FILE "\t\};\n\n");
	}
print(OUTPUT_FILE "LOCAL_D const STstIso8859TestDataItem iso8859TestDataItems\[\]=\n\t\{\n");
for ($i=0; $i<$numberOfItems; ++$i)
	{
	my $item=$iso8859TestData[$i];
	my $x=$item->[0];
	(scalar(@{$item->[1]})==scalar(@{$item->[2]})) or die;
	my $textLength=@{$item->[1]};
	print(OUTPUT_FILE "\t\t\{\n");
	print(OUTPUT_FILE "\t\t$x,\n\t\tKCharacterSetIdentifierIso8859$x,\n\t\t$textLength,\n\t\tiso8859_".$x."_unicode,\n\t\tiso8859_".$x."_iso8859\n");
	print(OUTPUT_FILE "\t\t\}");
	if ($i<$numberOfItems-1)
		{
		print(OUTPUT_FILE ',');
		}
	print(OUTPUT_FILE "\n");
	}
print(OUTPUT_FILE "\t\};\n\n");
print(OUTPUT_FILE "GLDEF_D const STstIso8859TestData iso8859TestData=\n\t\{\n");
print(OUTPUT_FILE "\t$numberOfItems,\n\tiso8859TestDataItems\n");
print(OUTPUT_FILE "\t\};\n\n");
close(OUTPUT_FILE) or die;

