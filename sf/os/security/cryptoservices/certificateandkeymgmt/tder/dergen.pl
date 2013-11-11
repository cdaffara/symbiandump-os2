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
use Digest::HMAC_MD5;
use Digest::HMAC_SHA1;
use Getopt::Long;

# 0 = off
# 1 = log parsing
# 2 = log parsing + encoding
# 3 = really verbose stuff
my $DEBUG=0;

# Turn on validation checks that attempt to only generate
# valid DER encodings.
my $VALIDATE=0;

my $OID_PKCS = "1.2.840.113549.1";
my $OID_PKCS7 ="${OID_PKCS}.7";
my $OID_PKCS9 = "${OID_PKCS}.9";
my $OID_PKCS9_CERTTYPES = "${OID_PKCS9}.22"; 
my $OID_PKCS12 = "${OID_PKCS}.12";
my $OID_PKCS12_BAGTYPES = "${OID_PKCS12}.10.1";
my $OID_PKCS12_PBEIDS = "${OID_PKCS12}.1";

my %OIDS = 
	(
	 "MD5"  => "1.2.840.113549.2.5",
	 "SHA1" => "1.3.14.3.2.26",
	 "X509CRL" => "1.3.6.1.4.1.3627.4",

	 "PKCS7_DATA" => "${OID_PKCS7}.1",
	 "PKCS7_SIGNEDDATA" => "${OID_PKCS7}.2",
	 "PKCS7_ENVELOPEDDATA" => "${OID_PKCS7}.3",
	 "PKCS7_SIGNEDANDENVELOPEDDATA" => "${OID_PKCS7}.4",
	 "PKCS7_DIGESTEDDATA" => "${OID_PKCS7}.5",
	 "PKCS7_ENCRYPTEDDATA" => "${OID_PKCS7}.6",	
	 
	 "PKCS9_CERTTYPES_PKCS12_X509" => "${OID_PKCS9_CERTTYPES}.1",
	 "PKCS9_FRIENDLY_NAME" => "${OID_PKCS9}.20",
	 "PKCS9_LOCAL_KEYID" => "${OID_PKCS9}.21",
	 
	 "PKCS12_BAGTYPES_KEYBAG" => "${OID_PKCS12_BAGTYPES}.1",
	 "PKCS12_BAGTYPES_PKCS8SHROUDEDKEYBAG" => "${OID_PKCS12_BAGTYPES}.2",
	 "PKCS12_BAGTYPES_CERTBAG" => "${OID_PKCS12_BAGTYPES}.3",
	 "PKCS12_BAGTYPES_CRLBAG" => "${OID_PKCS12_BAGTYPES}.4",
	 "PKCS12_BAGTYPES_SECRETBAG" => "${OID_PKCS12_BAGTYPES}.5",
	 "PKCS12_BAGTYPES_SAFECONTENTSBAG" => "${OID_PKCS12_BAGTYPES}.6",

	 "PKCS12_PBEIDS_SHAAND128BITRC4" => "${OID_PKCS12_PBEIDS}.1",
	 "PKCS12_PBEIDS_SHAAND40BITRC4" => "${OID_PKCS12_PBEIDS}.2",
	 "PKCS12_PBEIDS_SHAAND3KEYTRIPLEDESCBC" => "${OID_PKCS12_PBEIDS}.3",
	 "PKCS12_PBEIDS_SHAAND2KEYTRIPLEDESCBC" => "${OID_PKCS12_PBEIDS}.4",
	 "PKCS12_PBEIDS_SHAAND128BITRC2CBC" => "${OID_PKCS12_PBEIDS}.5", 
	 "PKCS12_PBEIDS_SHAAND40BITRC2CBC" => "${OID_PKCS12_PBEIDS}.6",

	 # Symbian dev cert extensions
	 "SYMBIAN_DEVICE_ID_LIST" => "1.2.826.0.1.1796587.1.1.1.1",
	 "SYMBIAN_SID_LIST" => "1.2.826.0.1.1796587.1.1.1.4",
	 "SYMBIAN_VID_LIST" => "1.2.826.0.1.1796587.1.1.1.5",
	 "SYMBIAN_CAPABILITIES" => "1.2.826.0.1.1796587.1.1.1.6"

);

my $DER_BOOLEAN_TAG="01";
my $DER_INTEGER_TAG="02";
my $DER_BITSTRING_TAG="03";
my $DER_OCTETSTRING_TAG="04";
my $DER_NULL_TAG="05";
my $DER_OID_TAG="06";
my $DER_ENUMERATED_TAG="0A";
my $DER_SEQUENCE_TAG="10";
my $DER_SET_TAG="11";
my $DER_UTF8STRING_TAG="0C";
my $DER_PRINTABLESTRING_TAG="13";
my $DER_IA5STRING_TAG="16";
my $DER_UTCTIME_TAG="17";
my $DER_BMPSTRING_TAG="1E";

my $UNIVERSAL_CLASS="UNIVERSAL";
my $APPLICATION_CLASS="APPLICATION";
my $CONTEXT_SPECIFIC_CLASS="CONTEXT-SPECIFIC";
my $PRIVATE_CLASS="PRIVATE";

