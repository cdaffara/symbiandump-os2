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


#include "goodconfigwriter.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "tcertapp_good.h"

// Array for the StatCA- holds all relevant details
const char *swicertemu_array[]=
	{
	"X509",
	"c7014670ad9c8ac296c2ae665c4e78f3d4df4a99"
	};

const char *cert_array[]=
	{
	"X509",
	"ca",
	"7ce306295116207214d425affd185b6d5e48af6f",
	"",
	"1"
	};

// uids required for cacerts
const char *emu_cacertsUid[]=
	{
	"268452523",
	"268478646"
	};


/**
Class required to create files to write data
*/
GoodConfigWriter::GoodConfigWriter(const std::stringstream &aFileName)
{
	std::string name = aFileName.str();
	iFile.open(name.c_str(), std::ios_base::trunc | std::ios_base::out);
	if(iFile.fail())
		{
		std::cout << "Failed to open '" << name.c_str()<< "' for output!" << std::endl;
		exit(-1);
		}
}

GoodConfigWriter::~GoodConfigWriter()
{
	iFile.close();
}

/**
Class definition for creating cacerts config files
*/
FileCertStoreConfigWriter::FileCertStoreConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iEntryCount(0)
{
	iFile << "StartCertStoreEntries" << std::endl;
}

FileCertStoreConfigWriter::~FileCertStoreConfigWriter()
{
	iFile << "EndCertStoreEntries" << std::endl;
}

void FileCertStoreConfigWriter::WriteExtraFileEntry()
{
	iFile << "\tStartEntry " << "\"AugmentData\"" << std::endl;
	iFile << "\t\tDeletable " << "\"true\"" << std::endl;
	iFile << "\t\tFormat " << "\"EX509Certificate\"" << std::endl;
	iFile << "\t\tCertOwnerType " << "\"ECACertificate\"" << std::endl;
	iFile << "\t\tSubjectKeyId " << "auto" << std::endl;
	iFile << "\t\tIssuerKeyId " << "auto" << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	iFile << "\t\t\t#Entry 1" << std::endl;
	iFile << "\t\t\t\tApplication " << "\"0x100042ab\"" << std::endl;
	iFile << "\t\t\t\tApplication " << "\"0x1000a8b6\"" << std::endl;
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << "\"true\"" << std::endl;
	iFile << "\t\tDataFileName " << "\"cert0.der\"" << std::endl;
	iFile <<"\tEndEntry" <<std::endl;
}

void FileCertStoreConfigWriter::WriteFileEntry(const char *aGoodLabel,
										   const char *aGoodDeletable,
										   const char *aGoodFormat,
										   const char *aGoodCertOwnerType,
										   const char *aGoodSubjectKeyId,
										   const char *aGoodIssuerKeyId,
										   const char *aGoodApplication,
										   const char *aGoodTrusted,
										   const char *aGoodDataFileName)
{
	++iEntryCount;
	// Setup default values
	if(!aGoodDeletable) 
	{
	aGoodDeletable = "\"true\"";
	}
	if(!aGoodFormat) aGoodFormat = "\"EX509Certificate\"";
	if(!aGoodCertOwnerType) aGoodCertOwnerType = "\"ECACertificate\"";
	if(!aGoodSubjectKeyId) aGoodSubjectKeyId = "auto";
	if(!aGoodIssuerKeyId) aGoodIssuerKeyId = "auto";
	if(!aGoodApplication) aGoodApplication = "\"Server Authentication\"";
	if(!aGoodTrusted) aGoodTrusted = "\"true\"";
	
	iFile << "\t# Entry " << iEntryCount << std::endl;
	iFile << "\tStartEntry " << aGoodLabel <<std::endl;
	iFile << "\t\tDeletable " << aGoodDeletable << std::endl;
	iFile << "\t\tFormat " << aGoodFormat << std::endl;
	iFile << "\t\tCertOwnerType " << aGoodCertOwnerType << std::endl;
	iFile << "\t\tSubjectKeyId " << aGoodSubjectKeyId << std::endl;
	iFile << "\t\tIssuerKeyId " << aGoodIssuerKeyId << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	// write down the applications
	for(int i = 0; i<2; i++)
		{
		iFile << "\t\t\t# Entry " << i << std::endl;
		iFile << "\t\t\t\tApplication " << goodApplications[i]<< std::endl;
		}
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << aGoodTrusted << std::endl;
	iFile << "\t\tDataFileName " << aGoodDataFileName << std::endl;
	iFile <<"\tEndEntry" <<std::endl;
}

