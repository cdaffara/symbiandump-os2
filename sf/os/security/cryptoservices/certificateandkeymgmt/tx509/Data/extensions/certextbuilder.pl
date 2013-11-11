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
#!/bin/perl -w

use strict;
use Getopt::Long;

my $OID_DEVICE_ID_LIST="1.2.826.0.1.1796587.1.1.1.1";
my $OID_SID_LIST="1.2.826.0.1.1796587.1.1.1.4";
my $OID_VID_LIST="1.2.826.0.1.1796587.1.1.1.5";
my $OID_CAPABILITIES="1.2.826.0.1.1796587.1.1.1.6";

# Reserved for future use
#my $OID_SUBSCRIBER_ID_LIST="1.2.826.0.1.1796587.1.1.2.2"; 
#my $OID_ICC_ID_LIST="1.2.826.0.1.1796587.1.1.1.3";

my $DER_UTF8STRING_TAG="0C";
my $DER_PRINTABLESTRING_TAG="13";
my $DER_SEQUENCE_TAG="30";
my $DER_INTEGER_TAG="02";
my $DER_BITSTRING_TAG="03";
my $DER_OCTET_TAG="04";

my %CAPABILITY_SET = (
	"TCB" => 0,
	"COMMDD" => 1,
	"POWERMGMT" => 2,
	"MULTIMEDIADD" => 3,
	"READDEVICEDATA" => 4,
	"WRITEDEVICEDATA" => 5,
	"DRM" => 6,
	"TRUSTEDUI" => 7,
	"PROTSERV" => 8,
	"DISKADMIN" => 9,
	"NETWORKCONTROL" => 10,
	"ALLFILES" => 11,
	"SWEVENT" => 12,
	"NETWORKSERVICES" => 13,
	"LOCALSERVICES" => 14,
	"READUSERDATA" => 15,
	"WRITEUSERDATA" => 16,
	"LOCATION" => 17,
	"SURROUNDINGSDD" => 18,
	"USERENVIRONMENT" => 19	
	);

# Sections encountered
my %sections;

# Fields within sectins
my @device_id_list = ();
my @sid_list = ();
my @vid_list = ();
my $capabilities = '';

main();
exit(0);

# Main function.
# Loops through the input file and creates lists of items for each section.
sub main {
	my $section = "";
	my $output_binary = 0;

	GetOptions('binary' => \$output_binary);			   
	my ($infile, $outfile) = @ARGV;
	
	if (defined $infile) {
		open IN, "$infile" || die "Cannot open input file $infile";
	}
	else {
		*IN = *STDIN;
	}

	if (defined $outfile) {
		open OUT,  ">$outfile" || die "Cannot open input file $outfile";
	}
	else {
		*OUT = *STDOUT;
	}
	
	while (<IN>) {
		chomp;
		if (/^\s*\[\w*\]/) {
			# Process the new section name
			
			s/.*\[\s*(.*)\s*].*/$1/g;
			$section = lc($_);
    		$sections{$section} = 1;
	    }
	    elsif (/^\s*\#/) {
			# Comment - ignore
		}
	    elsif (/.*=.*/ ) {
			# Process element within section
			s/.*=//g;
			if ($section eq "device_id_list") {
				push @device_id_list, $_;
			}
            elsif ($section eq "sid_list") {
                push @sid_list, $_;
			}
			elsif ($section eq "vid_list") {
				push @vid_list, $_;
			}
			elsif ($section eq "cert_capabilities") {

				if (! $capabilities eq "") {
					die "Error: Multiple capability constraints defined.";
				}

				$capabilities = $_;
				if ($capabilities eq "" || $capabilities =~ /^\s*[01]+\s*$/)  {
					# Handle explicit bit strings
					$capabilities =~ s/\s//g;
				}
				else {
					# Convert MMP syntax into an explicit bit string
					$capabilities = &encode_capabilities($capabilities);					
				}
			}
			else {
				# Not in a section so ignore text
			}
		}   
   }   
 
   if ($output_binary) {
	   print_to_der();  
   }
   else {
       print_to_cnf();
   }
   close IN;
   close OUT;
}

