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
* bad Labels
*
*/


const char *badLabels[]=
{
	"\"LetsTryLabelNameGreaterThen64CharactersToTestTheCertAppToolSymbianA\"", // >64 bytes
	"\" \"",        //no name in double quotes
	"\"!@#$%%^&\"", //junk value
	"   ",          // just empty space
	0
};

//bad deletables
const char *badDeletables[]=
{
	"\" \"",   //no value
	"\"!@#\"", //junk value
	"Zero",       //zero val
	0
};

//bad formats
const char *badFormats[]=
{
	"EUnknownCertificate", //unsupported
	"EX968CertificateUrl", //unsupported
	"EWTLSCertificateUrl", //unsupported
	" ",				   //unsupported
	0
};

//bad owner type
const char *badCertOwnerTypes[]=
{
	"\" \"",      //no name
	"\"@$@#$\"",  //junk value
	"",           //empty 
	0
};

//bad subject key id
const char *badSubjectKeyIds[]=
{
	" ",
	"@%#%$%",
	"'3333CC:80:C1:A2:323EA::::64:E4:4D:99:2B:74'",
	0
};

//bad issuer key id
const char *badIssuerKeyIds[] =
{
	" ",
	"@%#%$%",
	"'3333CC:80:C1:A2:323EA::::64:E4:4D:99:2B:74'",
	0
};

//bad application
const char *badApplications[] = 
{
	"\"Install\"",
	"\"xxxxxxxx\"",
	0
};

//bad trusted
const char *badTrusteds[] =
{
	"",
	"\"$@#$@\"",
	"\"trust\"",
	0
};

//bad array for certclient
const char *badCert_array[]= 
{
	"\" \"",
	"\"@#$#%%\"",
	"0",
	0
};

//bad array for cacerts
const char *bademu_cacerts_array[]=
{
	"\" \"",
	"\"@#$#%%\"",
	"0",
	0
};
	
//bad capabilites
const char *badCapabilitySets[]=
{
	"TTCB",
	"ProtttServ",
	"@$%%#$%$#",
	"XXXXXXX",
	" ",
	0
};

const char *capabilitySets[]=
{
	"TCB",	
	"CommDD",
	"PowerMgmt",
	"MultimediaDD",
	"ReadDeviceData",
	0
};

//bad mandatory values
const char *badMandatory[]=
{
	"\" \"",
	"\"@#@$#$\"",
	" ",
	0
};

//bad system upgrad values
const char *badSystemUpgrade[]=
{
	"\" \"",
	"\"@#@$#$\"",
	" ",
	0
};

//for cacerts file
const char *badUids[]=
{
	"0xxxxxxxxx",	  //SW Install
	"0x@$##%#$%",  // SW install Ocsp Signing
	"          ",   //Server Authentication
	"AlphabetEn",
	0
};


const char *appNames[]=
{
	"\"SW Install\"",
	"\"SW install Ocsp Signing\"",
	"\"Midlet Installation\"",
	"\"Server Authentication\"",
	0
};

//bad action names for the emulator tests
const char *badActionName[]=
{
	"TEST",
	"$#%$#%&*",
	" ",
	0
};

//bad action types 
const char *badActionType[]=
{
	"illegal name",
	"@#$%&*",
	" ",
	0
};

//bad owner type
const char *badOwnerType[]=
{
	"Test",
	"@#$%&*",
	"",
	0
};

//bad rcsa filter type
const char *badRcsaFilterType[]=
{
	"TestName",
	"X123",
	"test",
	"532324335454656565757",
	"xxx",
	"",
	0
};


//bad applications names used for the certclient
const char *badcertclient_array[]=
{
	"\"XXXXXXXXXXXXX\"",
	"\"@#$@#%%#^%$^$%\"",
	"\"   \"",
	"0",
	0
};

//bad app names used for swi certstore
const char *badSwiCert_array[]=
{
	"\"Test Install\"",
	"\"XXXXXXXXXXXXX\"",
	"\"@#$@#%%#^%$^$%\"",
	"\"   \"",
	"0",
	0
};


const char *decimalUid_array[]= 
{
	"268452523", //uid of SWInstall
	"268478646", //Uid of SW Install OCSP Signing
	"270506792", //Uid of Midlet Installation
	"268441661",  //Uid of Server Authentication
	0
};

const char *badDatafilePath[]=
{
	"c:\\test",
	"!:\\@:$%#$",
	" ",
	0
};

// End of file
