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

#!perl
use strict;
use warnings;
use File::Copy;

## THIS SCRIPT NEEDS TO BE RUN FROM \epoc32\rom\tools ONLY

## Perform cleanup first
rmdir("rom_images");
mkdir("rom_images");

## Number of ROFS to build specify number max 7
my $no_rofs = $ARGV[0];

## Udeb/Urel configuration	1 means udeb
my $udeb =$ARGV[1];

## Textshell/Techview configuration 1 means textshell
my $shell=$ARGV[2];

## Last argument check whether txt or cre test mode
my $txt=$ARGV[3];

my $buildrom_com="-D_NAND2 h4hrp";

## Append the full debug option
if ($udeb==1)
	{
	$buildrom_com.=" -D_FULL_DEBUG";
	}
if ($txt==1)
	{
	$buildrom_com.=" -DPDS_PERFTEST_TXT";
	}

## Append whether textshell or techview and also add the centrep patchable constant
if ($shell==1)
	{
	$buildrom_com.=" syslibs_textshell.oby syslibs_utils centreptesthelper.iby";
	my $patchconst=" t_centrepPatchConst".$no_rofs.".iby";
	$buildrom_com.=$patchconst;
	}
else
	{
#	$buildrom_com.=" techview centrepPatchConst.iby centreptesthelper.iby"; 
	$buildrom_com.=" techview centreptesthelper.iby"; 
	}

if ($txt==1)
	{
	$buildrom_com.=" t_centrepPerfPatchConstTxt.iby";
	}
else
	{
	$buildrom_com.=" t_centrepPerfPatchConstCre.iby";
	}

## Make a copy of the estart for backup
unlink("../h4hrp/unistore2/estartnandcomp.txt_bak");
copy("../h4hrp/unistore2/estartnandcomp.txt","../h4hrp/unistore2/estartnandcomp.txt_bak") or die("Cannot backup");

## Need to copy the corresponding estart to the right location
my $estartfile_name="estartnandcomp".$no_rofs.".txt";
copy ($estartfile_name,"../h4hrp/unistore2/estartnandcomp.txt") or die("Cannot copy estart");


## Now call the buildrom
my $command="buildrom.cmd ".$buildrom_com;
print $command;

## build the core and ROFS1.img
if ($no_rofs>=1)
	{
	$command.=" PREQ2112_ROFS1.iby";
	}
system($command);
## rename the image files

if ($shell==1)
	{
	copy("h4hrp_001.textshell_rom.nand.IMG","./rom_images/core.img") or die("Cannot copy core.img");
	copy("h4hrp_001.textshell_rom.nand.rofs.img","./rom_images/rofs1.img") or die("Cannot copy rofs1.img");
	}
else
	{
	copy("h4hrp_001.techview.nand.IMG","./rom_images/core.img") or die("Cannot copy");
	copy("h4hrp_001.techview.nand.rofs.img","./rom_images/rofs1.img") or die("Cannot copy");
	}

## Now build the rest ROFS images
for (my $i=2;$i<=$no_rofs;$i++)
	{
	my $rofs_comm="../../tools/rofsbuild.exe ../include/PREQ2112_ROFS".$i.".iby";
	system($rofs_comm);
	copy("rofs".$i.".img","rom_images/rofs".$i.".img") or die("Cannot copy rofs2 etc");
	}

## Restore the estart we have overriden
copy("../h4hrp/unistore2/estartnandcomp.txt_bak","../h4hrp/unistore2/estartnandcomp.txt");