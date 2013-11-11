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
# A simple class to manage feature flags for a feature set data file.
#
package FeatureFlag;

# Create a feature flag object.
sub new
{
	my $arg = shift;
	my $class = ref($arg) || $arg;
	my($uid, $sf, $ud) = @_;
	die "You must specify a UID when creating a FeatureFlag object"
	    unless(defined($uid));
	$sf = 0 unless(defined($sf));
	$ud = 0 unless(defined($ud));

	my $self = {
			     uid => $uid,
			     statusflags => $sf,
			     userdata => $ud,
				 endian => "LE",
			   };
	bless $self, $class;
	return $self;
}

sub Endian
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return $self->{endian} unless(defined($arg) and $arg =~ m/(^BE$|^LE$)/i);
	$self->{endian} = lc($1);
	return $self->{endian};
}

# Return a twelve byte string 'feature flag' information.
sub BinaryContent
{
	my $self = shift;
	return undef unless(ref($self));
	my @arr = ( $self->UID(), $self->StatusFlags(), $self->UserData() );

	# Decide whether we want big or little endian output.
	# According to the documentation, 'V', 'N' are GUARANTEED to be 32-bit.
	my $packstring = "V3"; # Little endian.
	   $packstring = "N3" if($self->Endian() eq "BE");

	my $string = pack $packstring, @arr;
	return $string;
}

# A single 32-bit number.
sub UID
{
	my $self = shift;
	return undef unless(ref($self));
	my $uid = shift;
	return $self->{uid} unless(defined($uid));
	$uid = int($uid);
	$self->{uid} = $uid;
	return $uid;
}

# A single 32-bit number.
sub StatusFlags
{
	my $self = shift;
	return undef unless(ref($self));
	my $sf = shift;
	return $self->{statusflags} unless(defined($sf));
	$sf = int($sf);
	$self->{statusflags} = $sf;
	return $sf;
}

# A single 32-bit number.
sub UserData
{
	my $self = shift;
	return undef unless(ref($self));
	my $ud = shift;
	return $self->{userdata} unless(defined($ud));
	$ud = int($ud);
	$self->{userdata} = $ud;
	return $ud;
}

# Display the content of the feature flag in english.
sub Show
{
	my $self = shift;
	return undef unless(ref($self));
	my $fd = shift;
	$fd = *STDOUT unless(defined($fd));
	printf $fd "UID 0x%08x\n", $self->UID();
	printf $fd "Status Flags 0x%08x\n", $self->StatusFlags();

	# Supported?
	print "\t";
	print "Not " unless($self->Supported);
	print "Supported\n";

	# Upgradable?
	print "\t";
	print "Not " unless($self->Upgradable);
	print "Upgradable\n";

	# Modifiable?
	print "\t";
	print "Not " unless($self->Modifiable);
	print "Modifiable\n";
	                              
	# BlackListed?
	print "\t";
	print "Not " unless($self->BlackListed);
	print "BlackListed\n";

	# Uninitialised?
	print "\t";
	print "Not " unless($self->Uninitialised); # Double negative.
	print "Uninitialised\n";

	# Persisted?
	print "\t";
	print "Not " unless($self->Persisted);
	print "Persisted\n";

	printf $fd "User Data 0x%08x\n\n", $self->UserData();
	return 1;
}

1;
# ###########################################################################

# The following methods operate on the 'StatusFlags' member, just setting
# or clearing bits as required.
#
# Bits 6 through 31 are currently reserved, 23/7/07.
#

sub Supported
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
   	my $sp = $self->StatusFlags;
	if(defined($arg))
	{
	    $arg = 0 if( $arg =~ m/EXCLUDE/i );
    	$arg = 1 if( $arg =~ m/FEATURE/i );
    	if($arg) { $sp |= 1; } else { $sp &= ~1; };
    	$self->StatusFlags($sp);
		return $arg;
	}
	else
	{
		return ($sp & 1);
	}
}

sub Upgradable
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
    my $sp = $self->StatusFlags;
	if(defined($arg))
	{
    	if($arg) { $sp |= 2; } else { $sp &= ~2; };
    	$self->StatusFlags($sp);
		return $arg;
	}
	else
	{
		return ($sp & 2);
	}
}

sub Modifiable
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
    my $sp = $self->StatusFlags;
	if(defined($arg))
	{
    	if($arg) { $sp |= 4; } else { $sp &= ~4; };
    	$self->StatusFlags($sp);
		return $arg;
	}
	else
	{
		return ($sp & 4);
	}
}

sub BlackListed
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
   	my $sp = $self->StatusFlags;
	if(defined($arg))
	{
    	if($arg) { $sp |= 8; } else { $sp &= ~8; };
    	$self->StatusFlags($sp);
		return($arg);
	}
	else
	{
		return ($sp & 8);
	}
}

sub Uninitialised
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
   	my $sp = $self->StatusFlags;
	if(defined($arg))
	{
    	if($arg) { $sp |= 16; } else { $sp &= ~16; };
    	$self->StatusFlags($sp);
		return($arg);
	}
	else
	{
		return ($sp & 16);
	}
}

sub Uninitialized
{
	return Uninitialised(@_);
}

sub Persisted
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
    my $sp = $self->StatusFlags;
	if(defined($arg))
	{
    	if($arg) { $sp |= 32; } else { $sp &= ~32; };
    	$self->StatusFlags($sp);
		return($arg);
	}
	else
	{
		return ($sp & 32);
	}
}


# ###########################################################################

1;

