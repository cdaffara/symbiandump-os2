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
 @file te_filepluginerr.h
 @internalTechnology
*/

#ifndef __TE_ULOGGERFILEPLUGINTESTERR_H__
#define __TE_ULOGGERFILEPLUGINTESTERR_H__
#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
using namespace Ulogger; //CA:added so can use old ulogger api

//negative testing
class CTCMFilePluginErrWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMFilePluginErrWrapper();
	static CTCMFilePluginErrWrapper* NewL();
	static CTCMFilePluginErrWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMFilePluginErrWrapper();
	void ConstructL();
	};
	
_LIT(KTCMFilePluginErrWrapper,"TCMFilePluginErr");

#endif