my %PARSE = 
	(
	 "BOOL" => \&parseBoolean,
	 "BOOLEAN" => \&parseBoolean,
	 "BIGINTEGER" => \&parseBigInteger,
	 "BITSTRING" => \&parseBitString,
	 "BITSTRING_WRAPPER" => \&parseBitStringWrapper,
	 "BMPSTRING" => \&parseBmpString,
	 "BMPSTRING_FILE" => \&parseBmpStringFile,
	 "ENUMERATED" => \&parseEnumerated,
	 "IA5STRING" => \&parseIA5String,
	 "IA5STRING_FILE" => \&parseIA5StringFile,
	 "INCLUDE" => \&parseInclude,
	 "INCLUDE_BINARY_FILE" => \&parseIncludeBinaryFile,
	 "INTEGER" => \&parseInteger,
	 "INT" => \&parseInteger,
	 "IMPLICIT" => \&parseImplicit,
	 "ENCRYPT" => \&parseEncrypt,
	 "EXPLICIT" => \&parseExplicit,
	 "HASH" => \&parseHash,
	 "HMAC" => \&parseHmac,
	 "NULL" => \&parseNull,
	 "OCTETSTRING" => \&parseOctetString,
	 "OUTPUT_BINARY_FILE" => \&parseOutputFile,
	 "OID" => \&parseOid,
	 "PRINTABLESTRING" => \&parsePrintableString,
	 "PRINTABLESTRING_FILE" => \&parsePrintableStringFile,
	 "RAW" => \&parseRaw,
	 "SEQUENCE" => \&parseSequence,
	 "SEQ" => \&parseSequence,
	 "SET" => \&parseSet,
	 "SHELL" => \&parseShell,
	 "SIGN" => \&parseSign,
	 "UTCTIME" => \&parseUtcTime,
	 "UTF8STRING" => \&parseUtf8String,
	 "UTF8STRING_FILE" => \&parseUtf8StringFile,
	 );

my $TABS = "";

&main;
exit(0);

sub main() {
	my $hex;
	my $out;
	my $in;	
	my @lines;

	GetOptions('debug=i' => \$DEBUG,
			   'hex' => \$hex, 
			   'in=s' => \$in,
			   'out=s' => \$out);

	if (! defined $in) {
		$in = $ARGV[0];
	}

	if (! defined $out) {
		$out = $ARGV[1];
	}

	if (defined $in) {
		@lines = readFile($in);
	}
	else {
		die "No input file specified.\n";
	}

	if (defined $out) {
		open OUT, ">$out" || die "Cannot open output file $out";
	}
	else {
		*OUT = *STDOUT;
	}

	my $oc = 0;
	my $asnHex = parseScript(\@lines, \$oc);
	$asnHex = tidyHex($asnHex);

	if ((!defined $hex) && (defined $out)) {
		binmode(OUT);
		print OUT toBin($asnHex);
	}
	elsif (defined $out) {
		print OUT $asnHex;
	}
	else {
		print $asnHex;
	}

	close OUT;
}

sub tidyHex($) {
	my ($input) = @_;	
	$input =~ s/:+/:/g;
	$input =~ s/(^:|:$)//g;
	return uc($input);
}

sub toBin($) {
	my ($asnHex) = @_;

	$asnHex =~ s/[\s:]//g;
	$asnHex = uc($asnHex);
	
	my $len = length($asnHex);
	if ($len % 2 != 0) {
		die "toBin: hex string contains an odd number ($len) of octets.\n$asnHex\n";
	}

	my $binary;
	$binary .= pack("H${len}", $asnHex);
#	for (my $i = 0; $i < length($asnHex); $i+=2) {
#		$binary .= pack('C', substr($asnHex, $i, 2));
#	}
	return $binary;
}

