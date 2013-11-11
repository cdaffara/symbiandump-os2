#! perl

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
# TSecurityPolicy.pl converts a string in the form of
# sid_wr=100 cap_wr=WriteDeviceData,NetworkControl to package format
# NB. This script treats cap_rd the same as cap_wr, i.e. it does not
# care if the policy is for read or for write.
# 
#


# AccessPolicy has many forms #####
# NB: ETypeC3, ETypeV3 not used by centrep
my $ETypeFail = "\0";
my $ETypePass = "\x1";
my $ETypeC7 = "\x3";
my $ETypeS3 = "\x4";
my $NoCapability = "\xff";


sub ToNum
  {
  my $s = shift;
  return $s if ($s =~ /^\d+$/);

  return hex($s) if ($s =~ /^0x[\da-fA-F]+$/ || $s =~ /^[\da-fA-F]+$/);

  return $s; # give up
  }

sub DisplayPolicy
{
  my $policy = shift;
  $len = length($policy);
  die "policy str len $len\n" if ($len != 8);

  my @bytes = unpack("C8", $policy);
  for (my $i = 0; $i < 8; $i++)
  {
	  printf("%02X", @bytes[$i]);
  }
  print "\n";
}

#
# return pack'ed data
sub parseCapability
  {
  my $s = shift;
  my @caplist = (
  "TCB",
  "CommDD",
  "PowerMgmt",
  "MultimediaDD",
  "ReadDeviceData",
  "WriteDeviceData",
  "DRM",
  "TrustedUI",
  "ProtServ",
  "DiskAdmin",
  "NetworkControl",
  "AllFiles",
  "SwEvent",
  "NetworkServices",
  "LocalServices",
  "ReadUserData",
  "WriteUserData",
  "Location",
  "SurroundingsDD",
  "UserEnvironment"
                );
  for (my $i = 0; $i < @caplist; $i++)
    {
    return pack("C", $i) if ($caplist[$i] =~ /^$s$/i);
    }

  return $NoCapability;
  } # sub parseCapability

#
# sid and up to 3 capabilities
#
sub doSidPlatsec
  {
  (my $sid, my $capstr) = @_;

  my @caps = split(/,/, $capstr);
  die("SID can only have 3 capabilities") if ($#caps > 2);

  my $policy = $ETypeS3; # \04

  for (my $i = 0; $i < 3; $i++)
    {
    my $binaryCap = $NoCapability; # initialize to "\xFF"
    if ($i <= $#caps)
      {
      $binaryCap = parseCapability($caps[$i]);
#      die("Bad capability $caps[$i]") if ($binaryCap eq $NoCapability);
      }
    $policy .= $binaryCap;
    }

  $policy .= pack("I", ToNum($sid));
  DisplayPolicy($policy);
  } # sub doSidPlatsec

#
# Up to 7 capabilities
# Return: pack'ed (i.e. binary) data
#
sub doCapabilityPlatsec
  {
  my $capstr = shift;
  my @caps = split(/,/, $capstr);

  die("SID can only have 7 capabilities") if ($#caps > 6);

  my $policy = $ETypeC7; # char \x3

  if ($caps[0] =~ /^AlwaysPass$/i)
  {
    # AlwaysPass is not a capability. It is a policy type.
    $policy = $ETypePass; # char \x1
    @caps = ();
  }
  elsif ($caps[0] =~ /^AlwaysFail$/i)
  {
    # AlwaysFail is not a capability. It is a policy type.
    $policy = $ETypeFail; # char \x0
    @caps = ();
  }

  for (my $i = 0; $i < 7; $i++)
    {
    my $binaryCap = $NoCapability; # initialize to "\xFF"
    if ($i <= $#caps)
      {
      $binaryCap = parseCapability($caps[$i]);
#      die("Bad capability $caps[$i]") if ($binaryCap eq $NoCapability);
      }
    $policy .= $binaryCap;
    }

  DisplayPolicy($policy);
  } # sub doCapabilityPlatsec


#########################
# main
#########################
my $s = shift;
my $unquotedSecondArg = shift;

my $Policy = "";
my $sid = undef;

# in case user forgot to put quote around the space.
if ($s =~ /^sid_/ and defined($unquotedSecondArg))
{
	$s = $s . " " . $unquotedSecondArg;
}

my @a = split(/\s+/, $s);

for (my $i = 0; $i < @a; $i++)
{
if ($a[$i] =~ /^sid_rd=/ or $a[$i] =~ /^sid_wr=/)
  {
  $sid = substr($a[$i], 7);
  }
elsif ($a[$i] =~ /^cap_rd=/ or $a[$i] =~ /^cap_wr=/)
  {
  $Policy = substr($a[$i], 7);
  }
} # for loop

# Start generating the output
if (defined($sid))
  {
  doSidPlatsec($sid, $Policy);
  }
elsif ($Policy)
  {
  doCapabilityPlatsec($Policy);
  }
else
  {
  die ("Fail to parse %s\n");
  }

