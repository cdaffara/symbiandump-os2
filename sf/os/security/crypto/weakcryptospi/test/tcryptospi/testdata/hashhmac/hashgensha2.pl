#
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

my $numArgs = $#ARGV + 1;

my $file = @ARGV[0];
my $origkey = @ARGV[1];
my $key = $origkey;
print "key is: $origkey\n";
print "File is: $file\n";


my $outfile = $file;
substr($outfile,length($file)-4,length($file),"_SHA2HASHDATA.txt");
print $outfile;
print "\n";

open(MARKER, ">$outfile");
print MARKER "****** HASH SHA224/SHA256/SHA384/SHA512 ******\n\n";
close (MARKER);

my $failed = system("openssl dgst -sha224 $file >> $outfile");
if ($failed) { print "openssl failed because $!"};

my $failed = system("openssl dgst -sha256 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};

my $failed = system("openssl dgst -sha384 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};

my $failed = system("openssl dgst -sha512 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};

open(MARKER, ">>$outfile");
print MARKER "\n\n****** HMAC SHA224/SHA256/SHA384/SHA512 ******\n\n";
close (MARKER);

my $failed = system("openssl dgst -hmac CryptoSpiValidKey -sha224 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};

my $failed = system("openssl dgst -hmac CryptoSpiValidKey -sha256 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};

my $failed = system("openssl dgst -hmac CryptoSpiValidKey -sha384 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};

my $failed = system("openssl dgst -hmac CryptoSpiValidKey -sha512 $file >> $outfile");
if ($failed) { print " openssl failed because $!"};
