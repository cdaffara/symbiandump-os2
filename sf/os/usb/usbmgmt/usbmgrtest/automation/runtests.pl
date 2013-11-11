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

use File::Path;
use Cwd;

#read config file
open TESTS, $ARGV[0] or die "Cannot open $ARGV[0]";
%config = (testdriver => "", logdir => "", romdir => "");
CONFIG: while (<TESTS>) {
	last CONFIG if /\[tests\]/i;
	next CONFIG if /^\n/;
	chomp;
	($var, $value) = split /=/, $_;
	if (exists $config{$var}) {
		$config{$var} = $value;
	}
}
#Save location of test list start
$testspos = tell TESTS;

print "td = ".$config{"testdriver"}."\n";
print "logdir = ".$config{"logdir"}."\n";
print "romdir = ".$config{"romdir"}."\n";

#delete old logs
print "deleting ".$config{"logdir"}."\n";
rmtree($config{"logdir"});

$cwd = getcwd;
chdir($config{"testdriver"});

#Build tests
print "Building tests\n";
while(<TESTS>) {
	($suite, $plat, $rel) = split /,/, $_;
	print "testdriver.exe build -p $plat -b $rel -s $suite\n";
	system("testdriver.exe build -p $plat -b $rel -s $suite");
}
#return to the beggining for later processing
seek TESTS, $testspos, 0;

#Run tests
$numtests = 0;
while(<TESTS>) {
	($suite, $plat, $rel, $rom) = split /,/, $_;

	#Turn board on
	system("hardwareswitch off");
	sleep(5);
	system("hardwareswitch on");
	sleep(5);

	#Transfer rom to board
	system("\\epoc32\\release\\tools\\rel\\trgtest.exe 1 $config{'romdir'}\\$rom");
	#I know, another hardcoded delay... but how is the script supposed to know when
	#the board has finished booting?
	sleep(200);
	system("testdriver.exe run -p $plat -b $rel -s $suite -t serial2");
	$numtests++
}

#Compile the log files
chdir($cwd);
system("perl \\build\\processLogs.pl $numtests $ARGV[0]");