/**
Class definition for generating certclient configuration files
*/
FileCertClientConfigWriter::FileCertClientConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName),iEntryCount(0)
{
	iFile << "StartClientInfo" << std::endl;
}

FileCertClientConfigWriter::~FileCertClientConfigWriter()
{
	iFile << "EndClientInfo" << std::endl;
}

void FileCertClientConfigWriter::WriteCertClientName(const char *aGoodAppName)
{
	iFile << "\t\tName "<< aGoodAppName << std::endl;
}

void FileCertClientConfigWriter::WriteCertClientUid(const char *aGoodUid)
{
	++iEntryCount;
	iFile << "\t#Entry "<< iEntryCount << std::endl;
	iFile << "\t\tUid "<< aGoodUid << std::endl;
}

void FileCertClientConfigWriter::WriteExtraCertClientEntry()
{
	iFile << "\t#Entry "<< iEntryCount << std::endl;
	iFile << "\t\tUid "<< "0x12345678" << std::endl;
	iFile << "\t\tName "<< "Augment_Label" << std::endl;
}

/**
Swi certstore config writer
*/
SwiCertStoreConfigWriter::SwiCertStoreConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iEntryCount(0)
{
	iFile <<"StartSwiCertStoreEntries" << std::endl;
}

SwiCertStoreConfigWriter::~SwiCertStoreConfigWriter()
{
	iFile << "EndSwiCertStoreEntries" << std::endl;
}

void SwiCertStoreConfigWriter::WriteExtraSwiEntry()
{
	iFile << "\tStartEntry " << "\"AugmentData\"" << std::endl;
	iFile << "\t\tFormat " << "\"EX509Certificate\"" << std::endl;
	iFile << "\t\tCertOwnerType " << "\"ECACertificate\"" << std::endl;
	iFile << "\t\tSubjectKeyId " << "auto" << std::endl;
	iFile << "\t\tIssuerKeyId " << "auto" << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	iFile << "\t\t\t# Entry 1" << std::endl;
	iFile << "\t\t\t\tApplication " << "0x1000aaaa" << std::endl;
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << "true" << std::endl;
	iFile << "\t\tDataFileName " << "\"swicertstore_cert0.der\"" << std::endl;
	iFile << "\t\tCapabilitySet " << "{ TCB LocalServices }";
	iFile << "\t\tMandatory " << "true" << std::endl;
	iFile << "\t\tSystemUpgrade " << "false"<< std::endl;
	iFile <<"\tEndEntry" <<std::endl;
}


