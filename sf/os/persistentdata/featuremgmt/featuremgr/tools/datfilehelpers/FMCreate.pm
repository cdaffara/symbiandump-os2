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
# Perl module to create and maintain feature manager data files.
# You can either set up the information programmatically or else load up
# information from a pre-existing feature data file and then modify it. You
# can also save the information to a file (in feature manager dataset format).
#
# This class maintains header information plus two arrays, one containing
# feature flag information and the other containing default supported range
# information. Those are themselves objects and have their own accessor
# methods.
#

package FMCreate;

use featureflag;
use featuredsr;

#
# n e w
#
# Create a new FMCreate object. For example 'my $fmc = FMCreate->new();
#
sub new
{
	my $arg = shift;
	my $class = ref($arg) || $arg;
	my $self = {
			        typefield => "feat",  # 4 bytes wide.
					fileversion => 1,     # 2 bytes.
					fileflags => 0,       # 2 bytes.
					numfeatures => 0,     # 4 bytes. (this is 'x')
					numdefuid => 0,       # 4 bytes. (this is 'y')
					endian => "LE",
					packprefix => "V",    # Changed with endian-ness.
					                      # Used to create binary strings.

					featureflags => [],   # There are x of these.
					dsrs => [],           # There are y of these.
	           };
	bless $self, $class;
	return $self;
}

# Print to STDOUT the header information, feature flags information and
# default supported range information.
sub ShowALL
{
	my $self = shift;
	return undef unless(ref($self));
	$self->ShowHeader();
	$self->ShowFeatureFlags();
	$self->ShowDSRs();
	return 1;
}

# Print to STDOUT the header information we have.
sub ShowHeader
{
	my $self = shift;
	return undef unless(ref($self));

	# Get header information..
	my $typefield = $self->TypeField();
	my $fileversion = $self->FileVersion();
	my $fileflags = $self->FileFlags();
	my $numfeatures = $self->NumFeatures();
	my $numdefuid = $self->NumDefUid();

	# Display it in English.
	print "TYPEFIELD: '$typefield'\n";
	print "FILEVERSION: '$fileversion'\n";
	print "FILEFLAGS: '$fileflags'\n";
	print "NUMFEATURES: '$numfeatures'\n";
	print "NUMDSRS: '$numdefuid'\n";

	return(1);
}

# Call the 'Show' method in each of the feature flag objects we
# have - this will print their content to STDOUT.
sub ShowFeatureFlags
{
	my $self = shift;
	return undef unless(ref($self));
	my $ffs = $self->FeatureFlags;
	return 1 unless(@$ffs);

	print "\nFeature Flags\n";
	print "=============\n";
	for my $ff (@$ffs)
	{
		$ff->Show();
	}
	return 1;
}

# Call the 'Show' method in each of the default supported range objects we
# have - this will print their content to STDOUT.
sub ShowDSRs
{
	my $self = shift;
	return undef unless(ref($self));
	my $fds = $self->FeatureDSRs;
	return 1 unless(@$fds);
	print "\nFeature DSRs\n";
	print "============\n";
	for my $fd (@$fds)
	{
		$fd->Show();
	}
	return 1;
}

# Get/Set the endian-ness we want. Changes the 'packprefix' member which is
# used in the creation of binary data.
sub Endian
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return $self->{endian} unless(defined($arg));
	if($arg =~ m/(LE|BE)/i)
	{
		my $endian = uc($1);
		$self->{endian} = $endian;
		# Used by 'pack' to generate binary strings.
		$self->{packprefix} = "V" if($endian eq "LE");
		$self->{packprefix} = "N" if($endian eq "BE");
	}
	return $self->{endian};
}

# This is 'feat'.
sub TypeField
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	$self->{typefield} = $arg if(defined($arg));
	return $self->{typefield};
}

sub FileVersion
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	# Should we be testing for a numeric value?
	$self->{fileversion} = $arg if(defined($arg));
	return $self->{fileversion};
}

sub FileFlags
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	$self->{fileflags} = $arg if(defined($arg));
	return $self->{fileflags};
}

# How many feature flag objects have we got?
sub NumFeatures
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	$self->{numfeatures} = $arg if(defined($arg));
	return $self->{numfeatures};
}

# How many default supported range objects have we got?
sub NumDefUid
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	$self->{numdefuid} = $arg if(defined($arg));
	return $self->{numdefuid};
}

# Create a binary string containing the header information for the
# feature manager data file based on the various fields in this object.
sub CreateBinaryHeader
{
	my $self = shift;
	return undef unless(ref($self));
	my $hdrstring;

	# Get the letter for packing information with 'pack' into a binary form.
	my $pack16 = lc($self->{packprefix});
	my $pack32 = uc($self->{packprefix});
	
	# Get header information..
	my $typefield = $self->TypeField();
	my $fileversion = $self->FileVersion();
	my $fileflags = $self->FileFlags();
	my $numfeatures = $self->NumFeatures();
	my $numdefuid = $self->NumDefUid();

	# Write the 'type' field out. This is 'feat'. Would this be different on
	# big-endian systems?
	$hdrstring = $typefield;

	# Now the file version number. A 16-bit value.. Will this cause trouble
	# if the shifted value is signed?
	$hdrstring .= pack($pack16 . "1", $fileversion);

	# Now the file flags. Another 16-bit value..
	$hdrstring .= pack($pack16 . "1", $fileflags);

	# Now the number of listed features - a 32-bit value.
	$hdrstring .= pack($pack32 . "1", $numfeatures);

	# Now the number of listed features - a 32-bit value.
	$hdrstring .= pack($pack32 . "1", $numdefuid);

	return $hdrstring;
}

