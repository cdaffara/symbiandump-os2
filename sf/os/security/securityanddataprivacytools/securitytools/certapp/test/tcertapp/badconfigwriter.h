#ifndef __BADCONFIGWRITER_H__
#define __BADCONFIGWRITER_H__/*
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

/**
Class to create file to write the configuration inputs for store creation
*/
class BadConfigWriter
	{
public:
	BadConfigWriter(const std::stringstream &aFileName);
	~BadConfigWriter();
protected:
	std::ofstream iFile;
	};

class FileBadCertStoreConfigWriter : public BadConfigWriter
	{
public:
	FileBadCertStoreConfigWriter(const std::stringstream &aFileName);
	~FileBadCertStoreConfigWriter();

	void WriteFileEntry(const char *aBadLabel= 0,
					const char *aBadDeletable = 0,
					const char *aBadFormat= 0,
					const char *aBadCertOwnerType = 0,
					const char *aBadSubjectKeyId =0,
					const char *aBadIssuerKeyId = 0,
					const char *aBadApplication = 0,
					const char *aBadTrusted = 0,
					const char *aBadInvalidPath = 0);
private:
	int iEntryCount;
	};

class FileBadCertClientConfigWriter : public BadConfigWriter
	{
public:
	FileBadCertClientConfigWriter(const std::stringstream &aFileName);
	~FileBadCertClientConfigWriter();

	void WriteCertClientName(const char *aArrayVal);
	void WriteCertClientUid(const char *aArrayVal);
	};

class SwiBadCertStoreConfigWriter : public BadConfigWriter
	{
public:
	SwiBadCertStoreConfigWriter(const std::stringstream &aFileName);
	~SwiBadCertStoreConfigWriter();

	void WriteSwiEntry(const char *aBadLabel= 0,
					const char *aBadFormat= 0,
					const char *aBadCertOwnerType = 0,
					const char *aBadSubjectKeyId =0,
					const char *aBadIssuerKeyId = 0,
					const char *aBadApplication = 0,
					const char *aBadTrusted = 0,
					const char *aBadDatafilePath= 0,
					const char *aBadCapabilitySet = 0,
					const char *aBadMandatory = 0,
					const char *aBadSystemUpgrade = 0);
private:
	int iEntryCount;
	};


#endif
