#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Generate certs for testing OCSP against OpenSSL implementation
#


# address of the Root5 responder, needed for Authority Info Access cert
my $ocspR5addr = "http://cam-ocsptest01.intra:19003"; 

# Address of the Apache server used for serving remote JAR files
my $apacheaddr = "cam-ocsptest01.intra";


sub head(@)
{
	my $src=shift(@_);
	my $dest=shift(@_);
	my $lines=shift(@_);

	open(IN,"< $src")  or print("Can't open $src");
	open(OUT,"> $dest") or print("Can't open $dest");
	 
	for (my $count=0; $count < $lines; $count++)
	{
		my $fline=<IN>;
		print OUT $fline;
	}
	close(IN);
	close(OUT);
}

sub createfile(@)
{
	open(TOUCH,">shift(@_)");
	close(TOUCH);
}

# Create ca files
sub mkcadirs(@)
{
	my $cadir=shift(@_);

	unlink($cadir);
	mkdir($cadir);
	open(TOUCH,">$cadir\\index.txt");
	close(TOUCH);
	system("echo 01 > $cadir\\serial");
	mkdir "$cadir\\private" ;
	mkdir "$cadir\\certs";
}





# Trash existing data

use File::Path;

rmtree ("Root1",0,true);
rmtree ("Root2",0,true);
rmtree ("Root5",0,true);
rmtree ("OCSPSigningRoot",0,true);
rmtree ("Apache",0,true);
rmtree ("Certs",0,true);
mkdir "Certs";

	 

#############################################################
## OCSP	Certificates for testing
##
## There are three roots for testing OCSP, Root1, Root2 and Root5
##
#############################################################



# Root1 ##########################################################################

mkcadirs("Root1");
$cert_path = "$ENV{'SECURITYSOURCEDIR'}\\testframework\\testcertificates\\certman\\testcertificates";
  
# Generate root cert
system("openssl req -extensions NoOCSP_Ext -config openssl.config -x509 -newkey rsa:1024 -keyout Root1\\private\\ca.key.pem -out Root1\\Certs\\ca.pem -subj \"/O=Symbian Software Ltd/CN=Root1-RSA\" -days 3650 -nodes"); 
system("openssl x509 -in $cert_path\\openssl\\Root1\\Certs\\ca.pem -outform DER -out $cert_path\\openssl\\Certs\\Root1-RSA.der");

# Generate Expired-R1
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root1\\private\\Expired-R1.key.pem -out Root1\\Expired-R1.req.pem -subj \"/O=Symbian Software Ltd/CN=Expired-R1\" -days 3650 -nodes");
system("openssl ca  -out $cert_path\\openssl\\Root1\\Certs\\Expired-R1.pem -config openssl.config -name Root1 -in $cert_path\\openssl\\Root1\\Expired-R1.req.pem -batch -startdate 820203120000Z -enddate 820203120001Z");
system("openssl x509 -in $cert_path\\openssl\\Root1\\Certs\\Expired-R1.pem -outform DER -out $cert_path\\openssl\\Certs\\Expired-R1.der");

# Generate Good-R1
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root1\\private\\Good-R1.key.pem -out $cert_path\\openssl\\Root1\\Good-R1.req.pem -subj \"/O=Symbian Software Ltd/CN=Good-R1\" -days 3650 -nodes");
system("openssl ca  -config openssl.config -out $cert_path\\openssl\\Root1\\Certs\\Good-R1.pem -name Root1 -in Root1\\Good-R1.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root1\\Certs\\Good-R1.pem -outform DER -out $cert_path\\openssl\\Certs\\Good-R1.der");

# Generate Revoked-R1
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root1\\private\\Revoked-R1.key.pem -out $cert_path\\openssl\\Root1\\Revoked-R1.req.pem -subj \"/O=Symbian Software Ltd/CN=Revoked-R1\" -days 3650 -nodes");
system("openssl ca  -out $cert_path\\openssl\\Root1\\Certs\\Revoked-R1.pem -config openssl.config -name Root1 -in $cert_path\\openssl\\Root1\\Revoked-R1.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root1\\Certs\\Revoked-R1.pem -outform DER -out $cert_path\\openssl\\Certs\\Revoked-R1.der");
system("openssl ca -config openssl.config -name Root1 -revoke $cert_path\\openssl\\Root1\\Certs\\Revoked-R1.pem -crl_reason keyCompromise");

# Generate Unknown-R1
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root1\\private\\Unknown-R1.key.pem -out $cert_path\\openssl\\Root1\\Unknown-R1.req.pem -subj \"/O=Symbian Software Ltd/CN=Unknown-R1\" -days 3650 -nodes");
system("openssl ca  -out $cert_path\\openssl\\Root1\\Certs\\Unknown-R1.pem -config openssl.config -name Root1 -in $cert_path\\openssl\\Root1\\Unknown-R1.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root1\\Certs\\Unknown-R1.pem -outform DER -out $cert_path\\openssl\\Certs\\Unknown-R1.der");

