#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This script was written as part the solution for DEF116697: Remove Security Test Certificates from CBR 
# The purpose of the defect was to stop the export of all test certificates that may not be Symbian owned.
# To accomplish this the certificates were all moved to a new location which does not get put in the CBR.
# This script is run in the ONB so that no new certificates are added to any directory that appears in the CBR.
# (Note that the certificates in rootcerts are Symbian owned and so can be exported.)
# This script takes 2 arguments
# - directory to search for certificates (defaults to \common\generic\security)
# - output file for result of test (defaults to $ENV{EPOCROOT}epoc32\\winscw\\c\\CheckLocationOfCertificatesLog.txt)
# The script searches through the specified directory for any certificate files (files ending in .cer, .der and .crt).
# It will print out the names of any files found. 
#
 

use File::Find;

# array holding the list of full path names to all the certificates found.
@Certificates;
 
 
sub FindCerts
{
	# Check for certificates which are not in valid locations 
	if (($File::Find::dir !~ m/\/testframework\/testcertificates/) && ($File::Find::dir !~ m/\/os\/security\/cryptoservices\/rootcertificates/) && ($File::Find::dir !~ m/\/os\/security\/cryptomgmtlibs\/securitytestfw\/testcertificates/))
	{	
		if ($File::Find::name =~ m/\.cer$/i)
		{
			push @Certificates, $File::Find::name;	 
		}
		if ($File::Find::name =~ m/\.crt$/i)
		{
			push @Certificates, $File::Find::name;	 
		}
		if ($File::Find::name =~ m/\.der$/i)
		{
			push @Certificates, $File::Find::name;
		}
		if ($File::Find::name =~ m/\.pem$/i)
		{
			push @Certificates, $File::Find::name;
		}	
	}
	
}

 

# Determine directory to search  
my $dirToSearch;
if (@ARGV[0])
	{
	$dirToSearch = $ARGV[0];
	}
else
	{
	$dirToSearch = "$ENV{'SECURITYSOURCEDIR'}";
	}

# Determine where to put the logs. This file will be parsed by the overnight build system.
my $outputFile;
if (@ARGV[1])
	{
	$outputFile = $ARGV[1];
	}
else
	{
	die "EPOCROOT not defined, must specify directory" if !defined ($ENV{EPOCROOT});
	my $emulatorLogDirectory = "$ENV{EPOCROOT}logs\\winscw\\c";

	if ( ! -d $emulatorLogDirectory )
		{
		system("md $ENV{EPOCROOT}logs\\winscw\\c");
		}
		$outputFile = "$ENV{EPOCROOT}epoc32\\winscw\\c\\checklocationofcertificateslog.txt";
	}

unlink $outputFile;
die "\nUnable to open log $outputFile\n" if( not open( SCANLOG, ">$outputFile" ) );


print SCANLOG "\nScanning $dirToSearch for incorrectly located certificate files.\n\n";


# Search for certificate files
find { wanted => \&FindCerts, no_chdir => 1 }, $dirToSearch;

my $count = scalar(@Certificates);
 
if ($count eq 0)
{
 	print (SCANLOG "No certificates found in $dirToSearch. Test PASSED.\n\n");
 	print (SCANLOG "\nTests completed OK");
 	print (SCANLOG "\nRun: 1");
 	print (SCANLOG "\nPassed: 1");	
	print (SCANLOG "\n0 tests failed out of 1"); 
} 
else 
	{	
	foreach $certificatefile (@Certificates)
		{
		$certificatefile =~ s/\//\\/g;
		print (SCANLOG "Certificate: $certificatefile is in an invalid location. Should be moved to ......\\security\\testframework\\testcertificates\\...\n");
		print (SCANLOG "Test for $certificatefile FAILED.\n\n");
		}
		print (SCANLOG "\nTests completed OK");
 		print (SCANLOG "\nRun: 1");
 		print (SCANLOG "\nPassed: 0");	
		print (SCANLOG "\n1 tests failed out of 1"); 
	}
 
close(SCANLOG);
 
 
 
