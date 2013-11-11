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
*
*/


#include "badconfigwriter.h"
#include <iostream>
#include <sstream>
#include "tcertapp_bad.h"

// Array for the StatCA- contains all relevant details
const char *badstatCA_array[]=
	{
	"X576",
	"TEst",
	"9b843cbfXXXXXXXXX5a87ce5f6da5ec",
	"",
	"!!!",
	"Junk"
	};

// Array for the Root5CA- contains all relevant details
const char *badroot5CA_array[]=
	{
	"!@##",
	"T@$%$%Est",
	"@#$b843#%3cbfXXXXXXXXX5a87ce5f6da5ec",
	"",
	"!!!",
	"J&u$n%k"
	};

// Array for the SymbianTestDSACA- contains all relevant details
const char *badsymbianTestDSACA_array[]=
	{
	"X576",
	"TEst",
	"9b843cbfXXXXXXXXX5a87ce5f6da5ec",
	"",
	"!!!",
	"J&u$n%k"
	};

// Array for the SymbianTestRSACA- contains all relevant details
const char *badsymbianTestRSACA_array[]=
	{
	"X509",
	"ca",
	"7ac99da26d384fcf6ffd0578c4f50b549ffb2032",
	"",
	"584",
	"EFalse"
	};

// uids required for cacerts
const char *bademu_cacertsUid[]=
	{
	"26QWERER23",
	"5555555555"
	};

//Capabilities required for stataca
const char *aBadCapabilities_statca[]=
{
	"TCB",
	"CommDD",
	"#&%^&%^&",
	"MultimediaDD",
	"ReadDeviceData",
	"WriteDeviceData",
	"DRM",
	"TrustedUI",
	"ProtServ",
	"@$%%#$%$#",
	"NetworkControl",
	"XXXXXXX",
	"SwEvent",
	"NetworkServices",
	"LocalServices",
	"ReadUserData",
	"#&%^&%^&",
	"Location",
	"SurroundingsDD",
	"UserEnvironment"
};


//Capabilities required for root5ca
const char *aBadCapabilities_root5ca[]=
{
	"SurroundingsDD",	
	"XXXXXXX",
	"WriteDeviceData",
	"   ",
	"A#$2l%#4lF$%^iles"
};


//Capabilities required for SymbianTestDSACA
const char *aBadCapabilities_symbianTestDSACA[]=
{
	"NetworkServices",
	"  ",
	"%#^$$&$"
};

//Capabilities required for SymbianTestRSACA
const char *aBadCapabilities_symbianTestRSACA[]=
{
	"  ",
	"%#^$$&$"
};


/**
Bad store writer
*/
BadConfigWriter::BadConfigWriter(const std::stringstream &aFileName)
{
	std::string name = aFileName.str();
	iFile.open(name.c_str(), std::ios_base::trunc | std::ios_base::out);
	if(iFile.fail())
		{
		std::cout << "Failed to open '" << name.c_str()<< "' for output!" << std::endl;
		exit(-1);
		}
}

BadConfigWriter::~BadConfigWriter()
{
	iFile.close();
}

/**
Class definition for creating cacerts
*/
FileBadCertStoreConfigWriter::FileBadCertStoreConfigWriter(const std::stringstream &aFileName)
	: BadConfigWriter(aFileName), iEntryCount(0)
{
	iFile << "StartCertStoreEntries" << std::endl;
}

FileBadCertStoreConfigWriter::~FileBadCertStoreConfigWriter()
{
	iFile << "EndCertStoreEntries" << std::endl;
}

void FileBadCertStoreConfigWriter::WriteFileEntry(const char *aBadLabel,
										   const char *aBadDeletable,
										   const char *aBadFormat,
										   const char *aBadCertOwnerType,
										   const char *aBadSubjectKeyId,
										   const char *aBadIssuerKeyId,
										   const char *aBadApplication,
										   const char *aBadTrusted,
										   const char *aBadInvalidPath)
{
	++iEntryCount;
	// Setup default values
	std::stringstream certName;
	certName << "cert" << iEntryCount << ".der";

	if(!aBadLabel) aBadLabel = "\"TestLabel\"";
	if(!aBadDeletable) aBadDeletable = "\"true\"";
	if(!aBadFormat) aBadFormat = "\"EX509Certificate\"";
	if(!aBadCertOwnerType) aBadCertOwnerType = "\"ECACertificate\"";
	if(!aBadSubjectKeyId) aBadSubjectKeyId = "auto";
	if(!aBadIssuerKeyId) aBadIssuerKeyId = "auto";
	if(!aBadTrusted) aBadTrusted = "\"true\"";
	if(!aBadInvalidPath) aBadInvalidPath = "cert0.der";

	iFile << "\t# Entry " << iEntryCount << std::endl;
	iFile << "\tStartEntry " << aBadLabel << std::endl;
	iFile << "\t\tDeletable " << aBadDeletable << std::endl;
	iFile << "\t\tFormat " << aBadFormat << std::endl;
	iFile << "\t\tCertOwnerType " << aBadCertOwnerType << std::endl;
	iFile << "\t\tSubjectKeyId " << aBadSubjectKeyId << std::endl;
	iFile << "\t\tIssuerKeyId " << aBadIssuerKeyId << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	// write down the applications
	for(int i = 0; i<2; i++)
		{
		iFile << "\t\t\t# Entry " << i << std::endl;
		if(!aBadApplication)
			{
			iFile << "\t\t\t\tApplication " << appNames[i]<< std::endl;
			}
		else
			{
			iFile << "\t\t\t\tApplication " << badApplications[i]<< std::endl;
			}
		}

	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << aBadTrusted << std::endl;
	iFile << "\t\tDataFileName " << aBadInvalidPath << std::endl;
	iFile << "\tEndEntry " <<std::endl;
	iFile <<std::endl;
}