sub parseScript($$;$) {
	my ($lines, $oc, $params) = @_;
	my $derHex = "";

	nest();
	substVars($lines, $params);

	while (my $line = shift @$lines) {
		chomp($line);

		# Remove leading spaces
		$line =~ s/^\s*//g;
  
		# skip comments 
		next if ($line =~ /^\/\//);

		if ($DEBUG == 3) {
			print "${TABS}:PARSE parseScript: $line\n";
		}

		my $argString;
		my $cmd;
		if ($line =~ /(\w+)\s*\{/ ) {
			# parse block commands e.g. large integer
			$cmd = uc($1);
			
			$line =~ s/.*\{//g;
			while (defined $line && !($line =~ /(^|[^\\]+)\}/) ) {
				$argString .= $line;
				$line = shift(@$lines);				
			}
			if (defined $line) {
				# append everything up to the closing curly bracket
				$line =~ s/(^|[^\\])\}.*/$1/g;
				$argString .= $line;
			}
		}	
		elsif ($line =~ /(\w+)\s*=*(.*)/) {
			# parse commands of the form key = value
			$cmd = uc($1);
			$argString = defined $2 ? $2 : "";			
		}

		if (defined $cmd) {
			if ($cmd =~ /^END/) {
				leaveNest();
				if ($DEBUG) {
					print "${TABS}:PARSE END\n";
				}
				return $derHex;
			}
			elsif (! defined $PARSE{$cmd}) {
				die "parseScript: Unknown command: $cmd\n";
			}
			else {
				if ($DEBUG) {
					print "${TABS}:PARSE CMD=$cmd";					
					if ($argString ne "") {print " ARG: $argString";}
					print "\n";
				}
				
				# Substitue variables in argString
				$derHex .= ":" . &{$PARSE{$cmd}}($argString, $oc, $lines);
			}
		}

	}
	leaveNest();
	return $derHex;
}

sub substVars($$) {
	my ($lines, $params) = @_;

	if (! defined $params) {
		@$params = ();
	}

	for (my $i = 0; $i < scalar(@$lines); $i++) {
		my $line = @$lines[$i];
		my $paramIndex = 1;

		# For each parameter search for the a use of $N where
		# N is the index of the parameter and replace $N with the
		# value of the parameter
		foreach (@$params) {
			$line =~ s/\$${paramIndex}(\D|$)/$_$1/g;	
			++$paramIndex;
		}
		
		# Remove any unused parameters
		$line =~ s/\$\d+//g;
		@$lines[$i] = $line;
	}
}

sub readFile($) {
	my ($fileName) = @_;
	my $inFile;

	if ($DEBUG) {
		print "readFile, $fileName\n";
	}

	open($inFile, $fileName) || die "readFile: cannot open $fileName\n";	
	my @lines = <$inFile>;
	close $inFile;

	return @lines;
}

sub parseBitString($$;$) {
	my ($argString, $oc, $lines) = @_;	
	return encodeBitString($argString, $oc);
}

sub parseBitStringWrapper($$;$) {
	my ($argString, $oc, $lines) = @_;	

	my $contents_oc = 0;
	my $contents = parseScript($lines, \$contents_oc);

	my $binary = toBin($contents);
	my $bitCount = $contents_oc * 8;
	my $bitStr = unpack("B${bitCount}", $binary);

	# remove trailing zeros - breaks signatures so disable for the moment
	# $bitStr =~ s/0*$//g;
	
	return encodeBitString($bitStr, $oc);
}

sub parseBmpString($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $bmpString_oc = 0;
	my $bmpString = asciiToBmpString($argString, \$bmpString_oc);
	return encodeBmpString($bmpString, $bmpString_oc, $oc);
}

sub parseBmpStringFile($$;$) {
	my ($binFName, $oc, $lines) = @_;
	$binFName =~ s/\s*//g;
	
	my $bmpString_oc = 0;
	my $bmpString = encodeBinaryFile($binFName, \$bmpString_oc);	
	
	return encodeBmpString($bmpString, $bmpString_oc, $oc);
}

sub parseBoolean($$;$) {
	my ($argString, $oc, $lines) = @_;
	
	$argString =~ s/\s//g;
	$argString = lc($argString);

	my $bool;
	if ($argString eq "t" || $argString eq "true" || $argString eq "1") {
		$bool = 1;
	}
	elsif ($argString eq "f" || $argString eq "false" || $argString eq "0") {
		$bool = 0;
	}
	else {
		die "parseBoolean: Invalid boolean value \'$argString\'";
	}
	
	return encodeBoolean($bool, $oc);
}

sub parseHash($$;$) {
	my ($argString, $oc, $lines) = @_;
	my ($algorithm) = getArgs($argString);

	if (! defined $algorithm) {
		die "parseHash: missing algortithm";
	}

	my $hashIn_oc = 0;
	my $hashIn = parseScript($lines, \$hashIn_oc);

	my $hashInFName = '_hashin.tmp';
	my $hashOutFName = '_hashout.tmp';

	# Create binary hash file
	my $hashInFh;
	open($hashInFh, ">$hashInFName") or die "Cannot create $hashInFName";
	binmode($hashInFh);
	print $hashInFh toBin($hashIn);
	close $hashInFh;

	my @command = ("cmd",
				   "/C \"openssl dgst -${algorithm} -binary $hashInFName > $hashOutFName\"");	
	if ($DEBUG == 1) {
		print "${TABS}:parseHash:" . join(" ", @command) . "\n";
	}

	if ((my $err = system(@command)) != 0) {
		die "parseHash: " . join(" ", @command) . "\nreturned error $err";
	}

	my $derHex = parseIncludeBinaryFile($hashOutFName, $oc);
	
	if (! $DEBUG) {
		unlink($hashInFName);
		unlink($hashOutFName);
	}
	return $derHex;
}

sub parseHmac($$;$) {
	my ($argString, $oc, $lines) = @_;
	my ($algorithm, $key) = getArgs($argString);

	if (! defined $algorithm) {
		die "parseHmac: missing algortithm";
	}
	$algorithm = uc($algorithm);
	if (! $algorithm =~ /MD5|SHA1/) {
		die "parseHmac: invalid algorithm $algorithm";
	}

	if (! defined $key) {
		die "parseHmac: missing key";
	}

	my $hmacIn_oc = 0;
	my $hmacIn = toBin(parseScript($lines, \$hmacIn_oc));
	my $hmac;
	my $binKey = toBin($key);

	if ($algorithm eq "SHA1") {

		$hmac = Digest::HMAC_SHA1->new($binKey);
	}
	else {
		$hmac = Digest::HMAC_MD5->new($binKey);
	}
	$hmac->add($hmacIn);
	my $digest = $hmac->digest;
	$$oc += length($digest);

	return toHex($digest);
}

sub parseIA5String($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $ia5String_oc = 0;
	my $ia5String = asciiToIA5String($argString, \$ia5String_oc);
	return encodeIA5String($ia5String, $ia5String_oc, $oc);
}


sub parseIA5StringFile($$;$) {
	my ($binFName, $oc, $lines) = @_;
	$binFName =~ s/\s*//g;
	
	my $ia5String_oc = 0;
	my $ia5String = encodeBinaryFile($binFName, \$ia5String_oc);	
	
	return encodeIA5String($ia5String, $ia5String_oc, $oc);
}

sub parseIncludeBinaryFile($$;$) {
	my ($binFName, $oc, $lines) = @_;
	$binFName =~ s/\s*//g;
	
	return encodeBinaryFile($binFName, $oc);
}

sub parseInclude($$$) {
	my ($argString, $oc, $lines) = @_;   
	my @args = getArgs($argString);

   	my $fileName = shift(@args);
	if (! (defined $fileName && $fileName ne "")) {
		die "parseInclude: Filename not specified\n";
	}

	my $derHex = "";
	my @lines = readFile($fileName);	
	$derHex = parseScript(\@lines, $oc, \@args);
	return $derHex;
}

sub parseInteger($$;$) {
	my ($argString, $oc, $lines) = @_;
	
	$argString =~ s/\s//g;
	return encodeInteger($argString, $oc);
}

sub parseBigInteger($$;$) {
	my ($argString, $oc, $lines) = @_;
	
	$argString =~ s/\s//g;
	return encodeBigInteger($argString, $oc);
}

sub parseEncrypt($$;$) {
	my ($argString, $oc, $lines) = @_;		
	my ($cipher, $key, $iv) = getArgs($argString);

	if (! defined $cipher) {
		die "parseEncrypt: missing cipher\n";
	}

	if (! defined $key) {
		die "parseEncrypt: missing key\n";
	}

	my $plainText_oc = 0;
	my $plainText = parseScript($lines, \$plainText_oc);

	my $plainTextFName = '_plaintext.tmp';
	my $cipherTextFName = '_ciphertext.tmp';

	# Create binary plaintext file
	my $plainTextFh;
	open($plainTextFh, ">$plainTextFName") or die "Cannot create $plainTextFName";
	binmode($plainTextFh);
	print $plainTextFh toBin($plainText);
	close $plainTextFh;

	my @command = ('openssl', 
				   'enc', 
				   "-${cipher}", 
				   '-e',
				   '-K', $key,
				   '-in', $plainTextFName, 
				   '-out', $cipherTextFName);

	if (defined $iv) {
		push @command, '-iv', $iv;
	}
	
	if ($DEBUG == 1) {
		print "${TABS}:parseEncrypt:" . join(" ", @command) . "\n";
	}

	if ((my $err = system(@command)) != 0) {
		die "parseEncrypt: " . join(" ", @command) . "\nreturned error $err";
	}

	my $derHex = parseIncludeBinaryFile($cipherTextFName, $oc);
	
	if (! $DEBUG) {
		unlink($plainTextFName);
		unlink($cipherTextFName);
	}
	return $derHex;
}

sub parseEnumerated($$;$) {
	my ($argString, $oc, $lines) = @_;
	
	$argString =~ s/\s//g;
	return encodeEnumerated($argString, $oc);
}

sub parseExplicit($$;$) {
	my ($argString, $oc, $lines) = @_;	
	my ($tagNumber, $class) = getArgs($argString);

	if (! defined $tagNumber || $tagNumber =~ /^\s*$/) {
		$tagNumber = "0";
	}
	elsif (!($tagNumber =~ /^[0-9A-Fa-f]+$/)) {
		die "parseExplicit: invalid tag number: \'$tagNumber\'";
	}
	$tagNumber = hex($tagNumber);

	if (!defined $class || $class =~ /^\s*$/) {
		$class = $CONTEXT_SPECIFIC_CLASS;
	}
	else {
		$class =~ s/\s*//g;
		$class = uc($class);
	}

	if (! isValidClass($class)) {
		die "parseExplicit: invalid class \'$class\'";
	}
	
	my $nested_oc = 0;
	my $nested = parseScript($lines, \$nested_oc);

	return encodeExplicit($class, $tagNumber, $nested, $nested_oc, $oc);
}

sub parseImplicit($$;$) {
	my ($argString, $oc, $lines) = @_;	
	my ($tagNumber, $class) = getArgs($argString);

	if (! defined $tagNumber || $tagNumber =~ /^\s*$/) {
		$tagNumber = "0";
	}
	elsif (!($tagNumber =~ /^[0-9A-Fa-f]+$/)) {
		die "parseImplicit: invalid tag number: \'$tagNumber\'";
	}
	$tagNumber = hex($tagNumber);

	if (!defined $class || $class =~ /^\s*$/) {
		$class = $CONTEXT_SPECIFIC_CLASS;
	}
	else {
		$class =~ s/\s*//g;
		$class = uc($class);
	}

	if (! isValidClass($class)) {
		die "parseImplicit: invalid class \'$class\'";
	}
	
	my $nested_oc = 0;
	my $nested = tidyHex(parseScript($lines, \$nested_oc));

	# De-construct the nested data to allow the underlying type tag to be
	# changed. The output of parseScript had better be valid DER or this 
	# will go horribly wrong !
	my $uClass = "";
	my $uConstructed = 0;
	my $uTag = 0;
	my $uLength = 0;
	my $uValue = "";
	getTlv($nested, \$uClass, \$uConstructed, \$uTag, \$uLength, \$uValue);

	if ($DEBUG == 2) {
		print "${TABS}parseImplicit: underlyingType \'$uTag\'\n";
	}
	
	# This only works for low tag numbers because we are assuming that the type
	# tag is a single octet
	return encodeImplicit($class, $uConstructed, $tagNumber, $uValue, $uLength, $oc);
}

sub parseNull($$;$) {
	my ($argString, $oc, $lines) = @_;
	
	return encodeNull($oc);
}

sub parseOctetString($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $octetString_oc = 0;
	my $octetString = parseScript($lines, \$octetString_oc);

	return encodeOctetString($octetString, $octetString_oc, $oc);
}

sub parseOid($$;$) {
	my ($argString, $oc, $lines) = @_;
	$argString =~ s/\s//g;
	$argString = uc($argString);

	if (! defined $argString) {
		die "parseOid: Missing OID value.";
	}

	foreach (keys %OIDS) {
		if ($argString =~ /$_/) {
			$argString =~ s/\Q$_\E/$OIDS{$_}/g;
		}
	}
	return encodeOid($argString, $oc);
}

sub parseOutputFile($$;$) {
	my ($argString, $oc, $lines) = @_;	
	my ($outputFile,$echo) = split(/,/, $argString);
	
	if (! defined $outputFile) {
		die "parseOutputFile: Missing file-name.\n";
	}
	
	my $content_oc = 0;
	my $content = parseScript($lines, \$content_oc);

	my $outFh;
	if (! open($outFh, ">${outputFile}")) {
		die "parseOutputFile: Cannot create $outputFile\n";
	}
	binmode($outFh);
	print $outFh toBin($content);
	close $outFh;
	
	# If echo is specified then include then contents of the output 
	# file at this point in the stream.
	if (defined $echo && $echo =~ /(1|t|true)/i) {
		$$oc += $content_oc;
		return $content;		
	}
	else {
		return "";
	}
}

sub parsePrintableString($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $printableString_oc = 0;
	my $printableString = asciiToPrintableString($argString, \$printableString_oc);
	return encodePrintableString($printableString, $printableString_oc, $oc);
}

sub parsePrintableStringFile($$;$) {
	my ($binFName, $oc, $lines) = @_;
	$binFName =~ s/\s*//g;
	
	my $printableString_oc = 0;
	my $printableString = encodeBinaryFile($binFName, \$printableString_oc);	
	
	return encodePrintableString($printableString, $printableString_oc, $oc);
}

sub parseRaw($$;$) {
	my ($argString, $oc, $lines) = @_;
	$argString =~ s/\s//g;
	$argString = uc($argString);
	
	my $asnHex = "";
	if (! ($argString =~ /(([A-Fa-f\d][A-Fa-f\d])[ :]*)+/)) {
		die "parseRaw: Invalid hex string: $argString\n";
	}
	my $binary = toBin($argString);
	$$oc += length($binary);
	return tidyHex(toHex($binary));
}

sub parseSequence($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $sequence_oc = 0;
	my $sequence = parseScript($lines, \$sequence_oc);

	return encodeSequence($sequence, $sequence_oc, $oc);
}

sub parseSet($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $set_oc = 0;
	my $set = parseScript($lines, \$set_oc);

	return encodeSet($set, $set_oc, $oc);
}

# Create a PKCS#7 signed data object for a chunk of data using 
# OpenSSL's SMIME command
sub parseSign($$;$) {
	my ($argString, $oc, $lines) = @_;
	my ($signerCert, $signerKey) = getArgs($argString);

	if (! defined $signerCert) {
		die "parseSign: missing signing certificate";
	}
	elsif (! -f $signerCert) {
		die "parseSign: signing certificate \'$signerCert\' does not exist.";
	}

	if (! defined $signerKey) {
		die "parseSign: missing signing certificate";
	}
	elsif (! -f $signerKey) {
		die "parseSign: signing key \'$signerKey\' does not exist.";
	}

	my $unsigned_oc = 0;
	my $unsigned = parseScript($lines, \$unsigned_oc);

	my $unsignedFName = '_unsigned.tmp';
	my $signedFName = '_signed.tmp';

	# Create binary unsigned data file
	my $unsignedFh;
	open($unsignedFh, ">$unsignedFName") or die "Cannot create $unsignedFName";
	binmode($unsignedFh);
	print $unsignedFh toBin($unsigned);
	close $unsignedFh;

	my @command = ('openssl', 
				   'smime', 
				   '-pk7out', 
				   '-nodetach',
				   '-outform',
				   'der',
				   '-sign',
				   '-signer',
				   $signerCert,
				   '-inkey',
				   $signerKey,
				   '-in', $unsignedFName, 
				   '-out', $signedFName);

	if ($DEBUG == 1) {
		print "${TABS}:parseSign:" . join(" ", @command) . "\n";
	}

	if ((my $err = system(@command)) != 0) {
		die "parseSign: " . join(" ", @command) . "\nreturned error $err";
	}

	my $derHex = parseIncludeBinaryFile($signedFName, $oc);
	
	if (! $DEBUG) {
		unlink($unsignedFName);
		unlink($signedFName);
	}
	return $derHex;
}

sub parseShell($$;$) {
	my ($argString, $oc, $lines) = @_;
	my @command = getArgs($argString);

	if (scalar(@command) < 1) {
		die "parseShell: no arguments";
	}

	if ($DEBUG == 1) {
		print "${TABS}:parseShell:" . join(" ", @command) . "\n";
	}

	if ((my $err = system(@command)) != 0) {
		die "parseShell: " . join(" ", @command) . "\nreturned error $err";
	}
	return "";
}

sub parseUtcTime($$;$) {
	my ($time, $oc, $lines) = @_;	
	$time =~ s/\s//g;

	my $time_oc = length($time);
	return encodeUtcTime(toHex($time), $time_oc, $oc);
}

sub parseUtf8String($$;$) {
	my ($argString, $oc, $lines) = @_;	
	
	my $utf8String_oc = 0;
	my $utf8String = asciiToUtf8String($argString, \$utf8String_oc);
	return encodeUtf8String($utf8String, $utf8String_oc, $oc);
}

sub parseUtf8StringFile($$;$) {
	my ($binFName, $oc, $lines) = @_;
	$binFName =~ s/\s*//g;
	
	my $utf8String_oc = 0;
	my $utf8String = encodeBinaryFile($binFName, \$utf8String_oc);	
	
	return encodeUtf8String($utf8String, $utf8String_oc, $oc);
}

sub toHex($) {
	my ($bin) = @_;
	my $hex = unpack("H" . (length($bin) * 2), $bin);
	$hex =~ s/(..)/$1:/g;
	return $hex;
}

sub encodeBinaryFile($$) {
	my ($binFName, $oc) = @_;

	my $binFH;
	open($binFH, "$binFName") || die "encodeBinaryFile: Cannot open $binFName\n";
	binmode($binFH);

	my $binBuf;
	my $readBuf;
	my $derHex = "";
	while (my $len = sysread($binFH, $readBuf, 1024)) {
		$binBuf .= $readBuf;
		$$oc += $len;
	}
	close $binFH;	

	return toHex($binBuf);;
}

# Creates a hex representation of the DER encoding of an arbitrary length bit string
sub encodeBitString($$) {
	my ($text, $oc) = @_;

	# Bit string in hex including padding length octet
	my $bit_str = "";
	my $bit_str_oc = 1; # one octet for padding

	# Current byte
	my $byte = 0;	
	my $len = length($text);

	if ($len == 0) {
		$$oc+=2;
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
			$bit_str_oc++;
			$byte = 0;
		}
	}
	# Pad any remaining bits / make sure 0 is output for empty string
	if ($byte != 0 || $bit_str_oc == 1) {
		if ($bit_str ne "") {
			$bit_str .= ":";
		}
		$bit_str .= sprintf("%2.2x", $byte);
		$bit_str_oc++;
	}

	my $pad_length = "00";
	if ($len % 8 > 0) {
		# If this isn't a multiple of 8 bits then calculated
		# the number of padding bits added.
		$pad_length = sprintf("%2.2x", 8 - ($len % 8));
	}
	
	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodeBitString, $bit_str_oc\n";
	}
	return encodeTlv($oc, $DER_BITSTRING_TAG, $bit_str_oc, "$pad_length:$bit_str");
}

