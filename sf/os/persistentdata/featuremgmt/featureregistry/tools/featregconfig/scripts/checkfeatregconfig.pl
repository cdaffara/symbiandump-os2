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
# Feature Registry Configuration File Lister/Checker
# 
#

use strict;

# Usage help text
my $usage = "Feature Registry Configuration File Lister/Checker\n" .
			"Copyright (c) 2005 Symbian Software Ltd. All rights reserved\n\n" .
			"Usage: perl $0 BINCONFIGFILE\n\n" .
			"Returns 0 if config file is valid, non-zero and reasons otherwise\n";

# Process command line
if (1 != scalar(@ARGV))
	{
	print $usage;
	exit 1;
	}
my $binConfigFile = $ARGV[0];

# open config file in binary mode
my $BINCONFIG;
if (!open ($BINCONFIG, $binConfigFile))
	{
	print "ERROR: Binary config file is missing or cannot be opened";
	exit 1;
	}
binmode $BINCONFIG;

# Check file size
my ($dev, $ino, $mode, $nlink, $_uid, $_gid, $redv, $size, $atime, $mtime, $ctime, $blksize, $blocks)
	= stat $BINCONFIG;
my $headerSize = 16;
if ($size < $headerSize)
	{
	print "INVALID Feature Registry Config File: file is too small, must have at least 16 byte header";
	exit 1;
	}
my $KMaxLargePropertySize = 65535;
if ($size > $KMaxLargePropertySize)
	{
	print "INVALID Feature Registry Config File: File is larger than limit of 65535 bytes";
	exit 1;
	}
if ($size > 4096) # arbitrarily chosen; it may be sensible to change the way featreg works if config files become this big
	{
	print "WARNING: Large Feature Registry config file (", $ARGV[0], "); something may be wrong\n";
	}

# Read and check header
my $packedHeader;
if (!read($BINCONFIG, $packedHeader, $headerSize) ||
	(length $packedHeader < 16))
	{
	print "INVALID Feature Registry Config File: Could not read 16-byte header";
	exit 1;
	}
my ($feat, $version, $entryCount, $rangeCount) = unpack 'a4I3', $packedHeader;
if ($feat ne 'feat')
	{
	print "INVALID Feature Registry Config File: First 4 bytes of header must be ASCII 'f' 'e' 'a' 't'";
	exit 1;
	}
if ($version != 0)
	{
	print "INVALID Feature Registry Config File: Second 32-bit word in header (version/flags) must be zero";
	exit 1;
	}
my $entrySize = 8;
my $rangeSize = 8;
my $expectedSize = $headerSize + $entryCount*$entrySize + $rangeCount*$rangeSize;
if ($expectedSize != $size)
	{
	print "INVALID Feature Registry Config File: Size of file ($size bytes) does not match size expected from header" .
		  " ($expectedSize bytes) for $entryCount feature entries and $rangeCount default ranges";
	exit 1;
	}

# Write information from the header:
print "Feature Registry Configuration file '$binConfigFile'\n";
print "Contains $entryCount feature entries and $rangeCount default ranges\n";

print "---\n";

# Check individual feature entries are listed by increasing (and non-repeating) UID, and write them to STDOUT
my $lastFeatureUid = 0;
for (my $e = 0; $e < $entryCount; $e++)
	{
	my $packedEntry;
	if (!read($BINCONFIG, $packedEntry, $entrySize) ||
		(length $packedEntry < $entrySize))
		{
		print "INVALID Feature Registry Config File: Unable to read feature entry $e";
		exit 1;
		}
	my ($featureUid, $status) = unpack 'I2', $packedEntry;
	printf "Feature UID 0x%08X, status = 0x%08X (%s)\n", $featureUid, $status, ($status & 1) ? 'supported' : 'not supported';
	if (($e > 0) && ($featureUid <= $lastFeatureUid))
		{
		print "INVALID Feature Registry Config File: Feature entries not listed by increasing (and non-repeating) UID";
		exit 1;
		}
	if ($status & ~3)
		{
		print "WARNING Feature Registry Config File: Feature entry status bits 2-31 should be zero\n";
		}
	$lastFeatureUid = $featureUid;
	}

print "---\n";

# Check default supported ranges are in low-UID / high-UID pairs
# AND that the Symbian Essential default range is covered:
my $EssentialDefaultSupportedRangeLowUid = 271030278;
my $EssentialDefaultSupportedRangeHighUid = 271063045;
if (($EssentialDefaultSupportedRangeLowUid != 0x10279806) ||
	($EssentialDefaultSupportedRangeHighUid != 0x10281805))
	{
	print "ERROR Mismatch on Feature Registry Symbian essential default-supported range";
	exit 1;
	}
my $haveEssentialRange;
for (my $r = 0; $r < $rangeCount; $r++)
	{
	my $packedRange;
	if (!read($BINCONFIG, $packedRange, $entrySize) ||
		(length $packedRange < $rangeSize))
		{
		print "INVALID Feature Registry Config File: Unable to read default-supported range $r";
		exit 1;
		}
	my ($lowUid, $highUid) = unpack 'I2', $packedRange;
	printf "Feature UID range 0x%08X to 0x%08X supported by default\n", $lowUid, $highUid;
	if (($lowUid <= $EssentialDefaultSupportedRangeLowUid) &&
		($highUid >= $EssentialDefaultSupportedRangeHighUid))
		{
		$haveEssentialRange = 1;
		}
	if ($lowUid > $highUid)
		{
		print "INVALID Feature Registry Config File: Default-supported range is not listed in order Low-UID High-Uid";
		exit 1;
		}
	}

if (!$haveEssentialRange)
	{
	printf "INVALID Feature Registry Config File: Symbian essential default-supported UID range 0x%08X to 0x%08X is not present\n",
		$EssentialDefaultSupportedRangeLowUid, $EssentialDefaultSupportedRangeHighUid;
	exit 1;
	}

# Confirm there's nothing more to read
my $junk;
if (read $BINCONFIG, $junk, 1)
	{
	printf "INVALID Feature Registry Config File: Unexpected data at end of binary configuration file";
	exit 1;
	}

# Warn if the filename is not 'featreg.cfg' (case insensitive)
if ($binConfigFile !~ m/[Ff][Ee][Aa][Tt][Rr][Ee][Gg]\.[Cc][Ff][Gg]$/)
	{
	printf "WARNING Feature Registry Config File must be called 'featreg.cfg'";
	}

# Success - a valid configuration file (but it's up to you to guarantee the entries and ranges are correct,
# and to put the file in the correct location
exit 0;
