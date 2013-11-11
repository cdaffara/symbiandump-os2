/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Good/Supportted values for labels
*
*/



const char *goodLabels[]=
{
  	"\"SymbianA\"",
  	"\"SymbianB\"",
  	"\"SymbianC\"",
  	"\"SymbianD\"",
  	"\"Label With Spaces\"",
  	"\"label with a quote \\\" char in it\"",   
  	//"\"label with backslash \\\\ char in it \"",
	0                       //null termination
};


const char *goodCert_array[]= 
{
  	"\"Root5CA\"",
  	"\"SymbianTestDSACA\"",
  	"\"SymbianTestRSACA\"",
  	"\"0\"",
  	"\"Label With Spaces\"",
  	"\"label with a quote \\\" char in it\"",   
  	//"\"label with backslash \\\\ char in it \"",
	0
};



//good deletables
const char *goodDeletables[]=
{
	"\"true\"",
	"\"false\"",
	"\"0\"",
	"\"1\"",
	"\"ETrue\"",
	"\"EFalse\"",
	0
};


const char *goodEmuCert_array[]= 
{
	"Root5CA",
	"SymbianTestDSACA",
	"SymbianTestRSACA",
	"0",
	"Label With Spaces",
	"label with a quote \" char in it",   
	//"label with backslash \\\\ char in it ",
	0
};


const char *goodSwiCert_array[]=
{
  	"SymbianA",
  	"SymbianB",
  	"SymbianC",
  	"SymbianD",
  	"Label With Spaces",
  	"label with a quote \" char in it",   
  	//"label with backslash \\ char in it ",
	0                       //null termination
};


const char *goodFormats[]=
{
	"EX509Certificate",
	"EWTLSCertificate", // if specified as WTLS then subject key id is mandatory
	"0",
	0
};
		
const char *goodCertOwnerTypes[]=
{
	"\"ECACertificate\"",
	"\"EUserCertificate\"",
	"\"EPeerCertificate\"",
	"0",
	0
};
		
const char *goodSubjectKeyIds[]=
{
	"auto",
	"''", 
	"'53:CC:80:C1:A2:EA:9C:94:17:B3:DE:38:44:63:64:E4:4D:99:2B:74'",
	0
};
		
const char *goodIssuerKeyIds[] =
{
	"auto",
	"''", 
	"'53:CC:80:C1:A2:EA:9C:94:17:B3:DE:38:44:63:64:E4:4D:99:2B:74'",
	0
};

const char *goodApplications[] = 
{
	"\"SW Install\"",
	"\"0x1000a8b6\"",
	0
};

//good trusted values
const char *goodTrusteds[] =
{
	"\"true\"",
	"\"false\"",
	"0",
	"1",
	0
};

//for swi cert store
const char *goodCapabilitySets[]=
{
	"TCB",
	"CommDD",
	"PowerMgmt",
	"MultimediaDD",
	"ReadDeviceData",
	"WriteDeviceData",
	"DRM",
	"TrustedUI",
	"ProtServ",
	"DiskAdmin",
	"NetworkControl",
	"AllFiles",
	"SwEvent",
	"NetworkServices",
	"LocalServices",
	"ReadUserData",
	"WriteUserData",
	"Location",
	"SurroundingsDD",
	"UserEnvironment",
	0
};

//good mandatory
const char *goodMandatory[]=
{
	"\"true\"",
	"\"false\"",
	"0",
	"1",
	0
};

//good system upgrade
const char *goodSystemUpgrade[]=
{
	"\"true\"",
	"\"false\"",
	"0",
	"1",
	0
};


//for cacerts file
const char *goodUids[]=
{
	"0x100042ab",	  //SW Install
	"0x1000a8b6",  // SW install Ocsp Signing
	"0x101f9b28",  // Midlet Installation
	"0x1000183d",   //Server Authentication
	0
};

// for cert client store
const char *goodAppNames[]=
{
	"\"SW Install\"",
	"\"SW install Ocsp Signing\"",
	"\"Midlet Installation\"",
	"\"Server Authentication\"",
	0
};

const char *goodActionType[]=
{
	"init",
	"listcert",
	0
};


const char *goodOwnerType[]=
{
	"ca",
	"User",
	"Peer",
	"0",
	0
};

//Filter type required for rcsa
const char *goodRcsaFilterType[]=
{
	"Root5CA",
	"X509",
	"ca",
	"53cc80c1a2ea9c9417b3de38446364e44d992b74",
	"549",
	"1",
	0
};


// applications names used in the store
const char *goodcertclient_array[]=
{
	"SW Install",
	"SW install Ocsp Signing",
	"Midlet Installation",
	"Server Authentication",
	0
};


//hex array of uid 
const char *gooduid_array[]= 
{
	"0x100042ab", //uid of SWInstall
	"0x1000a8b6", //Uid of SW Install OCSP Signing
	"0x101f9b28", //Uid of Midlet Installation
	"0x1000183d"  //Uid of Server Authentication
};

// decimal arry of uid
const char *gooddecimalUid_array[]= 
{
	"268452523", //uid of SWInstall
	"268478646", //Uid of SW Install OCSP Signing
	"270506792", //Uid of Midlet Installation
	"268441661"  //Uid of Server Authentication
};

const char *goodTrust[]=
{
	"1",
	"0"
};

const char *mandatory[]=
{
	"ETrue",
	"EFalse",
	"0",
	"1"
};

const char *systemUpgrade[]=
{
	"ETrue",
	"EFalse",
	"1",
	"0"
};

const char *deletable[]=
{
	"ETrue",
	"EFalse",
	"0",
	"1"
};

// End of file

	