# Creates a hex represenation of the DER encoding of a BMPSTRING
sub encodeBmpString($$$) {
	my ($bmpString, $bmpString_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodeBmpString, $bmpString_oc\n";
	}
	return encodeTlv($oc, $DER_BMPSTRING_TAG, $bmpString_oc, $bmpString);
}

sub encodeBoolean($$) {
	my ($value, $oc) = @_;

	my $boolean = "00";
	if ($value) {
		$boolean = "FF";
	}

	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodeBoolean, 1\n";
	}
	return encodeTlv($oc, $DER_BOOLEAN_TAG, 1, $boolean);
}

sub encodeEnumerated($$) {
	my ($int, $oc) = @_;

	$int =~ s/\s//g;

	if (! ($int =~ /^-??\d+$/ || $int =~ /0x[0-9A-Fa-f]+/)) {
		die "encodeEnumerated: Invalid argument: $int\n";
	}
	
	if ($int =~ s/^0x//) {
		$int = hex;
	}
	
	# Convert the enumerated to base 256 hex and find out how
	# many octets were required
	my $hex_enumerated_oc = 0;
	my $hex_enumerated = "";
	
	if ($int ne "") {
		$hex_enumerated = encodeBase256($int, \$hex_enumerated_oc);
	}
		
	if ($DEBUG == 2) {
		print "${TABS}:ENC: , $hex_enumerated_oc\n";
	}	

	return encodeTlv($oc, $DER_ENUMERATED_TAG, $hex_enumerated_oc, $hex_enumerated);
}