# Test function which outputs the binary DER encoding. This can be vieweed using an
# ASN.1 viewer.
# This is really a debug function.
sub print_to_der {
	my $seq_octets = 0; 
	my $seq_content = "";

	if (defined $sections{"device_id_list"}) { 
		$seq_content .= ":" . &encode_string_list(\@device_id_list, \$seq_octets);
	}
	
	if (defined $sections{"sid_list"}) {
		$seq_content .= ":" . &encode_integer_list(\@sid_list, \$seq_octets);
	}

	if (defined $sections{"vid_list"}) {
		$seq_content .= ":" . &encode_integer_list(\@vid_list, \$seq_octets);
	}

	if (defined $sections{"cert_capabilities"}) {
		$seq_content .= ":" . &encode_bit_string($capabilities, \$seq_octets);
	}
	# Tidy up repreated colons
	$seq_content =~ s/::/:/;
	$seq_content =~ s/^://g;
	
	my $seq_length_octets;
	my $seq_length = &encode_length($seq_octets, \$seq_length_octets);
	my $seq ="$DER_SEQUENCE_TAG:$seq_length:$seq_content";
	
	$seq =~ s/::/:/g;
	$seq = uc($seq);
	
	binmode(OUT);
	foreach (split(/:/, $seq)){
		print OUT pack('C', hex);
	}
}

# Output to a format that can be read by Open SSL using the -extfile parameter
sub print_to_cnf {
   print OUT "extensions = extend\n";
   print OUT "[extend]\n";
   
   my $octets = 0; 
   my $output = "";

   if (defined $sections{"device_id_list"}) {
	   $output .= "# Device ID List\n" .
		   $OID_DEVICE_ID_LIST . "= critical, " . "DER:" . 
		   uc(&encode_string_list(\@device_id_list, \$octets)) . "\n";
   }      
   
   if (defined $sections{"sid_list"}) {
	   $output .= "# SID List\n" .
		   $OID_SID_LIST . "= critical, " . "DER:" . 
		   uc(&encode_integer_list(\@sid_list, \$octets)) . "\n";
   }   

   if (defined $sections{"vid_list"}) {
	   $output .= "# VID List\n" .
	   $OID_VID_LIST . "= critical, " . "DER:" . uc(&encode_integer_list(\@vid_list, \$octets)) . "\n";
   }   

   if (defined $sections{"cert_capabilities"}) {
	   $output .= "# Capabilities\n" .
		   $OID_CAPABILITIES . "= critical, " . "DER:" . uc(&encode_bit_string($capabilities, \$octets)) . "\n";
   }   
   
   # Remove trailing colons
   $output=~ s/\:*$//mg;
   print OUT $output;
}

# Creates a hex representation of the DER encoding of a sequence of strings.
sub encode_string_list($$) {
	my ($list, $octets) = @_;
	
	my $sequence_body = "";
	
	my $sequence_octets = 0;
	foreach (@$list) {
		my $hex_string = &encode_utf8_string($_, \$sequence_octets);

		# Add to string sequence body
		if ($sequence_body ne "") {
			$sequence_body .= ":";
		}
		$sequence_body .= $hex_string;
	}
	my $seq_length_octets = 0;
	my $seq_length = &encode_length($sequence_octets, \$seq_length_octets);

	$$octets += 1 + $seq_length_octets + $sequence_octets;

	return "$DER_SEQUENCE_TAG:$seq_length:$sequence_body";
}

# Creates a hex represenation of the DER encoding of a sequence of integers.
sub encode_integer_list($$) {
	my ($list, $octets) = @_;	

	my $sequence_body = "";
	my $sequence_octets = 0;
	foreach (@$list) {
		# Increment for integer tag value
		# Increment for integer length < 127 octets assumed !
		$sequence_octets+=2;

		if (s/^0x//) {
			$_ = hex;
		}
		
		# Convert the integer to base 256 hex and find out how
		# many octets were required
		my $hex_octets;
		my $hex_integer;
		if (//) {
			$hex_octets = 0;
			$hex_integer = "";
		}
		else {
			$hex_integer = &to_hex_base256($_, \$hex_octets);
			$sequence_octets += $hex_octets;
		}
		
		# Add to integer  sequence body
		if ($sequence_body ne "") {
			$sequence_body .= ":";
		}
		
		# No need to store length in long form because in base256
		# we never need more than 7 octets to store the largest number
		# that we need.
		my $int_header = sprintf("%2.2x", $hex_octets);
		$sequence_body .= "$DER_INTEGER_TAG:$int_header:$hex_integer";		
	}

	# Get the number of octets of the entire sequence. This could require
	# encoding in long form.
	my $seq_length_octets = 0;
	my $seq_length = &encode_length($sequence_octets, \$seq_length_octets);

	$$octets += 1 + $seq_length_octets + $sequence_octets;

	return "$DER_SEQUENCE_TAG:$seq_length:$sequence_body";
}

