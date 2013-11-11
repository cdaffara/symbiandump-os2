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
# This simple script shows how simple it is to create a feature set data file
# using the 'FMCreate', 'FeatureFlag' and 'FeatureDSR' classes.
# 
#

use strict;
use fmcreate;
use featureflag;
use featuredsr;

#
# First create a 'FMCreate' object. Note that the typefield defaults to "feat",
# the fileversion defaults to '1', fileflags to '0'. Initially it contains no
# feature flags or default supported ranges.
#
my $fmc = FMCreate->new();
die "Couldn't create an 'FMCreate' object" unless(ref($fmc));

#
# Now we can do all sorts of things with this object - for example populating
# it from an existing file (using the LoadUp method). But let's populate it
# manually..
#

#
# Create a feature flag object. The arguments are UID, status flag (defaults
# to zero) and user data word. 
#
my $ff = FeatureFlag->new(0xabcd1234, undef, 0xf00df00d);
die "Couldn't create a 'FeatureFlag' object" unless(ref($ff));

#
# Change some bits in the status flags.
#
$ff->Modifiable(1);
$ff->BlackListed(0);
$ff->Uninitialized(1);

# Hey, I've decided I don't like the user data word (0xf00df00d) we defined
# when we created $ff, so lets change it..
$ff->UserData(0x12344331);

# We need to add the feature flag to the 'FMCreate' object - they aren't tied
# in any way yet. 
$fmc->AddFeatureFlag($ff);

# Create another FeatureFlag. We can't re-use the '$ff' object (without doing
# another 'new') because '$fmc' owns it now and holds a reference to it.
# This line creates a totally separate FeatureFlag object which we can set up
# in any way we like - but we must remember to add it to '$fmc'.

$ff = FeatureFlag->new(0xfedcba98);
$fmc->AddFeatureFlag($ff);

# Modify $ff - this is already in '$fmc' but that's ok - perhaps we want to
# modify one that's already in.
$ff->UserData(0x42);
$ff->BlackListed(1);



# Ok now create a couple of 'default supported range' objects. Again we
# have to add these to the '$fmc' FMCreate object.
my $dsr1 = FeatureDSR->new(0x10000000, 0x10000005);
my $dsr2 = FeatureDSR->new(0x10000050, 0x10000055);
die "Failed to create 'FeatureDSR' object"  unless(ref($dsr1) and ref($dsr2));

# Add them in the wrong order to '$fmc'. Note that 'FMCreate' does not check
# for duplicate default supported range objects (or FeatureFlag objects for
# that matter).
$fmc->AddFeatureDSR($dsr2);
$fmc->AddFeatureDSR($dsr1);


# OK, the FMCreate object '$fmc' is fairly well populated now. Lets display
# the content.. There also are methods to just show the header information, the
# FeatureFlag information and FeatureDSR information.
$fmc->ShowALL();


# Let's write the features to a binary file..
print "\n\n\nWriting feature set data to 'featureset.dat'\n";
$fmc->WriteToFile("featureset.dat")
	   	or die "Couldn't write to feature set data file 'featureset.dat'\n";