# explicit tags are always constructed
sub encodeExplicit($$$$) {
	my ($class, $tagNumber, $explicit, $explicit_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC: explicit, $explicit_oc\n";
	}
	return encodeTlv($oc, $tagNumber, $explicit_oc, $explicit, 1, $class);
}

# Creates a hex represenation of the DER encoding of an IA5 string
sub encodeIA5String($$) {
	my ($ia5String, $ia5String_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodeIA5String, $ia5String_oc\n";
	}
	return encodeTlv($oc, $DER_IA5STRING_TAG, $ia5String_oc, $ia5String);
}

sub encodeImplicit($$$$$) {
	my ($class, $constructed, $tagNumber, $implicit, $implicit_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC: implicit, $implicit_oc\n";
	}
	return encodeTlv($oc, $tagNumber, $implicit_oc, $implicit, $constructed, $class);
}

sub encodeBigInteger($$) {
	my ($hexString, $oc) = @_;

	my $bin = toBin($hexString);
	my $int = toHex($bin);
	my $int_oc = length($bin);

	if ($DEBUG == 2) {
		print "${TABS}:ENC: bigInteger, $int_oc\n";
	}
	return encodeTlv($oc, $DER_INTEGER_TAG, $int_oc, $int)
}

sub encodeInteger($$) {
	my ($int, $oc) = @_;

	$int =~ s/\s//g;

	if (! ($int =~ /^-??\d+$/ || $int =~ /0x[0-9A-Fa-f]+/)) {
		die "encodeInteger: Invalid argument: $int\n";
	}
	
	if ($int =~ s/^0x//) {
		$int = hex;
	}
	
	# Convert the integer to base 256 hex and find out how
	# many octets were required
	my $hex_integer_oc = 0;
	my $hex_integer = "";
	
	if ($int ne "") {
		$hex_integer = encodeBase256($int, \$hex_integer_oc);
	}
		
	if ($DEBUG == 2) {
		print "${TABS}:ENC: integer, $hex_integer_oc\n";
	}	

	return encodeTlv($oc, $DER_INTEGER_TAG, $hex_integer_oc, $hex_integer);
}

