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
use FindBin;

package WRITER;
require Exporter;
@WRITER::ISA=qw(Exporter);
@WRITER::EXPORT=qw(write8 write16 write32 writePositiveIntegerCompacted15 writePositiveIntegerCompacted30 writeSignedIntegerCompacted29 writeString writeUids);

sub write8
	{
	my $fileHandle=shift;
	my $integer=shift;
	if ($integer&0xffffff00)
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is too large to write into 8 bits\n");
		}
	printf $fileHandle "%c", $integer;
	}

sub write16 # little-endian
	{
	my $fileHandle=shift;
	my $integer=shift;
	if ($integer&0xffff0000)
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is too large to write into 8 bits\n");
		}
	&write8($fileHandle, $integer&0x000000ff);
	&write8($fileHandle, ($integer>>8)&0x000000ff);
	}

sub write32 # little-endian
	{
	my $fileHandle=shift;
	my $integer=shift;
	&write8($fileHandle, $integer&0x000000ff);
	&write8($fileHandle, ($integer>>8)&0x000000ff);
	&write8($fileHandle, ($integer>>16)&0x000000ff);
	&write8($fileHandle, ($integer>>24)&0x000000ff);
	}

sub writePositiveIntegerCompacted15 # big-endian
	{
	my $fileHandle=shift;
	my $integer=shift;
	if ($integer<0)
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is negative\n");
		}
	if ($integer<0x00000080)
		{
		&write8($fileHandle, $integer);
		}
	elsif ($integer<0x00008000)
		{
		&write8($fileHandle, 0x00000080|(($integer>>8)&0x0000007f));
		&write8($fileHandle, $integer&0x000000ff);
		}
	else
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is too large to write into 15 bits\n");
		}
	}

sub writePositiveIntegerCompacted30 # big-endian
	{
	my $fileHandle=shift;
	my $integer=shift;
	if ($integer<0)
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is negative\n");
		}
	if ($integer<0x00000080)
		{
		&write8($fileHandle, $integer);
		}
	elsif ($integer<0x00004000)
		{
		&write8($fileHandle, 0x00000080|(($integer>>8)&0x0000003f));
		&write8($fileHandle, $integer&0x000000ff);
		}
	elsif ($integer<0x40000000)
		{
		&write8($fileHandle, 0x000000c0|(($integer>>24)&0x0000003f));
		&write8($fileHandle, ($integer>>16)&0x000000ff);
		&write8($fileHandle, ($integer>>8)&0x000000ff);
		&write8($fileHandle, $integer&0x000000ff);
		}
	else
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is too large to write into 30 bits\n");
		}
	}

sub writeSignedIntegerCompacted29 # big-endian
	{
	my $fileHandle=shift;
	my $integer=shift;
	my $negativeFlag=0x00000000;
	if ($integer<0)
		{
		$negativeFlag=0x00000080;
		if (($integer^-$integer)==0)
			{
			die("Error: the integer ".sprintf("0x%08x", $integer)." is too large to write into 29 bits\n");
			}
		$integer=-$integer;
		}
	if ($integer<0x00000040)
		{
		&write8($fileHandle, $negativeFlag|$integer);
		}
	elsif ($integer<0x00002000)
		{
		&write8($fileHandle, $negativeFlag|0x00000040|(($integer>>8)&0x0000001f));
		&write8($fileHandle, $integer&0x000000ff);
		}
	elsif ($integer<0x20000000)
		{
		&write8($fileHandle, $negativeFlag|0x00000060|(($integer>>24)&0x0000001f));
		&write8($fileHandle, ($integer>>16)&0x000000ff);
		&write8($fileHandle, ($integer>>8)&0x000000ff);
		&write8($fileHandle, $integer&0x000000ff);
		}
	else
		{
		die("Error: the integer ".sprintf("0x%08x", $integer)." is too large to write into 30 bits\n");
		}
	}

sub writeString
	{
	my $fileHandle=shift;
	my $string=shift;
	my $lengthOfString=length($string);
	my $i;
	for ($i=0; $i<$lengthOfString; ++$i)
		{
		my $byteAsNumber;
		($byteAsNumber)=unpack("C", substr($string, $i, 1));
		&write8($fileHandle, $byteAsNumber);
		}
	}

sub writeUids
	{
	my $fileHandle=shift;
	my $uid1=shift;
	my $uid2=shift;
	my $uid3=shift;
	use Cwd;
	my $uidsin = sprintf("0x%08x 0x%08x 0x%08x ", $uid1, $uid2, $uid3);

	# Locate uidcrc on the same path as this module, and if not found then run through environment

  	my $uidsout = "";
  	my $uidlocation = "$FindBin::Bin/uidcrc";
  	if(-e $uidlocation or -e $uidlocation.".exe") {
		$uidsout = `$uidlocation $uidsin` or die('Error: could not find UIDCRC at '.$uidlocation ); }
	else {
		$uidsout = `uidcrc $uidsin` or die('Error: could not find UIDCRC on PATH');
	}

	die ('Did not understand output of UIDCRC')
		unless $uidsout =~ /^0x([0-9a-fA-F]{8}) 0x([0-9a-fA-F]{8}) 0x([0-9a-fA-F]{8}) 0x([0-9a-fA-F]{8})[ \n\r\t]*$/s;
	my $uidCrcBinaryBuffer = pack 'V4', hex($1), hex($2), hex($3), hex($4);
	&writeString($fileHandle, $uidCrcBinaryBuffer);
	}