# remove Unknown-R1 from the CA
rename("Root1\\index.txt","Root1\\index.txt.new");
head("Root1\\index.txt.new","Root1\\index.txt",3);
unlink("Root1\\Certs\\Unknown-R1.pem.pem");
unlink("Root1\\index.txt.new");

# Generate OCSPSigner-R1
system("openssl req -extensions NoOCSP_Ext -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root1\\private\\OCSPSigner-R1.key.pem -out $cert_path\\openssl\\Root1\\OCSPSigner-R1.req.pem -subj \"/O=Symbian Software Ltd/CN=OCSPSigner-R1\" -days 3650 -nodes");
system("openssl ca  -extensions NoOCSP_Ext -config openssl.config -name $cert_path\\openssl\\Root1 -in $cert_path\\openssl\\Root1\\OCSPSigner-R1.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root1\\Certs\\05.pem -outform DER -out $cert_path\\openssl\\Certs\\OCSPSigner-R1.der");


# Root2 ##########################################################################

mkcadirs("Root2");


system("openssl dsaparam -out Root2\\dsaparam.pem 1024");
my $keyParams= "-newkey dsa:Root2\\dsaparam.pem";

# my $keyParams="-newkey rsa:1024";


  
# Generate root cert
system("openssl req -extensions NoOCSP_Ext -config openssl.config -x509 $keyParams -keyout $cert_path\\openssl\\Root2\\private\\ca.key.pem -out $cert_path\\openssl\\Root2\\Certs\\ca.pem -subj \"/O=Symbian Software Ltd/CN=Root2-DSA\" -days 6000 -nodes"); 
system("openssl x509 -in $cert_path\\openssl\\Root2\\Certs\\ca.pem -outform DER -out $cert_path\\openssl\\Certs\\Root2-DSA.der");

# Generate Expired-R2
system("openssl req -config openssl.config $keyParams -keyout $cert_path\\openssl\\Root2\\private\\Expired-R2.key.pem -out $cert_path\\openssl\\Root2\\Expired-R2.req.pem -subj \"/O=Symbian Software Ltd/CN=Expired-R2\" -days 3650 -nodes");
system("openssl ca  -config openssl.config -name Root2 -in $cert_path\\openssl\\Root2\\Expired-R2.req.pem -batch -out $cert_path\\openssl\\Root2\\Certs\\Expired-R2.pem -startdate 820203120000Z -enddate 820203120001Z");
system("openssl x509 -in Root2\\Certs\\Expired-R2.pem -outform DER -out $cert_path\\openssl\\Certs\\Expired-R2.der");

# Generate Good-R2
system("openssl req -config openssl.config $keyParams -keyout $cert_path\\openssl\\Root2\\private\\Good-R2.key.pem -out $cert_path\\openssl\\Root2\\Good-R2.req.pem -subj \"/O=Symbian Software Ltd/CN=Good-R2\" -days 3650 -nodes");
system("openssl ca  -config openssl.config -name Root2 -in $cert_path\\openssl\\Root2\\Good-R2.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root2\\Certs\\02.pem -outform DER -out $cert_path\\openssl\\Certs\\Good-R2.der");

# Generate Revoked-R2
system("openssl req -config openssl.config $keyParams -keyout $cert_path\\openssl\\Root2\\private\\Revoked-R2.key.pem -out $cert_path\\openssl\\Root2\\Revoked-R2.req.pem -subj \"/O=Symbian Software Ltd/CN=Revoked-R2\" -days 3650 -nodes");
system("openssl ca  -config openssl.config -name $cert_path\\openssl\\Root2 -in $cert_path\\openssl\\Root2\\Revoked-R2.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root2\\Certs\\03.pem -outform DER -out $cert_path\\openssl\\Certs\\Revoked-R2.der");
system("openssl ca -config openssl.config -name Root2 -revoke Root2\\Certs\\03.pem -crl_reason keyCompromise");

# Generate Unknown-R2
system("openssl req -config openssl.config $keyParams -keyout $cert_path\\openssl\\Root2\\private\\Unknown-R2.key.pem -out Root2\\Unknown-R2.req.pem -subj \"/O=Symbian Software Ltd/CN=Unknown-R2\" -days 3650 -nodes");
system("openssl ca  -config openssl.config -name Root2 -in $cert_path\\openssl\\Root2\\Unknown-R2.req.pem -batch -days 3650");
system("openssl x509 -in Root2\\Certs\\04.pem -outform DER -out $cert_path\\openssl\\Certs\\Unknown-R2.der");

