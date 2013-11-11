#!perl

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

system("getsource -o base_e32");
CreateAutoExeFlash();	
system("rename \\epoc32\\rom\\nandloader\\autoexec.bat autoexec_orig.bat")
	&& print "rename of autoexec.bat failed\n";
system("copy /y \\epoc32\\rom\\nandloader\\autoexec_flash.bat \\epoc32\\rom\\nandloader\\autoexec.bat")
	&& die "copy of autoexec_flash.bat failed\n";
chdir("\\src\\cedar\\generic\\base\\e32\\rombuild");

system("rom -v=h4hrp -i=armv5 -b=urel -t=nandloader -m=_NAND2")
	&& die "building h4 nandloader rom failed\n";
system("copy H4HRP__NAND2ARMV5.IMG \\syslibs_multiimageloader.img")
	&& die "copy of H4HRP__NAND2ARMV5.IMG failed\n";
system("del \\epoc32\\rom\\nandloader\\autoexec.bat");
system("rename \\epoc32\\rom\\nandloader\\autoexec_orig.bat autoexec.bat")
	&& print "rename of autoexec_orig.bat failed\n";

sub CreateAutoExeFlash
{
	my $sAutoExec = "\\epoc32\\rom\\nandloader\\autoexec_flash.bat";
	open(sAUTOFLASH,">$sAutoExec");
	print sAUTOFLASH "z:\n";
	print sAUTOFLASH "nandloader -f -d 3 -a d: cldr.img core.img rofs1.img rofs2.img -r";
	close(sAUTOFLASH);
}
