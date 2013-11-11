#!perl -w
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#
# A simple class to maintain a single 'default supported range' feature
# object.
#
package FeatureDSR;

sub new
{
	my $arg = shift;
	my $class = ref($arg) || $arg;
	my($low, $high) = @_;
	my $self = {
			       lowuid => $low,
			       highuid => $high,
				   endian => "LE",
			   };
	bless $self, $class;
	return $self;
}

sub LowUID
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return $self->{lowuid} unless(defined($arg));
	$self->{lowuid} = $arg;
	return $arg;
}

sub HighUID
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return $self->{highuid} unless(defined($arg));
	$self->{highuid} = $arg;
	return $arg;
}

sub Endian
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return $self->{endian} unless($arg =~ m/(LE|BE)i/);
	$arg = uc($arg);
	$self->{endian} = $arg;
	return $arg;
}

# Return the content of this object as packed binary, i.e 8 bytes
# of the specified endian-ness.
sub BinaryContent
{
	my $self = shift;
	return undef unless(ref($self));
	my $packstring = "V2";
	   $packstring = "N2" if($self->{endian} eq "BE");
	my $retstring = pack($packstring, ( $self->LowUID, $self->HighUID ) );
	return $retstring;
}

# Display the content of the DSR in english.
sub Show
{
	my $self = shift;
	return undef unless(ref($self));
	my $fd = shift;
	$fd = *STDOUT unless(defined($fd));
	printf $fd "LOW UID 0x%08x\n", $self->LowUID();
	printf $fd "HIGH UID 0x%08x\n", $self->HighUID();
	return 1;
}

1;