sub encodeNull($) {
	my ($oc) = @_;	
	return encodeTlv($oc, $DER_NULL_TAG, 0, "");
}

sub encodeOctetString($$$) {
	my ($octetString, $octetString_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC: octetString, $octetString_oc\n";
	}
	return encodeTlv($oc, $DER_OCTETSTRING_TAG, $octetString_oc, $octetString);
}

sub encodeOid($$) {
	my ($text, $oc) = @_;

	my @fields = split /\./, $text;
	
	if (! ($fields[0] >= 0 && $fields[0] <=2) ) { 
		die "Invalid OID: $text\n";
	}
	if (! ($fields[1] >= 0 && $fields[1] <= 39) ) {
		die "Invalid OID: $text";
	}
		
	my $oid = sprintf("%2.2x", (40 * $fields[0]) + $fields[1]);
	my $oid_oc = 1;
	shift @fields;
	shift @fields;

	foreach (@fields) {		
		$oid .= ":" . encodeBase128($_, \$oid_oc);
	}

	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodeOid, $oid_oc\n";
	}
	return encodeTlv($oc, $DER_OID_TAG, $oid_oc, $oid);
}

# Creates a hex represenation of the DER encoding of a PRINTABLE string
sub encodePrintableString($$$) {
	my ($printableString, $printableString_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodePrintableString, $printableString_oc\n";
	}
	return encodeTlv($oc, $DER_PRINTABLESTRING_TAG, $printableString_oc, $printableString);
}

