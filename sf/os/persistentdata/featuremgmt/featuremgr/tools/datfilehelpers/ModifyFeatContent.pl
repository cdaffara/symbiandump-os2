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
# This simple script shows how to modify the content of a feature manager
# data file.
# 
#

use strict;
use FMCreate;

#
# Hardwire the datafile - this is only an example.
# 
my $datfile = "features.dat";
my $datfile2 = "features2.dat";
my $datfile3 = "features3.dat";

#
# Create an object that represents a feature data file.
#
my $fmc = FMCreate->new();

#
# Load the content of the data file into our FMCreate object.
# Note that this will die if the content does not seem to be a feature set
# file. This can happen if the first four bytes aren't 'feat' or if reading
# the file fails at any point. This will also happen if the file is the wrong
# size.
#
$fmc->LoadUp($datfile) or die "Failed to load up data from '$datfile'\n";

#
# Get feature flag '0x10000009'
#
my $ffuid = 0x10000009;
my $ff = $fmc->GetFeatureFlagByUID($ffuid);
if(ref($ff) ne "FeatureFlag")
{
	printf "Couldn't get feature flag uid 0x%0x from file $datfile\n";
	die "no feature flag";
}

#
# Change the feature flag.. Note that the feature flag object '$ff' is a
# reference to the feature flag in '$fmc' - it's still in there, so when
# we modify it here it will affect what '$fmc' will write.
#
$ff->BlackListed(1);
$fmc->WriteToFile($datfile2) or die "Couldn't write feature data file '$datfile2'\n";

#
# Now add a new feature. The three arguments are UID, status flags (not defined
# here) and user data word.
#
my $newfeat = FeatureFlag->new(0x12345678, undef, 0xabcd);
die "Couldn't create new feature flag object.\n" unless(ref($newfeat));

#
# Add it to our existing feature data.
#
$fmc->AddFeatureFlag($newfeat) or die "Couldn't add new feature flag..\n";

#
# Remove the original feature flag.
#
$fmc->RemoveFeatureFlagByUID($ffuid) or die "Couldn't remove feature flag\n";

$fmc->WriteToFile($datfile3);