# remove Unknown-R2 from the CA
rename("Root2\\index.txt","Root2\\index.txt.new");
head("Root2\\index.txt.new","Root2\\index.txt",3);
unlink("Root2\\Certs\\04.pem");


# Generate OCSPSigner-R2
system("openssl req -extensions NoOCSP_Ext -config openssl.config $keyParams -keyout $cert_path\\openssl\\Root2\\private\\OCSPSigner-R2.key.pem -out $cert_path\\openssl\\Root2\\OCSPSigner-R2.req.pem -subj \"/O=Symbian Software Ltd/CN=OCSPSigner-R2\" -days 6000 -nodes");
system("openssl ca  -extensions NoOCSP_Ext -config openssl.config -name $cert_path\\openssl\\Root2 -in $cert_path\\openssl\\Root2\\OCSPSigner-R2.req.pem -batch -days 6000");
system("openssl x509 -in $cert_path\\openssl\\Root2\\Certs\\05.pem -outform DER -out $cert_path\\openssl\\Certs\\OCSPSigner-R2.der");


# Root5 ##########################################################################

mkcadirs("Root5");
  
# Generate root cert
system("openssl req -extensions Root5_Root_Ext -config openssl.config -x509 -newkey rsa:1024 -keyout Root5\\private\\ca.key.pem -out Root5\\Certs\\ca.pem -subj \"/O=Symbian Software Ltd/CN=Root5-RSA\" -days 3650 -nodes"); 
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\ca.pem -outform DER -out $cert_path\\openssl\\Certs\\Root5-RSA.der");

# Generate Expired-R5
system("openssl req  -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\Expired-R5.key.pem -out Root5\\Expired-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=Expired-R5\" -days 3650 -nodes");
system("openssl ca  -extensions Root5_Ext -config openssl.config -name Root5 -in Root5\\Expired-R5.req.pem -batch -startdate 820203120000Z -enddate 820203120001Z");
system("openssl x509 -in Root5\\Certs\\01.pem -outform DER -out $cert_path\\openssl\\Certs\\Expired-R5.der");

# Generate Good-R5
system("openssl req -config openssl.config -newkey rsa:1024 -keyout Root5\\private\\Good-R5.key.pem -out Root5\\Good-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=Good-R5\" -days 3650 -nodes");
system("openssl ca  -extensions Root5_Ext -config openssl.config -name Root5 -in Root5\\Good-R5.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\02.pem -outform DER -out $cert_path\\openssl\\Certs\\Good-R5.der");

# Generate Revoked-R5
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\Revoked-R5.key.pem -out $cert_path\\openssl\\Root5\\Revoked-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=Revoked-R5\" -days 3650 -nodes");
system("openssl ca  -extensions Root5_Ext -config openssl.config -name Root5 -in $cert_path\\openssl\\Root5\\Revoked-R5.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\03.pem -outform DER -out $cert_path\\openssl\\Certs\\Revoked-R5.der");
system("openssl ca -config openssl.config -name Root5 -revoke $cert_path\\openssl\\Root5\\Certs\\03.pem -crl_reason keyCompromise");

# Generate Unknown-R5
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\Unknown-R5.key.pem -out $cert_path\\openssl\\Root5\\Unknown-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=Unknown-R5\" -days 3650 -nodes");
system("openssl ca  -extensions Root5_Ext -config openssl.config -name Root5 -in $cert_path\\openssl\\Root5\\Unknown-R5.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\04.pem -outform DER -out $cert_path\\openssl\\Certs\\Unknown-R5.der");

# remove Unknown-R5 from the CA
rename("Root5\\index.txt","Root5\\index.txt.new");
head("Root5\\index.txt.new","Root5\\index.txt",3);
unlink("$cert_path\\openssl\\Root5\\Certs\\04.pem");


# Generate Mid-R5
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\Mid-R5.key.pem -out $cert_path\\openssl\\Root5\\Mid-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=Mid-R5\" -days 3650 -nodes");
system("openssl ca  -extensions Root5_Mid -config openssl.config -name Root5 -in $cert_path\\openssl\\Root5\\Mid-R5.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\05.pem -outform DER -out $cert_path\\openssl\\Certs\\Mid-R5.der");

use File::Copy;

copy("$cert_path\\openssl\\Root5\\private\\ca.key.pem","$cert_path\\openssl\\Root5-Mid\\Private\\Mid-R5.key.pem");
copy("$cert_path\\openssl\\Root5\\Certs\\05.pem","$cert_path\\openssl\\Root5-Mid\\Certs\\Mid-R5.pem");

