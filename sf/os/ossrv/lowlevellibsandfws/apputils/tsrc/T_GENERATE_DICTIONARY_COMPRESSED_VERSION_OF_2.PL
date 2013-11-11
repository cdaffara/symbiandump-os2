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

my $numberOfBitsUsedForDictionaryTokens=3;
my @dictionarySource=
	(
	'{1}data\\',
	'z:\system\\',
	'ikon{3}',
	'.mbm',
	'my_{5}',
	'program',
	);
my @resourceSource=
	(
	'<C,18>My {5} is cool',
	'<C,24>{0}e{2}',
	'<V,9174804><C,6><V,-6208493><v,0x3176>',
	'<V,-120727><V,-82385253><v,0x3176>',
	'<C,0><C,2><C,2><C,24><C,24>{0}u{2}<C,1><C,40><C,40>{1}apps\{4}\{4}{3}',
	);

my $outputFile=shift;
my $dictionaryDataAndIndex='';
my $resourceDataAndIndex='';
&generateDataAndIndexFromSource(\$dictionaryDataAndIndex, \@dictionarySource);
&generateDataAndIndexFromSource(\$resourceDataAndIndex, \@resourceSource);
(system('uidcrc 0x101f5010 0 0x00018bf1 '.$outputFile)==0) or die;
(-s($outputFile)==16) or die;
open(OUTPUT_INDEX_FILE, '>> '.$outputFile) or die;
binmode(OUTPUT_INDEX_FILE);
print(OUTPUT_INDEX_FILE pack('Cv2', 0xc0|($numberOfBitsUsedForDictionaryTokens-3), 300, 16+5+1+length($dictionaryDataAndIndex))); # the 5-byte header for dictionary-compressed resource files
print(OUTPUT_INDEX_FILE pack('C', 0x13)); # the bit-array of resources containing compressed Unicode
print(OUTPUT_INDEX_FILE $dictionaryDataAndIndex);
print(OUTPUT_INDEX_FILE $resourceDataAndIndex);
close(OUTPUT_INDEX_FILE) or die;

sub generateDataAndIndexFromSource
	{
	my $dataAndIndex=shift;
	my $source=shift;
	my $dataBits='';
	my $numberOfDataBits=0;
	my @dataIndex=();
	foreach my $line (@$source)
		{
		($line=~/^(.*)$/) or die; $line=$1; # get rid of the end-of-line character(s) (these, by default, are not matched by '.')
		if ($line ne '')
			{
			my $remainder=$line;
			$line='';
			for (;;)
				{
				if ($remainder!~/^(.*?)\<(.*)$/)
					{
					$line.=$remainder;
					last;
					}
				$line.=$1;
				$remainder=$2;
				($remainder=~/^([a-z])\,(.+?)\>(.*)$/i) or die;
				my $template=$1;
				my $hexadecimalInteger=$2;
				$remainder=$3;
				if ($hexadecimalInteger=~/^(0x[0-9a-f]+)$/i)
					{
					$hexadecimalInteger=hex($hexadecimalInteger);
					}
				else
					{
					($hexadecimalInteger=~/^\-?\d+$/i) or die;
					}
				$line.=pack($template, $hexadecimalInteger);
				}
			my $oldNumberOfDataBits=$numberOfDataBits;
			$remainder=$line;
			for (;;)
				{
				if ($remainder!~/^(.*?)\{(.*)$/)
					{
					&appendPrefixedStringToDataBits(\$dataBits, \$numberOfDataBits, $remainder);
					$remainder='';
					last;
					}
				my $pre=$1;
				my $post=$2;
				($post=~/^(\d+)\}(.*)$/) or die;
				&appendPrefixedStringToDataBits(\$dataBits, \$numberOfDataBits, $pre);
				my $dictionaryIndex=$1;
				$remainder=$2;
				&appendBitsToDataBits(\$dataBits, \$numberOfDataBits, 1, 0);
				&appendBitsToDataBits(\$dataBits, \$numberOfDataBits, $numberOfBitsUsedForDictionaryTokens, $dictionaryIndex);
				}
			(($numberOfDataBits&0xffff)==$numberOfDataBits) or die;
			push(@dataIndex, $numberOfDataBits);
			}
		}
	(length($dataBits)==($numberOfDataBits+7)/8) or die;
	$$dataAndIndex=$dataBits;
	$$dataAndIndex.=pack('v*', @dataIndex);
	}

sub appendPrefixedStringToDataBits
	{
	my $dataBits=shift;
	my $numberOfDataBits=shift;
	my $string=shift;
	for (;;)
		{
		my $remainder='';
		my $lengthOfString=length($string);
		if ($lengthOfString==0)
			{
			last;
			}
		if ($lengthOfString==1)
			{
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 2, 0x1); # 1 0
			}
		elsif ($lengthOfString==2)
			{
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 3, 0x3); # 1 1 0
			}
		elsif ($lengthOfString<11)
			{
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 4, 0x7); # 1 1 1 0
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 3, $lengthOfString-3);
			}
		else
			{
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 4, 0xf); # 1 1 1 1
			if ($lengthOfString>266)
				{
				$remainder=substr($string, 266);
				substr($string, 266)='';
				$lengthOfString=length($string);
				($lengthOfString==266) or die;
				}
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 8, $lengthOfString-11);
			}
		foreach my $byte (unpack('C*', $string))
			{
			&appendBitsToDataBits($dataBits, $numberOfDataBits, 8, $byte);
			}
		$string=$remainder;
		}
	}

sub appendBitsToDataBits
	{
	my $dataBits=shift;
	my $numberOfDataBits=shift;
	my $numberOfBitsToAppend=shift;
	my $bitsToAppend=shift;
	for (my $i=0; $i<$numberOfBitsToAppend; ++$i)
		{
		vec($$dataBits, $$numberOfDataBits, 1)=(($bitsToAppend>>$i)&1);
		++$$numberOfDataBits;
		}
	}