# Writes the binary file specified as an argument with the content of this
# and contained feature flag and dsr objects.
sub WriteToFile
{
	my $self = shift;
	return undef unless(ref($self));
	my $file = shift;
	return undef unless(defined($file));
    open FILE, "> $file" or die "Couldn't open file '$file' for writing.\n";
	binmode FILE;
	print FILE $self->BinaryContent();
	close FILE;
	return 1;
}


# Create the binary equivalent of the internal data and return it as a
# string.
sub BinaryContent
{
	my $self = shift;
	return undef unless(ref($self));

	# First get the header information for the registry manager data file.
	my $ret = $self->CreateBinaryHeader();

    # Get the feature flag entries.. This is an array reference.
	# For each one append the binary representation of the information
	# contained.
	my $ffs = $self->FeatureFlags;
	for my $ff (@$ffs)
	{
		$ret .= $ff->BinaryContent();
	}

    # Get the feature default supported range entries.. This is an array
	# reference too. For each one append the binary representation of 
	# uid range contained.
	my $fdsrs = $self->FeatureDSRs;
	for my $ff (@$fdsrs)
	{
		$ret .= $ff->BinaryContent();
	}
	return $ret;
}

# Return a reference to the 'feature flags' array.
sub FeatureFlags
{
	my $self = shift;
	return undef unless(ref($self));
	return $self->{featureflags};
}

# Add a Feature Flag object. Perhaps there should be code to check if we
# already know about this feature flag. (i.e check the uid against the ones
# we have).
sub AddFeatureFlag
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	die "Method 'AddFeatureFlag' requires a 'FeatureFlag' object as argument.\n"
   	    unless(ref($arg) eq  "FeatureFlag");
	push @{$self->FeatureFlags()}, $arg;
	$self->NumFeatures($self->NumFeatures() + 1);
    return 1;
}

# Return a reference to the 'feature dsrs' array.
sub FeatureDSRs
{
	my $self = shift;
	return undef unless(ref($self));
	return $self->{dsrs};
}

# Add a Feature 'Default Support Range' object.
sub AddFeatureDSR
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	die "Method 'AddFeatureDSR' requires a 'FeatureDSR' object as argument.\n"
   	    unless(ref($arg) eq  "FeatureDSR");
	push @{$self->FeatureDSRs()}, $arg;
	$self->NumDefUid($self->NumDefUid() + 1);
    return 1;
}