sub encodeSet($$$) {
	my ($set, $set_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC: set, $set_oc\n";
	}
	return encodeTlv($oc, $DER_SET_TAG, $set_oc, $set, 1);
}

sub encodeSequence($$$) {
	my ($sequence, $sequence_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC: sequence, $sequence_oc\n";
	}
	return encodeTlv($oc, $DER_SEQUENCE_TAG, $sequence_oc, $sequence, 1);
}

sub encodeUtcTime($$$) {
	my ($utcTime, $utcTime_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC: UTCTime, $utcTime_oc\n";
	}
	return encodeTlv($oc, $DER_UTCTIME_TAG, $utcTime_oc, $utcTime);
}

# Creates a hex represenation of the DER encoding of a UTF-8 string.
sub encodeUtf8String($$) {
	my ($utf8String, $utf8String_oc, $oc) = @_;

	if ($DEBUG == 2) {
		print "${TABS}:ENC:encodeUTF8String, $utf8String_oc\n";
	}
	return encodeTlv($oc, $DER_UTF8STRING_TAG, $utf8String_oc, $utf8String);
}

sub asciiToBmpString($$) {
	my ($input, $oc) = @_;

	my $bmpString = "";
	my $input_len = length($input);
	$$oc += $input_len * 2;

	for (my $i = 0; $i < $input_len; ++$i) {
		my $hex_val = ord(substr($input, $i, 1));
		if ($bmpString ne "") {
			$bmpString .= ":";
		}
		$bmpString .= sprintf(":00:%2.2x", $hex_val);
	}	
	return $bmpString;
}

sub asciiToIA5String($$) {
	my ($input, $oc) = @_;

	my $printableString = "";
	my $input_len = length($input);
	$$oc += $input_len;

	for (my $i = 0; $i < $input_len; ++$i) {
		my $hex_val = ord(substr($input, $i, 1));
		if ($printableString ne "") {
			$printableString .= ":";
		}
		$printableString .= sprintf(":%2.2x", $hex_val);
	}	
	return $printableString;
}

sub asciiToPrintableString($$) {
	my ($input, $oc) = @_;

	my $ia5String = "";
	my $input_len = length($input);
	$$oc += $input_len;

	for (my $i = 0; $i < $input_len; ++$i) {
		my $hex_val = ord(substr($input, $i, 1));
		if ($ia5String ne "") {
			$ia5String .= ":";
		}
		$ia5String .= sprintf(":%2.2x", $hex_val);
	}	
	return $ia5String;
}

sub asciiToUtf8String($$) {
	my ($input, $oc) = @_;

	my $utf8String = "";
	my $input_len = length($input);
	$$oc += $input_len;

	for (my $i = 0; $i < $input_len; ++$i) {
		my $hex_val = ord(substr($input, $i, 1));
		if ($utf8String ne "") {
			$utf8String .= ":";
		}
		$utf8String .= sprintf(":%2.2x", $hex_val);
	}	
	return $utf8String;
}

sub encodeBase128($$$) {
	my ($num, $oc) = @_;

	my $base128 = "";
	$num = int($num);
	my $base128_length = 0;

	while ($num > 0) {
		my $hexoctet;

		if ($base128 eq "") {
			$hexoctet = sprintf("%2.2x", $num & 0x7f);
		}
		else {
			$hexoctet = sprintf("%2.2x", ($num & 0x7f) | 0x80);
		}
		
		if ($base128 eq "") {			
			$base128 = $hexoctet;	   
		}
		else {
			$base128 = "$hexoctet:$base128";
		}		

		$num >>= 7;
		$base128_length++;
	}
	if ($base128 eq "") {
		$base128 = "00";
		$base128_length++;
	}

	$$oc += $base128_length;
	
	if ($DEBUG == 2) {
		print "${TABS}:ENC: base128, $base128_length, $$oc\n";
	}

	return $base128;
}

# Return a hex represenation of the length using DER primitive (definate length encoding)
sub encodeLength($$) {
	my ($num, $oc) = @_;

	if ($num < 128) {
		# Number is < 128 so encode in short form
		$$oc++;
		return sprintf("%2.2x", $num);
	}
	else {
		# Number >= 128 so encode in long form
		my $length_oc = 0;
		my $base256 = &encodeBase256($num, \$length_oc, 1);
		if ($length_oc > 127) {die "Encoding overflow.";}
		
		$$oc += 1 + $length_oc;
		
		# Set the top bit of the length octet to indicate long form		
		return "" . sprintf("%2.2x", ($length_oc | 0x80)) . ":$base256";
	}
}

# Convert an integer into an ascii hex representation in base 256
# $num    - the number to encode
# $octets - refernce to the octet count to increment
# $unsigned - assume unsigned
sub encodeBase256($$) {
	my ($numIn, $oc, $unsigned) = @_;

	my $base256 = "";
	my $num = int($numIn);	

	while ($num != 0) {
		my $hexoctet = sprintf("%2.2x", $num & 0xFF);
		if ($base256 ne "") {
			$base256 = "$hexoctet:$base256";
		}
		else {
			$base256 = $hexoctet;
		}		
		$num >>= 8;
		$$oc++;
	}
	if ($base256 eq "") {
		$base256 = "00";
		$$oc++;
	}

	# If the integer is +ve and the MSB is 1 then padd with a leading zero 
	# octet otherwise it will look -ve
	if ((! $unsigned) && $numIn > 0 && $base256 =~ /^:*[8ABCDEF]/i) {
		$base256 = "00:$base256";
		$$oc++;
	}

	# If the first octet is all ones and the msb of the next bit
	# is also one then drop the first octet because negative
	# numbers should not be padded
	while ($base256 =~ s/^(FF:)([8ABCDEF][0-9A-F].*)/$2/i) {
		$$oc--;
	}

	return $base256;
}

