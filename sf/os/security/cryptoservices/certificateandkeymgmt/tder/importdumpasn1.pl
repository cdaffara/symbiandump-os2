#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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
#!/bin/perl

use strict;
use Getopt::Long;

my $DEBUG = 0;
my $TABS = "";
my $OUTPUT_BUFFER = "";

main();
exit;

sub main() {
	my $out;
	my $outFh;
	my $in;	
	my @lines;

	GetOptions('debug=i' => \$DEBUG,
			   'in=s' => \$in,
			   'out=s' => \$out);

	if (! defined $in && defined $ARGV[0]) {
		$in = $ARGV[0];
	}

	if (defined $in) {
		@lines = decompile($in);
	}
	else {
		die "No input file specified.\n";
	}

	if (! defined $out && defined $ARGV[1]) {
		$out = $ARGV[1];
	}
	if (defined $out) {
		open $outFh, ">$out" || die "Cannot open output file $out";
	}
	else {
		$outFh = *STDOUT;
	}
	translate(\@lines);
	print $outFh $OUTPUT_BUFFER; 
}

sub translate($) {
	my ($lines) = @_;
	my $lineCount = scalar(@$lines);

	for (my $i = 0; $i < $lineCount; ++$i) {
		$_ = @$lines[$i];
		s/^\s*//g;
		s/\n//g;

		if ($DEBUG >= 3) {
			print "$_\n";
		}

		if ( /^OBJECT\s+IDENTIFIER\s*$/ ) {
			if ($DEBUG >= 3) {
				print "reading OID value from next line\n";
			}
			if (defined @$lines[$i+1]) {
				$_ .= @$lines[++$i];
			}
		}

		if ( /BIT\s+STRING,\s+encapsulates/i ) {
			addToOutput("BITSTRING_WRAPPER");
			nest();
		}
		elsif ( /^\s*BIT\s+STRING/i ) {
			# bitstring defined in binary
			if ( $$lines[$i+1] =~ /\'([01]+)\'/ ) {
				$i++;
				addToOutput("BITSTRING=$1");
			}
			# bit string defined in hex
			elsif ( /^\s*BIT\s+STRING\s+(([A-F0-9][A-F0-9]\s*)+)/i ) {
				my $bitStr = toBitString($1);
				addToOutput("BITSTRING=$bitStr");
			}
			else {
			# bit string wrapper
				addToOutput("BITSTRING_WRAPPER");
				nest();
				addToOutput("RAW \{");
				nest();
				$i++;
				addToOutput(getRawHex($lines,\$i));
				leaveNest();
				addToOutput(" \}");
				leaveNest();
				addToOutput("END");								
			}
		}
		elsif ( /^(BMPSTRING\s+)\'(.*)\'/i ) {
			addToOutput("BMPSTRING=$2");
		}
		elsif ( /^(BOOLEAN\s+)(.*)/i ) {
			addToOutput("BOOLEAN=$2");
		}
		elsif ( /(^ENUMERATED\s+)(\d+)*$/i ) {
			# small integer - non hex incoded
			addToOutput("ENUMERATED=$2");
		}
		elsif ( /^\[(\d+)\]\s*\'(.*)\'/ ) { 
			addToOutput("IMPLICIT=$1");
			nest();
			addToOutput("PRINTABLESTRING=$2");
			leaveNest();
			addToOutput("END");
		}
		elsif ( /^\[(\d+)\]/ ) {
			# general case for implicit & explicit tags
			my $tag=$1;
			if (defined @$lines[$i+1] && isRawData(@$lines[$i+1])) {
				# if there is only raw data assume implicit
				addToOutput("IMPLICIT=$tag");				
				nest();
				addToOutput("OCTETSTRING");
				nest();
				addToOutput("RAW \{");
				while (isRawData(@$lines[++$i])) {
					addToOutput("" . @$lines[$i] . "");
				}
				--$i;
				addToOutput("\}");
				leaveNest();
				addToOutput("END");
				leaveNest();
				addToOutput("END");
				leaveNest();
			}
			else {
				# otherwise assume explicit
				addToOutput("EXPLICIT=$tag");
			}
			nest();
		}			
		elsif ( /^(IA5STRING\s+)\'(.*)\'/i ) {
			addToOutput("IA5STRING=$2");
		}
		elsif ( /(^INTEGER\s+)(\d+)*$/i ) {
			# small integer - non hex incoded
			addToOutput("INTEGER=$2");
		}
		elsif (/^INTEGER/) {
			# big integer
			addToOutput("BIGINTEGER {");
			my $tmp = $_;
			$tmp =~ s/.*INTEGER\s+//g;
			nest();
			if (isRawData($tmp)) {
				addToOutput($tmp);
			}

			$i++;
			addToOutput(getRawHex($lines,\$i));
			leaveNest();
			addToOutput("\}");
		}
		elsif ( /^NULL/i ) {
			addToOutput("NULL");
		}
		elsif ( /^OCTET STRING\s*$/i ) {
			$i++;
			addToOutput("OCTETSTRING");				
			nest();
			addToOutput("RAW \{");
			nest();
			addToOutput(getRawHex($lines,\$i));
			leaveNest();
			addToOutput("\}");
			leaveNest();
			addToOutput("END");								
		}
		elsif ( /^OCTET\s+STRING.*encapsulates/i ) {
			addToOutput("OCTETSTRING");
			nest();			
		}
		elsif ( /^OCTET\s+STRING/i ) {
			addToOutput("OCTETSTRING");
			nest();			
			my $hex = $_;
			$hex =~ s/OCTET\s+STRING\s+//g;
			addToOutput("RAW=$hex");
			leaveNest();
			addToOutput("END");
		}
		elsif ( /^OBJECT\s+IDENTIFIER\s+\'([\d ]+)\'/i ) { 
			# plain oid
			my $oid = $1;
			$oid =~ s/ /./g;
			addToOutput("OID=$oid");
		}
		elsif ( /(^OBJECT\s+IDENTIFIER.*\()([\d ]+)/i ) { 
			# extra information printed with oid
			my $oid = $2;
			$oid =~ s/ /./g;
			addToOutput("OID=$oid");
		}
		elsif ( /(^PRINTABLESTRING\s*\')([^\']*)/i ) {
			addToOutput("PRINTABLESTRING=$2");
		}
		elsif ( /^SEQUENCE/i ) {
			addToOutput("SEQUENCE");
			nest();
		}
		elsif ( /^SET/i ) {
			addToOutput("SET");
			nest();
		}
		elsif (/^(UTCTIME\s+\')([^\']+)/i) {
			addToOutput("UTCTIME=$2");
		}
		elsif ( /^(UTF8STRING\s+)\'(.*)\'/i ) {
			addToOutput("UTF8STRING=$2");
		}
		elsif ( /^\}/) {				
			leaveNest();
			addToOutput("END");
		}
		elsif (isRawData($_)) {
			my $raw = s/\s+/ /g;
			addToOutput("RAW=$_");
		}

	}
}

sub addToOutput($) {
	my ($text) = @_;

	if ($DEBUG >= 3) {
		print "+${TABS}$text\n";
	}
	$OUTPUT_BUFFER .= "${TABS}$text\n";
}

sub getRawHex($) {
	my ($lines,$index) = @_;
	my $translated = '';

	my $end = 0;
	do {
		my $line = $$lines[$$index];
		last if (!defined $line);
		chomp($line);

		if (isRawData($line)) {
			$line =~ s/^\s+//g;
			addToOutput("$line");
			$$index++;
		}
		else {
			$$index--;
			$end = 1;
		}					
	} while (! $end);
	return $translated;
}

sub isRawData() {
	my ($line) = @_;
	my $retVal = ($line =~ /^\s*([A-F0-9][A-F0-9]\s?)+$/i);
	if ($DEBUG >= 3 && $retVal) {
		print "RAW: ";
	}
	return $retVal;
}

sub toBitString() {
	my ($hex) = @_;
	my $bitStr = "";
	$hex =~ s/\s//g;

	for (my $i=0; $i < length($hex); $i+=2) {
		my $num = hex(substr($hex, $i, 2));
		print ".$num";
		for (my $j=0; $j < 8; $j++) {
			$bitStr .= ($num & 0x80) ? '1' : '0';
			$num <<= 1;
		}
	}
	if ($DEBUG >= 2) {
		print "\nbitStr: $hex = $bitStr\n";
	}
	return $bitStr;
}

# increment debug tabbing level
sub nest() {
	$TABS .= "   ";
}

# decrement debug tabbing level
sub leaveNest() {
	$TABS =~ s/^...//;
}


sub decompile($) {
	my ($inFile) = @_;


	my @command = ("cmd", 
				   "/C \"dumpasn1 -apruz $inFile > _dump.tmp\"");

	if ((my $err = system(@command)) != 0) {
		die "decode: " . join(" ", @command) . "\nreturned error $err";
	}

	my $dumpFh;
	open $dumpFh, "_dump.tmp";
	my @lines = <$dumpFh>;
	close $dumpFh;

	return @lines;
}
