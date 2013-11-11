// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file te_configfile.h
 @internalTechnology
*/
#if (!defined TE_ULOGGERCONFIGFILESTEP_H)
#define TE_ULOGGERCONFIGFILESTEP_H
#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
using namespace Ulogger; //CA:added so can use old ulogger api


class CPublicConfigFileWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CPublicConfigFileWrapper();
	static CPublicConfigFileWrapper* NewL();
	static CPublicConfigFileWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
	static TInt aTestCount;
private:
	CPublicConfigFileWrapper();
	void ConstructL();
	};
	
class CPrivateConfigFileWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CPrivateConfigFileWrapper();
	static CPrivateConfigFileWrapper* NewL();
	static CPrivateConfigFileWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
	
private:
	CPrivateConfigFileWrapper();
	void ConstructL();

	};
	
_LIT(KPublicConfigFileWrapper,"PublicConfigFile");
_LIT(KPrivateConfigFileWrapper,"PrivateConfigFile");


#endif
