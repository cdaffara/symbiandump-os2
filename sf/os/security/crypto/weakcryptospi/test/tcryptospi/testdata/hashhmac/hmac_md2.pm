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

package Digest::HMAC_MD2;
$VERSION="1.01";

use strict;
use Digest::MD2  qw(md2);
use Digest::HMAC qw(hmac);

# OO interface
use vars qw(@ISA);
@ISA=qw(Digest::HMAC);
sub new
{
    my $class = shift;
    $class->SUPER::new($_[0], "Digest::MD2", 16);
}

# Functional interface
require Exporter;
*import = \&Exporter::import;
use vars qw(@EXPORT_OK);
@EXPORT_OK=qw(hmac_md2 hmac_md2_hex);

sub hmac_md2
{
    hmac($_[0], $_[1], \&md2, 16);
}

sub hmac_md2_hex
{
    unpack("H*", &hmac_md2)
}

1;

__END__

=head1 NAME

Digest::HMAC_MD2 - Keyed-Hashing for Message Authentication

=head1 SYNOPSIS

 # Functional style
 use Digest::HMAC_MD2 qw(hmac_md2 hmac_md2_hex);
 $digest = hmac_md2($data, $key);
 print hmac_md2_hex($data, $key);

 # OO style
 use Digest::HMAC_MD2;
 $hmac = Digest::HMAC_MD2->new($key);

 $hmac->add($data);
 $hmac->addfile(*FILE);

 $digest = $hmac->digest;
 $digest = $hmac->hexdigest;
 $digest = $hmac->b64digest;

=head1 DESCRIPTION

This module provide HMAC-MD2 hashing.

=head1 SEE ALSO

L<Digest::HMAC>, L<Digest::MD2>, L<Digest::HMAC_SHA1>

=head1 AUTHOR

Jon Thackray, from Gisle Aas <gisle@aas.no>'s MD5 code

=cut