void SwiCertStoreConfigWriter::WriteSwiEntry(const char *aGoodLabel,
										   const char *aGoodFormat,
										   const char *aGoodCertOwnerType,
										   const char *aGoodSubjectKeyId,
										   const char *aGoodIssuerKeyId,
										   const char *aGoodApplication,
										   const char *aGoodTrusted,
										   const char *aGoodCapabilitySets,
										   const char *aGoodMandatory,
										   const char *aGoodSystemUpgrade)
{
	++iEntryCount;
	// Setup default values
	if(!aGoodFormat) aGoodFormat = "\"EX509Certificate\"";
	if(!aGoodCertOwnerType) aGoodCertOwnerType = "ECACertificate";
	if(!aGoodSubjectKeyId) aGoodSubjectKeyId = "auto";
	if(!aGoodIssuerKeyId) aGoodIssuerKeyId = "auto";
	if(!aGoodApplication) aGoodApplication = "\"Server Authentication\"";
	if(!aGoodTrusted) aGoodTrusted = "\"true\"";
	if(!aGoodCapabilitySets) aGoodCapabilitySets = "TCB";
	if(!aGoodMandatory) aGoodMandatory = "\"true\"";
	if(!aGoodSystemUpgrade) aGoodSystemUpgrade = "\"true\"";

	iFile << "\t#Entry " << iEntryCount << std::endl;
	iFile << "\tStartEntry " << aGoodLabel << std::endl;
	iFile << "\t\tFormat " << aGoodFormat << std::endl;
	iFile << "\t\tCertOwnerType " << aGoodCertOwnerType << std::endl;
	iFile << "\t\tSubjectKeyId " << aGoodSubjectKeyId << std::endl;
	iFile << "\t\tIssuerKeyId " << aGoodIssuerKeyId << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	//write application
	for(int k = 0; k<2 ; k++)
		{
		iFile << "\t\t\t# Entry " << k << std::endl;
		iFile << "\t\t\t\tApplication " << goodUids[k] << std::endl;
		}
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << aGoodTrusted << std::endl;
	iFile << "\t\tDataFileName " << "\"swicertstore_cert0.der\"" << std::endl;
	iFile << "\t\tCapabilitySet " << "{ ";
	for(int i = 0; i< 20; i++)
		{
		iFile << goodCapabilitySets[i];
		iFile <<" ";
		}
	iFile <<"}" <<std::endl;
	iFile << "\t\tMandatory " << aGoodMandatory << std::endl;
	iFile << "\t\tSystemUpgrade " << aGoodSystemUpgrade << std::endl;
	iFile << "\tEndEntry" << std::endl;
}


//Script and INI file generator
ScriptAndIniGeneration::ScriptAndIniGeneration(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName)
{
	int last_dot = aFileName.str().rfind('.');
	if(last_dot>=0)
	{
		iIniFileName = aFileName.str().substr(0,last_dot);
		iIniFileName.append(".ini");
		iIniFile.open(iIniFileName.c_str(), std::ios_base::trunc | std::ios_base::out);
		if(iIniFile.fail())
		{
			std::cout << "Failed to open '" << iIniFileName.c_str()<< "' for output!" << std::endl;
			exit(-1);
		}
	}	
}

ScriptAndIniGeneration::~ScriptAndIniGeneration()
{
	iIniFile.close();
}

void ScriptAndIniGeneration::WriteTestCaseToScript(const std::stringstream &aTestCaseType,int &aTestIndex,const char *aTestActionName,const char *aTestActionType,bool aHasActionBody)
{
	// set test case ID string
	char testCaseIndexBuffer[6];
	sprintf(testCaseIndexBuffer, "-%04d", aTestIndex);

	iFile << "START_TESTCASE                " << aTestCaseType.str() << testCaseIndexBuffer << std::endl;
	iFile << "//! @SYMTestCaseID            " << aTestCaseType.str() << testCaseIndexBuffer << std::endl;
	iFile << "//! @SYMTestCaseDesc          " << aTestActionName << std::endl;
	iFile << "RUN_TEST_STEP                 -1\tCTestHandler\t" << aTestActionType;
	if(aHasActionBody)
	{
		iFile << "\t" << iIniFileName << "\t" << aTestCaseType.str() << testCaseIndexBuffer << "-001";
		iIniFile << "[" << aTestCaseType.str() << testCaseIndexBuffer << "-001" << "]" << std::endl;
	}
	iFile << std::endl;
	iFile << "END_TESTCASE                  " << aTestCaseType.str() << testCaseIndexBuffer << std::endl;
	iFile << std::endl;
}


