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

#ifndef __RIGHTSDBSTEP_H__
#define __RIGHTSDBSTEP_H__

#include "rtautilsstep.h"

// Constants used to name test cases 
_LIT(KClearRightsDb,"ClearRightsDb");
_LIT(KDumpRightsDb,"DumpRightsDb");
_LIT(KLoadOldDb,"LoadOldDb");

class CRTAUtilsServer;

class CClearRightsDb : public CRTAUtilsStep
	{
public:
	CClearRightsDb(CRTAUtilsServer& aParent);
	~CClearRightsDb();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

class CDumpRightsDb: public CRTAUtilsStep
	{
public:
	CDumpRightsDb(CRTAUtilsServer& aParent);
	~CDumpRightsDb();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

class CLoadOldDb: public CRTAUtilsStep
	{
public:
	CLoadOldDb(CRTAUtilsServer& aParent);
	~CLoadOldDb();
	virtual TVerdict doTestStepL();
	
private:
	CRTAUtilsServer& iParent;
	};

#endif
