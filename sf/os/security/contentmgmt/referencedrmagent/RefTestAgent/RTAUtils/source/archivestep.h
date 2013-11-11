/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalAll
 @released
*/

#ifndef __ARCHIVESTEP_H__
#define __ARCHIVESTEP_H__

#include "rtautilsstep.h"

// Constants used to name test cases 
_LIT(KCreateDrmArchive,"CreateDrmArchive");
_LIT(KDumpDrmArchive,"DumpDrmArchive");
_LIT(KImportDrmArchive,"ImportDrmArchive");
_LIT(KMultipartContent, "MultipartContent");

class CRTAUtilsServer;

class CCreateDrmArchive : public CRTAUtilsStep
	{
public:
	CCreateDrmArchive(CRTAUtilsServer& aParent);
	~CCreateDrmArchive();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

class CDumpDrmArchive : public CRTAUtilsStep
	{
public:
	CDumpDrmArchive(CRTAUtilsServer& aParent);
	~CDumpDrmArchive();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

class CImportDrmArchive : public CRTAUtilsStep
	{
public:
	CImportDrmArchive(CRTAUtilsServer& aParent);
	~CImportDrmArchive();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

class CMultipartContent : public CRTAUtilsStep
	{
public:
	CMultipartContent(CRTAUtilsServer& aParent);
	~CMultipartContent();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

#endif