FileStoreScriptGeneration::FileStoreScriptGeneration(const std::stringstream &aFileName)
	: ScriptAndIniGeneration(aFileName)
{
	
}

FileStoreScriptGeneration::~FileStoreScriptGeneration()
{
}

void FileStoreScriptGeneration:: WriteInitialiseCert(const char *aMode, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Initializing a CUnifiedCertStore";
	const char *testcasetype = "init";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<mode>" << aMode << "</mode>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << std::endl;
}



void  FileStoreScriptGeneration::WriteListcert(const char *aGoodOwnerType, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get the list of certificates";
	const char *testcasetype = "listcert";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<filter>" << std::endl;
	iIniFile << "\t\t<ownertype>" << aGoodOwnerType << "</ownertype>" << std::endl;
	iIniFile << "\t</filter>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	for(int z =0; z<6; z++)
		{
		iIniFile << "\t<CCTCertInfo><label>" << goodEmuCert_array[z] << "</label></CCTCertInfo>" << std::endl;
		}
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}

void  FileStoreScriptGeneration::WriteGetCertificateDetails(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get certificate details";
	const char *testcasetype = "listcert";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	int i = 0;
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<filter>" << std::endl;
	iIniFile << "\t\t<label>" << label << "</label>" << std::endl;
	iIniFile << "\t\t<format>" << cert_array[i++]<< "</format>" << std::endl;
	iIniFile << "\t\t<certowner>" << cert_array[i++] << "</certowner>" << std::endl;
	iIniFile << "\t\t<subjectkeyid>" << cert_array[i++] << "</subjectkeyid>" << std::endl;
	iIniFile << "\t\t<issuerkeyid>" <<cert_array[i++] << "</issuerkeyid>" << std::endl;
	iIniFile << "\t\t<deletable>" << cert_array[i++] << "</deletable>" << std::endl;
	iIniFile << "\t</filter>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<CCTCertInfo><label>" << label << "</label></CCTCertInfo>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


void  FileStoreScriptGeneration::WriteGetTrust(const char *label, const char *trust, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get Trust certificate";
	const char *testcasetype = "gettrusters";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<trust>" << trust << "</trust>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


void  FileStoreScriptGeneration::WriteGetApplications(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get applications";
	const char *testcasetype = "getapplications";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<uid>";
	for(int j = 0; j<2; j++)
		{
		iIniFile << emu_cacertsUid[j] << " ";
		}
	iIniFile << "</uid>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


void  FileStoreScriptGeneration::WriteRetrieveCerts(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Retrieve Certificate";
	const char *testcasetype = "retrieve";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << std::endl;
}



//Swi store script generator for emulator tests
SWIStoreScriptGeneration::SWIStoreScriptGeneration(const std::stringstream &aFileName)
	: ScriptAndIniGeneration(aFileName)
{
	
}

SWIStoreScriptGeneration::~SWIStoreScriptGeneration()
{
}

void SWIStoreScriptGeneration:: WriteInitialiseCert(const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Initialise a SWICertStore";
	const char *testcasetype = "initswicertstore";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype, false);
}


void  SWIStoreScriptGeneration::WriteListcert(const char *aGoodOwnerType, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get the list of certificates";
	const char *testcasetype = "listcert";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<filter>" << std::endl;
	iIniFile << "\t\t<ownertype>" << aGoodOwnerType << "</ownertype>" << std::endl;
	iIniFile << "\t</filter>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	for(int i =0; i<6; i++)
		{
		iIniFile << "\t<CCTCertInfo><label>" << goodSwiCert_array[i] << "</label><readonly>True</readonly></CCTCertInfo>" << std::endl;
		}
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}

void  SWIStoreScriptGeneration::WriteGetSystemUpgrade(const char *label, const char *aSystemUpgrade, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get the systemupgrade flag";
	const char *testcasetype = "getsystemupgrade";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<systemupgrade>" << aSystemUpgrade << "</systemupgrade>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


void  SWIStoreScriptGeneration::WriteRetrieveCerts(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Retrieve Certificate";
	const char *testcasetype = "retrieve";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << std::endl;
}



void  SWIStoreScriptGeneration::WriteGetApplications(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get applications";
	const char *testcasetype = "getapplications";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<uid>";
	for(int j = 0; j<2; j++)
		{
		iIniFile << emu_cacertsUid[j] << " ";
		}
	iIniFile << "</uid>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


void  SWIStoreScriptGeneration::WriteGetTrust(const char *label, const char *trust, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get Trust certificate";
	const char *testcasetype = "gettrusters";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<trust>" << trust << "</trust>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}



void  SWIStoreScriptGeneration::WriteGetCapabilities(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get the capabilities";
	const char *testcasetype = "getcapabilities";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	for(int i=0; i<20; i++)
		{
		iIniFile << "\t<capability>" << goodCapabilitySets[i] << "</capability>" << std::endl;
		}
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


void  SWIStoreScriptGeneration::WriteGetMandatoryFlag(const char *label, const char *aMandatory, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get the mandatory flag";
	const char *testcasetype = "getmandatory";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<label>" << label << "</label>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << "<actionresult>" << std::endl;
	iIniFile << "\t<mandatory>" << aMandatory << "</mandatory>" << std::endl;
	iIniFile << "</actionresult>" << std::endl;
	iIniFile << std::endl;
}


//Cert client script generator
CertClientsStoreScriptGeneration::CertClientsStoreScriptGeneration(const std::stringstream &aFileName)
	: ScriptAndIniGeneration(aFileName)
{
}

CertClientsStoreScriptGeneration::~CertClientsStoreScriptGeneration()
{
}

void CertClientsStoreScriptGeneration::WriteInitialiseCertClient(const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Initialise a CertClientStore";
	const char *testcasetype = "InitManager";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype, false);
}

void CertClientsStoreScriptGeneration::WriteGetCount(const int aApp_uidIndex, const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Get Count of Applications";
	const char *testcasetype = "AppCount";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<count>" << aApp_uidIndex << "</count>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << std::endl;
}


void CertClientsStoreScriptGeneration::WriteGetApplicationsList(const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Getting the application list";
	const char *testcasetype = "GetApplications";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	for(int i = 0 ; i<4 ; i++)
		{
		iIniFile << "\t<uid>" << gooddecimalUid_array[i] << "</uid>"<<"<appname>" << goodcertclient_array[i] << "</appname>" << std::endl;
		}
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << std::endl;
}


void CertClientsStoreScriptGeneration::WriteGetAppWithUid(const char *goodlabel,const char *uid,const std::stringstream &aTestCaseType,int &aTestIndex)
{
	const char *testcasename = "Get application with given id";
	const char *testcasetype = "GetApp";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype);
	iIniFile << "<actionbody>" << std::endl;
	iIniFile << "\t<uid>" << uid << "</uid>" << std::endl;
	iIniFile << "\t<appname>" << goodlabel << "</appname>" << std::endl;
	iIniFile << "</actionbody>" << std::endl;
	iIniFile << std::endl;
}


void CertClientsStoreScriptGeneration::WriteDestroyManager(const std::stringstream &aTestCaseType, int &aTestIndex)
{
	const char *testcasename = "Destroy the manager";
	const char *testcasetype = "DestroyManager";
	WriteTestCaseToScript(aTestCaseType, ++aTestIndex, testcasename, testcasetype, false);
}


/**
Class definition for creating cacerts
*/
EmptyFileConfigWriter::EmptyFileConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName)
{
	iFile << "StartCertStoreEntries" << std::endl;
}

EmptyFileConfigWriter::~EmptyFileConfigWriter()
{
	iFile << "EndCertStoreEntries" << std::endl;
}

// End of file

