#ifndef __CONFIGWRITER_H__
#define __CONFIGWRITER_H__/*
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


#include <string>
#include <fstream>
#include <sstream>

class GoodConfigWriter
	{
public:
	GoodConfigWriter(const std::stringstream &aFileName);
	~GoodConfigWriter();
	std::ofstream iFile;
	};

class FileCertStoreConfigWriter : public GoodConfigWriter
	{
public:
	FileCertStoreConfigWriter(const std::stringstream &aFileName);
	~FileCertStoreConfigWriter();

	void WriteFileEntry(const char *aGoodLabel= "\"Root5CA\"",
					const char *aGoodDeletable = "\"true\"",
					const char *aGoodFormat= "\"EX509Certificate\"",
					const char *aGoodCertOwnerType = "\"ECACertificate\"",
					const char *aGoodSubjectKeyId ="auto",
					const char *aGoodIssuerKeyId = "auto",
					const char *aGoodApplication = "\"SW Install\"",
					const char *aGoodTrusted = "\"true\"",
					const char *aGoodDataFileName  = "\"cert0.der\"");
	void WriteExtraFileEntry();
	
private:
	int iEntryCount;
	};

class FileCertClientConfigWriter : public GoodConfigWriter
	{
public:
	FileCertClientConfigWriter(const std::stringstream &aFileName);
	~FileCertClientConfigWriter();

	void WriteCertClientName(const char *arrayVal);
	void WriteCertClientUid(const char *arrayVal);
	void WriteExtraCertClientEntry();
private:
	int iEntryCount;
	};


class SwiCertStoreConfigWriter : public GoodConfigWriter
	{
public:
	SwiCertStoreConfigWriter(const std::stringstream &aFileName);
	~SwiCertStoreConfigWriter();

	void WriteSwiEntry(const char *aGoodLabel= "\"certificate_Label\"",
					/*const char *aGoodDeletable = "\"true\"",*/
					const char *aGoodFormat= "EX509Certificate",
					const char *aGoodCertOwnerType = "\"ECACertificate\"",
					const char *aGoodSubjectKeyId ="auto",
					const char *aGoodIssuerKeyId = "auto",
					const char *aGoodApplication = "\"SW Install\"",
					const char *aGoodTrusted = "\"true\"",
					const char *aGoodCapabilitySet = "TCB",
					const char *aGoodMandatory = "\"true\"",
					const char *aGoodSystemUpgrade = "\"true\"");
	void WriteExtraSwiEntry();
private:
	int iEntryCount;
	};


// to generate script and ini files to run tests on emulator

class ScriptAndIniGeneration : public GoodConfigWriter
	{
public:
	ScriptAndIniGeneration(const std::stringstream &aFileName);
	~ScriptAndIniGeneration();
	
	void WriteTestCaseToScript(const std::stringstream &aTestCaseType, int &aTestIndex, const char *aTestActionName, const char *aTestActionType, bool aHasActionBody = true);

protected:
	std::ofstream iIniFile;
	std::string   iIniFileName;
	};


// to generate script files for the file certstore to run tests on emulator

class FileStoreScriptGeneration : public ScriptAndIniGeneration
	{
public:
	FileStoreScriptGeneration(const std::stringstream &aFileName);
	~FileStoreScriptGeneration();
	
	void WriteInitialiseCert(const char *aMode, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteListcert(const char *aGoodOwnerType, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetCertificateDetails(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetTrust(const char *label, const char *trust, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetApplications(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteRetrieveCerts(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex);
	};


// to generate script files for the cert cleint store to run tests on emulator

class CertClientsStoreScriptGeneration : public ScriptAndIniGeneration
	{
public:
	CertClientsStoreScriptGeneration(const std::stringstream &aFileName);
	~CertClientsStoreScriptGeneration();
	
	void WriteInitialiseCertClient(const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetCount(const int index, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetApplicationsList(const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetAppWithUid(const char *label, const char *uid, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteDestroyManager(const std::stringstream &aTestCaseType, int &aTestIndex);
	};


// to generate script files for the swicertstore to run tests on emulator

class SWIStoreScriptGeneration : public ScriptAndIniGeneration
	{
public:
	SWIStoreScriptGeneration(const std::stringstream &aFileName);
	~SWIStoreScriptGeneration();
	
	void WriteInitialiseCert(const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteListcert(const char *aGoodOwnerType, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetSystemUpgrade(const char *label , const char *aSystemUpgrade, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetCapabilities(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetMandatoryFlag(const char *label, const char *aMandatory, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetTrust(const char *label, const char *trust, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteGetApplications(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex);
	void WriteRetrieveCerts(const char *label, const std::stringstream &aTestCaseType, int &aTestIndex);
	};

class EmptyFileConfigWriter : public GoodConfigWriter
	{
public:
	EmptyFileConfigWriter(const std::stringstream &aFileName);
	~EmptyFileConfigWriter();
	};

#endif
