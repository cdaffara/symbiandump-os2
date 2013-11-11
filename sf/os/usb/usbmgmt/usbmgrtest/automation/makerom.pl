#!/usr/bin/perl -w
# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

prebuildrom();
open ROMCONF, $ARGV[0] or die "Cannot open $ARGV[0]";
while (<ROMCONF>) {
	($name, $type, $flags, $obylist) = split /,/,;
	buildrom($name, $flags, $type, $obylist);
}
close ROMCONF;

sub prebuildrom {
	# Rebuild start.rsc to start stauto on startup
	unlink("\\epoc32\\data\\z\\System\\PROGRAMS\\start.rsc");
	system("attrib -r \\techview\\toolkit\\startup\\group\\start.rss");
	system("xcopy start.rss \\techview\\toolkit\\startup\\group\\start.rss /S/C/Y");
	system("attrib +r \\techview\\toolkit\\startup\\group\\start.rss");
	system("metabld start.mbc");
}

sub buildrom {
	my($name, $flags, $type, $obylist) = @_;

	chdir("\\epoc32\\rom");
	# Delete Existing Files
	unlink("$name.dir");
	unlink("$name.img");
	unlink("$name.log");
	unlink("$name.oby");
	unlink("$name.symbol");
	unlink("rombuild.txt");
	unlink("$name.zip");
	unlink("tmp1.oby");

	#Build and zip the rom
	system("buildrom.cmd $flags $type $obylist -o$name.img");
	print "Zipping the rom\n";
	system("zip -q -m -9 $name.zip $name.img");
}