/**
Class definition for generating certclient files
*/
FileBadCertClientConfigWriter::FileBadCertClientConfigWriter(const std::stringstream &aFileName)
	: BadConfigWriter(aFileName)
{
	iFile << "StartClientInfo" << std::endl;
}

FileBadCertClientConfigWriter::~FileBadCertClientConfigWriter()
{
	iFile << "EndClientInfo" << std::endl;
}

void FileBadCertClientConfigWriter::WriteCertClientName(const char *aBadAppName)
{
	iFile << "\tName "<< aBadAppName << std::endl;
	
}

void FileBadCertClientConfigWriter::WriteCertClientUid(const char *aBadUid)
{
	iFile << "\tUid "<< aBadUid << std::endl;
}


/**
Swi certstore writer
*/
SwiBadCertStoreConfigWriter::SwiBadCertStoreConfigWriter(const std::stringstream &aFileName)
	: BadConfigWriter(aFileName), iEntryCount(0)
{
	iFile <<"StartSwiCertStoreEntries" << std::endl;
}

SwiBadCertStoreConfigWriter::~SwiBadCertStoreConfigWriter()
{
	iFile << "EndSwiCertStoreEntries" << std::endl;
}

void SwiBadCertStoreConfigWriter::WriteSwiEntry(const char *aBadLabel,
										   const char *aBadFormat,
										   const char *aBadCertOwnerType,
										   const char *aBadSubjectKeyId,
										   const char *aBadIssuerKeyId,
										   const char *aBadApplication,
										   const char *aBadTrusted,
										   const char *aBadDatafilePath,
										   const char *aBadCapabilitySets,
										   const char *aBadMandatory,
										   const char *aBadSystemUpgrade)
{
	++iEntryCount;
	
	if(!aBadLabel) aBadLabel = "TestLabel";
	if(!aBadFormat) aBadFormat = "EX509Certificate";
	if(!aBadCertOwnerType) aBadCertOwnerType = "ECACertificate";
	if(!aBadSubjectKeyId) aBadSubjectKeyId = "auto";
	if(!aBadIssuerKeyId) aBadIssuerKeyId = "auto";
	if(!aBadTrusted) aBadTrusted = "\"true\"";
	if(!aBadDatafilePath) aBadDatafilePath = "\"swicertstore_cert0.der\"";
	if(!aBadMandatory) aBadMandatory = "\"true\"";
	if(!aBadSystemUpgrade) aBadSystemUpgrade = "\"true\"";

	iFile << "\t#Entry " << iEntryCount << std::endl;
	iFile << "\tStartEntry " << aBadLabel << std::endl;
	iFile << "\t\tFormat " << aBadFormat << std::endl;
	iFile << "\t\tCertOwnerType " << aBadCertOwnerType << std::endl;
	iFile << "\t\tSubjectKeyId " << aBadSubjectKeyId << std::endl;
	iFile << "\t\tIssuerKeyId " << aBadIssuerKeyId << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	//write application
	for(int k = 0; k<2; k++)
		{
		iFile << "\t\t\t# Entry " << k << std::endl;
		if(!aBadApplication)
			{
			iFile << "\t\t\t\tApplication " << appNames[k]<< std::endl;
			}
		else
			{
			iFile << "\t\t\t\tApplication " << badApplications[k]<< std::endl;
			}
		}
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << aBadTrusted << std::endl;
	iFile << "\t\tDataFileName " << aBadDatafilePath << std::endl;
	iFile << "\t\tCapabilitySet " << "{ ";
	for(int i = 0; i< 5; i++)
		{
		if(!aBadCapabilitySets)
			{
			iFile << capabilitySets[i];
			}
		else
			{
			iFile << badCapabilitySets[i];
			}
		iFile <<" ";
		}

	iFile <<"}" <<std::endl;
	iFile << "\t\tMandatory " << aBadMandatory << std::endl;
	iFile << "\t\tSystemUpgrade " << aBadSystemUpgrade << std::endl;
	iFile << "\tEndEntry " <<std::endl;
	iFile <<std::endl;
}

// End of file