# Creates a hex represenation of the DER encoding of a UTF-8 string.
sub encode_utf8_string($$) {
	my ($input, $octets) = @_;

	# Hex encoded string
	my $output = "";
	my $input_len = length($input);	
	my $i = 0;
	while ($i < $input_len) {
		my $hex_val = ord(substr($input, $i, 1));
		if ($output ne "") {
			$output .= ":";
		}
		$output .= sprintf("%2.2x", $hex_val);
		$i++;
	}
	
	# Build string header
	my $output_length_octets = 0;		
	my $output_length = &encode_length($input_len, \$output_length_octets);
	
	# Track number of octets added for string header
	$$octets += 1 + $output_length_octets + $input_len;
	return "$DER_UTF8STRING_TAG:$output_length:$output";
}

# Converts the text description of capabilities into an ASCII string of 0s and 1s;
sub encode_capabilities($) {
	my ($value) = @_;
	my $output = "";
	my @caps = (0);
	
	$value = uc($value);
	foreach (split(/[\s,]/, $value)) {
		my $set_val = 1;

		if (s/^-//g) {
			$set_val = 0;
		}
		
		if (/^ALL$/) {
			foreach (keys %CAPABILITY_SET) {
				@caps[$CAPABILITY_SET{$_}] = 1;
			}
		}
	    elsif (/^NONE$/) {
			@caps = ();
		}
		if (defined $CAPABILITY_SET{$_}) {
			$caps[$CAPABILITY_SET{$_}] = $set_val;
		}
	}

	# Build the ascii bit string. Bit 0 is the left most bit
	for (my $i = 0; $i <= $#caps; $i++) {
		$output .= (defined $caps[$i] && $caps[$i] ? "1" : "0");
	}

	return $output;
}

# Creates a hex representation of the DER encoding of an arbitrary length bit string
sub encode_bit_string($$) {
	my ($text, $octets) = @_;

	# Bit string in hex including padding length octet
	my $bit_str = "";
	my $bit_str_octets = 1; # one octet for padding

	# Current byte
	my $byte = 0;	
	my $len = length($text);

	if ($len == 0) {
		$$octets+=2;
		return "03:00";
	}

	my $i = 0;
	while ($i < $len) {		

		# Read the ith character and insert it in the correct place in the byte
		# (fill from the left)
		my $c = substr($text, $i, 1);		
		if ($c eq "1") {
			$byte |= (1 << (7 - ($i % 8)));
		}
		elsif ($c ne "0") {
			die "Invalid character $c in bit string $text";
		}

		if (++$i % 8 == 0) {
			# Received 8 bits so output byte in hex
			if ($bit_str ne "") {
				$bit_str .= ":";
			}
			$bit_str .= sprintf("%2.2x", $byte);
			$bit_str_octets++;
			$byte = 0;
		}
	}
	# Pad any remaining bits / make sure 0 is output for empty string
	if ($byte != 0 || $bit_str_octets == 1) {
		if ($bit_str ne "") {
			$bit_str .= ":";
		}
		$bit_str .= sprintf("%2.2x", $byte);
		$bit_str_octets++;
	}

	my $pad_length = "00";
	if ($len % 8 > 0) {
		# If this isn't a multiple of 8 bits then calculated
		# the number of padding bits added.
		$pad_length = sprintf("%2.2x", 8 - ($len % 8));
	}
	
	# Octets used to store the length
	my $bit_str_length_octets = 0;
	my $bit_str_length = &encode_length($bit_str_octets, \$bit_str_length_octets);
	$$octets += 1 + $bit_str_length_octets + $bit_str_octets;

	return "$DER_BITSTRING_TAG:$bit_str_length:$pad_length:$bit_str";
}

# Return a hex represenation of the length using DER primitive (definate length encoding)
sub encode_length($$) {
	my ($num, $octets) = @_;

	if ($num < 128) {
		# Number is < 128 so encode in short form
		$$octets++;
		return sprintf("%2.2x", $num);
	}
	else {
		# Number >= 128 so encode in long form
		my $length_octets = 0;
		my $base256 = &to_hex_base256($num, \$length_octets);
		if ($length_octets > 127) {die "Encoding overflow.";}
		
		$$octets += 1 + $length_octets;
		
		# Set the top bit of the length octet to indicate long form		
		return "" . sprintf("%2.2x", ($length_octets | 0x80)) . ":$base256";
	}
}

# Convert an integer into an ascii hex representation in base 256
# $num    - the number to encode
# $octets - refernce to the octet count to increment
sub to_hex_base256($$) {
	my ($num, $octets) = @_;

	my $base256 = "";
	$num = int($num);
	while ($num > 0) {
		my $hexoctet = sprintf("%2.2x", $num & 0xFF);
		if ($base256 ne "") {
			$base256 = "$hexoctet:$base256";
		}
		else {
			$base256 = $hexoctet;
		}		
		$num >>= 8;
		$$octets++;
	}
	if ($base256 eq "") {
		$base256 = "0";
		$$octets++;
	}
	return $base256;
}