# Encode the Type
# Only low tag form is supported at the moment
sub encodeType($$;$$) {
	my ($oc, $tagNumber, $constructed, $class) = @_;

	$tagNumber = hex($tagNumber);

	if ($tagNumber < 0 || $tagNumber > 30) {
		die "encodeType: Currently, only low tag numbers (0 - 30) are supported.";
	}

	if (! defined $class) {
		$class = "UNIVERSAL";
	}
	
	$class = uc($class);	
	if (! isValidClass($class)) {
		die "encodeType: invalid class \'$class\'";
	}   

	# If the type is constructed then set bit 6
	if (defined $constructed && $constructed == 1) {
		$tagNumber |= 0x20;
	}

	if ($class eq $UNIVERSAL_CLASS) {
	   # do nothing, bits 7 and 8 are zero
	}
	elsif ($class eq $APPLICATION_CLASS) {
		# set bit 7
		$tagNumber |= 0x40;
	}
	elsif ($class eq $CONTEXT_SPECIFIC_CLASS) {
		# set bit 8
		$tagNumber |= 0x80;
	}
	elsif ($class eq $PRIVATE_CLASS) {
		# set bits 7 and 8
		$tagNumber |= 0xC0;
	}
	$$oc++;
	return sprintf("%2.2x", $tagNumber);
}

sub encodeTlv($$$$;$$) {
	my ($oc, $tag, $length, $value, $constructed, $class) = @_;

	if ($DEBUG == 3) {
		print "${TABS}encodeTlv\n";
		print "${TABS}oc=$$oc\n";
		print "${TABS}tag=$tag\n";
		print "${TABS}length=$length\n";
		print "${TABS}value=$value\n";
		if (defined $constructed) {
			print "${TABS}constructed=$constructed\n";
		}
		if (defined $class) {
			print "${TABS}class=$class\n";
		}
	}

	my $hex;
	$hex = encodeType($oc, $tag, $constructed, $class);
	$hex .= ":" . encodeLength($length, $oc);
	$$oc += $length;
	$hex .= ":" . $value;

	if ($DEBUG == 3) {
		print "${TABS}oc=$$oc\n";
		print "${TABS}encoding=$hex\n";
		print "${TABS}end\n";

		toBin($hex);
	}
	return $hex;
}

# increment debug tabbing level
sub nest() {
	$TABS .= "   ";
}

# decrement debug tabbing level
sub leaveNest() {
	$TABS =~ s/^...//;
}

sub isValidClass($) {
	my ($class) = @_;

	if (defined $class &&
		$class =~ /^(UNIVERSAL|APPLICATION|CONTEXT-SPECIFIC|PRIVATE)$/) {
		return 1;
	}
	return 0;
}

# Parse a DER field
sub getTlv($$$$$$) {
	my ($input, $class, $constructed, $tag, $length, $value) = @_;
	
	my @hexOctets = split(/:+/,tidyHex($input));
	
	if (scalar(@hexOctets) < 2) {
		die "getTlv: too short";
	}

	my $type = hex(shift @hexOctets);
	if (($type & 0xC0) == 0x00) {
		# universal: bit 8 = 0, bit 7 = 0
		$$class = $UNIVERSAL_CLASS;
	}
	elsif (($type & 0xC0) == 0x40) {
		# application: bit 8 = 0, bit 7 = 1
		$$class = $APPLICATION_CLASS;
	}
	elsif (($type & 0xC0) == 0x80) {
		# application: bit 8 = 1, bit 7 = 0
		$$class = $CONTEXT_SPECIFIC_CLASS;
	}
	elsif (($type & 0xC0) == 0xC0) {
		# application: bit 8 = 1, bit 7 = 1
		$$class = $PRIVATE_CLASS;
	}
	else {
		die "getTlv: assert";
	}

	if ($type & 0x20) {
		# constructed if bit 6 = 1
		$$constructed = 1;
	}
	else {
		$$constructed = 0;
	}
	
	# We assumme the tag number is in low form
	# and just look at the bottom 5 hits
	$$tag = $type & 0x1F;

	$$length = hex(shift @hexOctets);
	if ($$length & 0x80) {
		# long form
		my $length_oc = $$length & 0x7F;
		$$length = 0;
		for (my $i = 0; $i < $length_oc; $i++) {
			# length is encoded base 256
			$$length *= 256;
			$$length += hex(shift @hexOctets);
		}
	}
	else {
		# short form
		# don't do anything here, length is just bits 7 - 1 and 
		# we already know bit 8 is zero.
	}

	$$value = "";
	foreach (@hexOctets) {
		$$value .= ":$_";
	}

	if ($DEBUG == 3) {
		print "${TABS} class=$$class\n";
		print "${TABS} constructed=$$constructed\n";
		print "${TABS} tag=$$tag\n";
		print "${TABS} length=$$length\n";
	}
}

# parse an escaped (\) comma seperated argument string
# into an array
sub getArgs($) {
	my ($argString) = @_;
	my @args = ();
	
	while ($argString =~ /(^|.*?[^\\]),(.*)/ ) {
		my $match = $1;
		$argString = $2;
		if ($match ne "") {
			
			# unescape
			$match =~ s/(\\)([^\\])/$2/g;
			push @args, $match;
		}
	}
	if ($argString ne "") {
		push @args, $argString;
	}
    return @args;
}
