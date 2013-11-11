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

use strict;
use Getopt::Long;
my %opt;


# HASH MD2/MD4/MD5/SHA1 Digest Generator

#----------Define Function Headers----------

#Hash Functions
use Digest::MD2  qw(md2 md2_hex md2_base64);		# Copy-right 1998-2001 Gisle Aas.
use Digest::MD4;					# Mike McCauley/Neil Winton/RSA Data Security
use Digest::MD5  qw(md5 md5_hex md5_base64);		# Copy-right 1998-2002 Gisle Aas.
use Digest::SHA1 qw(sha1 sha1_hex sha1_base64);		# Copy-right 1999-2001 Gisle Aas.

#HMAC Functions
use Digest::HMAC_MD2 qw(hmac_md2 hmac_md2_hex);
use Digest::HMAC_MD4;
use Digest::HMAC_MD5 qw(hmac_md5 hmac_md5_hex);
use Digest::HMAC_SHA1 qw(hmac_sha1 hmac_sha1_hex);

#-------------- MAIN SCRIPT -----------------

my $convfromhex = '';
GetOptions ('convfromhex' => \$convfromhex );
my $numArgs = $#ARGV + 1;

my $file = @ARGV[0];
my $origkey = @ARGV[1];
my $key = $origkey;
print "key is: $origkey\n";
if ( $convfromhex ) {
  $key = pack("H*", $origkey);
  print "Converting key from hex\n";
}

#print("Data to be Hashed: \n");

my $data = "";
open(DATA, "< $file" ) or die "Can't open $file : $!";
binmode(DATA);

    while ( <DATA> ) {
	$data.= $_;
    }
close(DATA);

#print($data);
#print("\n");

#----------Return Hashed Values----------

my $output = $file;

substr($output,length($file)-4,length($file),"_HASHDATA.txt");

open(OUT, "> $output") or die "Can't open $output : $!";

print OUT "****** HASH MD2/MD4/MD5/SHA1 ******\n";
print OUT "\n";

print OUT "MD2: ";
print OUT md2_hex($data);
print OUT "\n";

print OUT "MD4: ";
print OUT Digest::MD4->hexhash($data);
print OUT "\n";

print OUT "MD5: ";
print OUT md5_hex($data);
print OUT "\n";

print OUT "SHA1: ";
print OUT sha1_hex($data);
print OUT "\n";

print OUT "\n";
print OUT "****** HMAC MD2/MD4/MD5/SHA1 ******\n";
print OUT "\n";

print OUT "HMAC-MD2: ";
print OUT hmac_md2_hex($data, $key);
print OUT "\n";

my $hmac = Digest::HMAC_MD4->new($key);
$hmac->add($data);

print OUT "HMAC-MD4: ";
print OUT $hmac->hexdigest;
print OUT "\n";

print OUT "HMAC-MD5: ";
print OUT hmac_md5_hex($data, $key);
print OUT "\n";

print OUT "HMAC-SHA1: ";
print OUT hmac_sha1_hex($data, $key);
print OUT "\n";

print OUT "\n";

print OUT "Key: ";
print OUT $origkey;
print OUT "\n";

close(OUT);