system("openssl req -config openssl.config  -extensions Root5_Mid_EE -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\Good-M5.key.pem -out $cert_path\\openssl\\Root5\\Good-M5.req.pem -subj \"/O=Symbian Software Ltd/CN=Good-M5\" -days 3650 -nodes");
system("openssl x509 -extfile Good-M5.extensions -req -in $cert_path\\openssl\\Root5\\Good-M5.req.pem -CA $cert_path\\openssl\\Root5\\certs\\05.pem -CAkey $cert_path\\openssl\\Root5\\private\\Mid-R5.key.pem -out $cert_path\\openssl\\Root5\\private\\Good-M5.cert.pem -CAserial Root5\\serial");
system("openssl x509 -in Root5\\private\\Good-M5.cert.pem -outform DER -out $cert_path\\openssl\\Certs\\Good-M5.der");


# Generate OCSPSigner-R5
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\OCSPSigner-R5.key.pem -out $cert_path\\openssl\\Root5\\OCSPSigner-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=OCSPSigner-R5\" -days 6000 -nodes");
system("openssl ca  -extensions NoOCSP_Ext -config openssl.config -name $cert_path\\openssl\\Root5 -in $cert_path\\openssl\\Root5\\OCSPSigner-R5.req.pem -batch -days 6000");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\07.pem -outform DER -out $cert_path\\openssl\\Certs\\OCSPSigner-R5.der");


#Generate GoodAIA-R5 cert, server specified in cert extension
open(AIAEXT,">GoodAIA-R5.extension");
print AIAEXT "authorityInfoAccess = OCSP;URI:$ocspR5addr";
close(AIAEXT);

system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\GoodAIA-R5.key.pem -out $cert_path\\openssl\\Root5\\GoodAIA-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=GoodAIA-R5\" -days 3650 -nodes");
system("openssl ca  -extfile GoodAIA-R5.extension -config openssl.config -name Root5 -in $cert_path\\openssl\\Root5\\GoodAIA-R5.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\08.pem -outform DER -out $cert_path\\openssl\\Certs\\GoodAIA-R5.der");

unlink "GoodAIA-R5.extension";			


# Apache certificate stuff

mkdir("Apache");


#Generate Apache-R5 cert, certificate used for SSL on apache server
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\Root5\\private\\Apache-R5.key.pem -out Root5\\Apache-R5.req.pem -subj \"/O=Symbian Software Ltd/CN=$apacheaddr\" -days 6000 -nodes");
system("openssl ca  -config openssl.config -name Root5 -in $cert_path\\openssl\\Root5\\Apache-R5.req.pem -batch -days 6000");
system("openssl x509 -in $cert_path\\openssl\\Root5\\Certs\\09.pem -outform DER -out $cert_path\\openssl\\Certs\\Apache-R5.der");

copy("Root5\\Certs\\09.pem","$cert_path\\openssl\\Apache\\Apache-R5.pem");
copy("Root5\\private\\$cert_path\\openssl\\Apache-R5.key.pem","$cert_path\\openssl\\Apache\\Apache-R5.key.pem");

copy("$cert_path\\openssl\\Root5\\Certs\ca.pem","$cert_path\\openssl\\Apache\\Root5-RSA.pem");


# OCSPSigningRoot ##########################################################################

mkcadirs("OCSPSigningRoot");
  
# Generate root cert
system("openssl req -config openssl.config -x509 -newkey rsa:1024 -keyout $cert_path\\openssl\\OCSPSigningRoot\\private\\ca.key.pem -out $cert_path\\openssl\\OCSPSigningRoot\\Certs\\ca.pem -subj \"/O=Symbian Software Ltd/CN=OCSPSigningRoot-RSA\" -days 3650 -nodes"); 
system("openssl x509 -in $cert_path\\openssl\\OCSPSigningRoot\\Certs\\ca.pem -outform DER -out $cert_path\\openssl\\Certs\\OCSPSigningRoot-RSA.der");

# Generate Signer-OCSPR
system("openssl req -config openssl.config -newkey rsa:1024 -keyout $cert_path\\openssl\\OCSPSigningRoot\\private\\Signer-OCSPR.key.pem -out $cert_path\\openssl\\OCSPSigningRoot\\Signer-OCSPR.req.pem -subj \"/O=Symbian Software Ltd/CN=Signer-OCSPR\" -days 3650 -nodes");
system("openssl ca  -config openssl.config -name OCSPSigningRoot -in $cert_path\\openssl\\OCSPSigningRoot\\Signer-OCSPR.req.pem -batch -days 3650");
system("openssl x509 -in $cert_path\\openssl\\OCSPSigningRoot\\Certs\\01.pem -outform DER -out $cert_path\\openssl\\Certs\\Signer-OCSPR.der");

