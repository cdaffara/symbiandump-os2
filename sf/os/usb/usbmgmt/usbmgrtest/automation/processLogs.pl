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

$suite = "";
$verbose = 1;

$numtests = $ARGV[0];

open TESTS, $ARGV[1] or die "Cannot open $ARGV[1]";
%config = (tdlogs => "", logdir => "", mainlog => "");
CONFIG: while (<TESTS>) {
	last CONFIG if /\[tests\]/i;
	next CONFIG if /^\n/;
	chomp;
	($var, $value) = split /=/, $_;
	if (exists $config{$var}) {
		$config{$var} = $value;
	}
}
close TESTS;

@rundirs = &find_rundirs;
@sorted = sort {
	$a =~ /RunNo([0-9]+)/;
	$l = $1;
	$b =~ /RunNo([0-9]+)/;
	$r = $1;
	$r <=> $l;
	} @rundirs;

open MAIN, ">".$config{"mainlog"} or die "Can't open ".$config{"mainlog"}.": $!";
print MAIN <<EOT;
<html>
<head>
<title>$suite Log</title></head>
<style type="text/css">
.pass { color: green; }
.fail { color: red; }
</style>
<body>
EOT
print "Number of test dirs to process: $numtests\n";
my @dirlist;
for ($i = 0; $i < $numtests; $i++) {
	$dirlist[$i] = $sorted[$i];
}
@dirlist = reverse @dirlist;
for ($i = 0; $i < $numtests; $i++) {
	&process_dir($dirlist[$i], "");
}
print MAIN "</body></html>";
close MAIN;

sub find_rundirs {
	my @rundirs;
	opendir(RESDIR, $config{"tdlogs"}) or die "Can't open ".$config{"tdlogs"}.": $!\n";
	my @dirs = grep { /^[^.]/ } readdir(RESDIR);
	close RESDIR;
	my $highest = 0;
	my $highDir = "";
	for my $dir (@dirs) {
		opendir(SUBDIR, $config{"tdlogs"}."\\$dir") or die "Can't open ".$config{"tdlogs"}."\\$dir: $!\n";
		@subdirs = grep { /[^.]/ } readdir(SUBDIR);
		for my $subdir (@subdirs) {
			push @rundirs, $config{"tdlogs"}."\\$dir\\$subdir";
#			print "Found ".$config{"tdlogs"}."\\$dir\\$subdir\n"
			#$subdir =~ /RunNo([0-9]+)/;
			#if ($1 > $highest) {
			#	$highest = $1;
			#	$highDir = $config{"tdlogs"}."\\$dir\\$subdir";
			#}
		}
		close SUBDIR;
	}
	#return $highDir;
	return @rundirs;
}

sub process_dir {
	my ($tdlogdir, $currdir) = @_;
	print "Processing $tdlogdir$currdir\n" if $verbose;
	chdir("$tdlogdir$currdir");
	#Find all log files that match the ones retrieved from the board and process them.
	while (<*.htm>) {
		/(.*)\.htm/;
		my $htmlog = $_;
		my $txtlog = "";
		print "Looking for ".$config{"logdir"}."\\$suite$currdir\\$1.txt\n" if $verbose;
		if (-e $config{"logdir"}."\\$suite$currdir\\$1.txt") {
			print "found $1.txt\n" if $verbose;
			$txtlog = $config{"logdir"}."\\$suite$currdir\\$1.txt";
		}
		&process_logs($1, $txtlog, $htmlog);
	}

	#Process subdirectories
	opendir(DIR, "$tdlogdir$currdir") or die "Can't open $tdlogdir$currdir: $!\n";
	my @dirs = grep { /^[^.]/ } readdir(DIR);
	close DIR;
	for my $dir (@dirs) {
		if (-d "$tdlogdir$currdir\\$dir") {
			&process_dir($tdlogdir, "$currdir\\$dir");
		}
	}
}

sub process_logs {
	my ($name, $txtlog, $htmlog) = @_;
	print MAIN "<b>$name Log</b><br/><br/>";
	my $fail = 0;
	$fail += &process_txt_log($txtlog) unless $txtlog eq "";
	$fail += &process_htm_log($htmlog);
	if ($fail > 0) {
		print MAIN "<b>$name: </b><span class=\"fail\"><b>$fail step(s) failed</b></span><br/>";
	} else {
		print MAIN "<b>$name: </b><span class=\"pass\"><b>All steps passed</b></span><br/>";
	}
}

sub process_txt_log {
	open LOG, $_[0] or die "Can't open".$_[0].": $!\n";
	my $fail = 0;
	while (<LOG>) {
		print MAIN "Command $_ : ";
		$_ = <LOG>;
		print MAIN "<span class=\"pass\">PASS" if /pass/i;
		do { print MAIN "<span class=\"fail\">FAIL"; $fail++ } if /fail/i;
		print MAIN "</span><br/>";
	}
	close LOG;
	return $fail;
}

sub process_htm_log {
	open LOG, $_[0] or die "Can't open".$_[0].": $!\n";
	print MAIN "<pre>\n";
	my $fail = 0;
	LINE: while (<LOG>) {
		next LINE if /<\/?html>/;
		if (/(FAIL|ABORT|PANIC|INCONCLUSIVE|UNKNOWN|UNEXECUTED) = ([0-9]+)/) {
			$fail += $2; 
		}		
		print MAIN;
	}
	print MAIN "</pre>\n";
	return $fail;
}


