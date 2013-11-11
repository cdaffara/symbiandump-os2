#!perl

# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use File::Copy;
use Getopt::Long;
use File::Basename;
use File::Spec;

my $basedir;
my $platform;
my $cfg;
my $certpem;
my $certkey;
my $outdir;
my $maketrgt;
my $sources;
my $stublist;

GetOptions(
  'basedir=s' => \$basedir,
  'platform=s' => \$platform,
  'cfg=s' => \$cfg,
  'certpem=s' => \$certpem,
  'certkey=s' => \$certkey,
  'outdir=s' => \$outdir,
  'maketrgt=s' => \$maketrgt,
  'sources=s' => \$sources,
  'stublist=s' => \$stublist,
);

my $ROOT = $ENV{EPOCROOT};
$ROOT =~ s#\\#/#g;

if ($maketrgt =~ /RELEASABLES/i or $maketrgt =~ /EXTRATARGET/i)
{
  my @targets = ();
  my @extra = ();
  my $s;
  my @a = split(/\s+/,  $sources);
  foreach my $pkg (@a)
  {
	my $isStub = $stublist =~ /\b$pkg\b/i;
	my $destdir = GetTargetDir($isStub);
	my $trgtname = PkgToSis($pkg, $isStub);
	$s =  "$destdir/$trgtname";
    GetExternalForm(\$s);
   	push(@targets, $s);

    if ($destdir =~ /epoc32\/data\/z\/.*\/udeb/ || $isStub)
	{
	  my $extratrgt = $destdir;
	  $extratrgt =~ s/\/udeb//;
	  $s = "$extratrgt/$trgtname";
      GetExternalForm(\$s);
	  push(@extra, $s);
	}
  }

  $s = join(' ', @targets);
  $s = join(' ', @extra) if ($maketrgt =~ /EXTRATARGET/i);
  print "$s";
  exit(0);
}

# Getting here is build final. The pkg file must be in basedir.
chdir($basedir);

# $ARGV[0] is one of the targets generated in RELEASABLES above.
my $arg = $ARGV[0];
my $filename = basename($arg);
my $trgtdir = dirname($arg);
RecursiveMkDir($trgtdir);

my $isStub;
($filename, $isStub) = SisToPkg($filename);
die "Pkg $filename does not exist!\n" if (! -f $filename);

my $cmd;
if ($isStub)
{
  $cmd = "makesis -s $filename $arg";
  system($cmd);
}
else
{
  # change the source locations in pkg file
  my $generatedpkg = "generated.pkg";
  EditPkgFile($filename, $generatedpkg);

  my $generatedsis = "generated.sis";
  $cmd = "makesis $generatedpkg $generatedsis";
  system($cmd);

  $cmd = "signsis -S $generatedsis $arg $certpem $certkey";
  system($cmd);

  # make an extra copy in /epoc32/data/z/tef_ecomswi.
  # iby files normally do not expect files in udeb or urel
  # under /epoc32/data/z.
  if ($trgtdir =~ /\bepoc32\b.\bdata\b.\bz\b.*\budeb\b/)
  {
	my $extratrgt = $arg;
	$extratrgt =~ s/[\/\\]udeb//;
	copy($arg, $extratrgt);
  }

  unlink($generatedsis);
  unlink($generatedpkg);
}

###################################################
# subroutines
###################################################
sub GetTargetDir
{
  my $isStub = shift;

  # for armv5, store all artefacts in data/z.
  my $d = "${ROOT}epoc32/data/z/$outdir/$cfg";
  $d =~ s#/$cfg## if ($isStub);

  if ($platform =~ /winscw/i)
  {
    $d = "${ROOT}epoc32/release/$platform/$cfg/z/$outdir";
    $d = "${ROOT}epoc32/release/$platform/$cfg/z/system/install" if ($isStub);
  }
  return $d;
}

sub PkgToSis
{
  my $f = shift;
  my $isStub = shift;
  $f =~ s/\.pkg$//i;
  my $suffix = ($isStub) ? ".sis" : "signed.sis";
  $f .= $suffix;
  return $f;
}

sub SisToPkg
{
  my $f = shift;
  my $isStub = 1;
  $isStub = 0 if ($f =~ s/signed.sis$//);
  $f =~ s/\.sis$//;
  $f .= ".pkg";
  return ($f, $isStub);
}

sub EditPkgFile
{
  my $inf = shift;
  my $outf = shift;

  open(FIN, $inf) or die "fail to read $inf $!\n";
  open(FOUT, ">$outf") or die "fail to write $outf $!\n";
  local $/ = undef;
  my $data = <FIN>;
  close(FIN);

  $data =~ s/_PLATFORM_/$platform/g;
  $data =~ s/_CFG_/$cfg/g;
  print FOUT $data;
  close(FOUT);
}

# simple one, cannot handle cases like \\londata04\somedir
sub RecursiveMkDir
{
  my $path = shift;
  $path =~ s#\\#/#g;

  my $i = 1;
  my $s;
  do {
	$i = index($path, "/", $i);
	if ($i != -1)
	{
	  $s = substr($path, 0, $i++);
	  mkdir($s, 0777) if (! -d $s);
	}
  } while ($i != -1);

  mkdir($path, 0777) if (! -d $path);
}

sub GetExternalForm
{
  my $refs = shift;
  my @a = split(/[\/\\]/, $$refs);
  $$refs = File::Spec->catfile(@a);
}