# This method loads up it's information from an existing feature manager
# data file. This will die if it thinks there is something wrong with the file.
sub LoadUp
{
	my $self = shift;
	return undef unless(ref($self) eq "FMCreate");

	my $packprefix16 = lc($self->{packprefix});
	my $packprefix32 = uc($self->{packprefix});

	my $file = shift;
	return undef unless(defined($file) and -f $file);
	open FILE, $file or die "Couldn't open '$file'\n";
	binmode FILE;
	my ($tmp, $feat);

	# First get the file size.
	my $fsz = sysseek(FILE, 0, 2);
	sysseek(FILE, 0, 0);

	# Read the 'feat' marker from the top of the file. Check it.
	die "Unable to read first 4 bytes from '$file'"
	    unless(4 == sysread(FILE, $feat, 4) );
	die "First four bytes of '$file' do not contain 'feat'"
	    unless($feat eq "feat");
	$self->TypeField($feat); # Pointless. It's set to that anyway.

	# Read the file version number.
	die "Unable to read two bytes from index 4 from '$file'"
	    unless(2 == sysread(FILE, $tmp, 2) );
	my $filever = unpack( $packprefix16, $tmp );
	$self->FileVersion($filever);

	# Read the file flags.
	die "Unable to read two bytes from index 6 from '$file'"
	    unless(2 == sysread(FILE, $tmp, 2) );
	my $fileflags = unpack( $packprefix16, $tmp );
	$self->FileFlags($fileflags);

	# Read the number of features. Don't do anything with this yet..
	die "Unable to read four bytes from index 8 from '$file'"
	    unless(4 == sysread(FILE, $tmp, 4) );
	my $nfeat = unpack( $packprefix32, $tmp );

	# Read the number of DSRs. Don't do anything with this yet..
	die "Unable to read four bytes from index 12 from '$file'"
	    unless(4 == sysread(FILE, $tmp, 4) );
	my $ndsr = unpack( $packprefix32, $tmp );

	# Forget it if the filesize is clearly wrong.
	my $expsz = 16 + 12*$nfeat + 8*$ndsr;
	die "The file '$file' is $fsz bytes long, but the content suggests it should be $expsz bytes long. NFeatures: $nfeat   NDSRs: $ndsr\n" unless($expsz == $fsz);

	# Now read in the feature flags.
	my $offset = 16;
	for(my $ff=0 ; $ff<$nfeat ; $ff++)
	{
		# Get the UID.
	    die "Unable to read four bytes (uid) from index $offset from '$file'"
	        unless(4 == sysread(FILE, $tmp, 4) );
		$offset += 4;
		my $uid = unpack( $packprefix32, $tmp );

		# Get the status word.
	    die "Unable to read four bytes (sw) from index $offset from '$file'"
	        unless(4 == sysread(FILE, $tmp, 4) );
		$offset += 4;
		my $sw = unpack( $packprefix32, $tmp );

		# Get the user data word.
	    die "Unable to read four bytes (udw) from index $offset from '$file'"
	        unless(4 == sysread(FILE, $tmp, 4) );
		$offset += 4;
		my $ud = unpack( $packprefix32, $tmp );

		my $featflag = FeatureFlag->new($uid, $sw, $ud);
		die "Couldn't create a feature flag object!\n" unless(ref($featflag));
		$self->AddFeatureFlag($featflag);
	}

	# Now read in the DSRs.
	for( my $dsr=0 ; $dsr<$ndsr ; $dsr++ )
	{
		# Get the low UID.
	    die "Unable to read four bytes (lowuid) from index $offset from '$file'"
	        unless(4 == sysread(FILE, $tmp, 4) );
		$offset += 4;
		my $lowuid = unpack( $packprefix32, $tmp );

		# Get the high UID.
	    die "Can't read four bytes (high uid) from index $offset from '$file'"
	        unless(4 == sysread(FILE, $tmp, 4) );
		$offset += 4;
		my $highuid = unpack( $packprefix32, $tmp );

		my $fd = FeatureDSR->new($lowuid, $highuid);
		die "Couldn't create 'FeatureDSR' object!\n" unless(ref($fd));
		$self->AddFeatureDSR($fd);
	}

	# Check if our calculated file offset matches the end of the file.
	# This is pointless actually, because we've already checked the file
	# size..
	my $fileoffset = sysseek(FILE, 0, 2);
	die "End of file offset ($fileoffset) does not match end of DSRs!\n"
	    unless($fileoffset == $offset);

	close FILE;
	return 1;
}

# Remove the feature flag object specified by UID held in this object (in
# the internal 'featureflags' array). Returns 1 on success, undef otherwise.
sub RemoveFeatureFlagByUID
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return undef unless(defined($arg));
	my $ffs = $self->FeatureFlags;

	my $idx = 0;
	for my $ff (@$ffs)
	{
		if($ff->UID() == $arg)
		{
			splice(@$ffs, $idx, 1);
			$self->NumFeatures($self->NumFeatures() - 1);
			return 1;
		}
		$idx++;
	}
	return undef;
}

# Return a reference to the 'FeatureFlag' object held in this object (in
# the internal 'featureflags' array) with the uid specified as an
# argument. This returns a reference so it's still in this object on return,
# you can modify it and then write out (for example) the data file.
sub GetFeatureFlagByUID
{
	my $self = shift;
	return undef unless(ref($self));
	my $arg = shift;
	return undef unless(defined($arg));
	my $ffs = $self->FeatureFlags;
	for my $ff (@$ffs)
	{
		return $ff if($ff->UID() == $arg);
	}
	return undef;
}


# Remove the feature DSR object specified by UIDs held in this object (in
# the internal 'dsrs' array). Returns 1 on success, undef otherwise.
sub RemoveDSRByUIDs
{
	my $self = shift;
	return undef unless(ref($self));
	my($lowuid, $highuid) = @_;
	return undef unless(defined($lowuid) and defined($highuid));
	my $fdsrs = $self->FeatureDSRs;

	my $idx = 0;
	for my $fdsr (@$fdsrs)
	{
		if( ($fdsrs->LowUID() == $lowuid) and
            ($fdsrs->HighUID() == $highuid) )
		{
			splice(@$fdsrs, $idx, 1);
			$self->NumDefUid($self->NumDefUid() - 1);
			return 1;
		}
		$idx++;
	}
	return undef;
}

# Return a reference to the 'FeatureDSR' object held in this object (in
# the internal 'dsrs' array) with the low and high uids specified in the
# arguments. This returns a reference so it's still in this object on return,
# you can modify it (by changing the uids) and then write out (for example)
# the data file.
sub GetDSRByUIDs
{
	my $self = shift;
	return undef unless(ref($self));
	my($lowuid, $highuid) = @_;
	return undef unless(defined($lowuid) and defined($highuid));
	my $fdsrs = $self->FeatureDSRs;
	for my $fdsr (@$fdsrs)
	{
		return $fdsr if( ($fdsr->LowUID() == $lowuid) and
                         ($fdsr->HighUID() == $highuid));
	}
	return undef;
}

1;

