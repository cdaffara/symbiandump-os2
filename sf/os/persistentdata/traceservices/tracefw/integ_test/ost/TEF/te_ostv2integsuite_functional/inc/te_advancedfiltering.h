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
 @file te_advancedfiltering.h
 @internalTechnology
*/
#if (!defined TE_ULOGGERADPAPITESTSTEP_H)
#define TE_ULOGGERADPAPITESTSTEP_H
#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>
#include "te_dataparameters.h"
#include "te_perftracetester.h"
using namespace Ulogger; //CA:added so can use old ulogger api


/*should put in a general filter test step class do derive the primary and secondary tests from*/
/*pass the filter values, have get/setfilter(0) as members*/
/*derive the relavent primary/secondary specifics*/

class CTCMAdvancedGroupIDFilteringWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMAdvancedGroupIDFilteringWrapper();
	static CTCMAdvancedGroupIDFilteringWrapper* NewL();
	static CTCMAdvancedGroupIDFilteringWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
	TVerdict TestGroupIdFilteringL(RULogger& aLogger, const TBool aFilter, const TBool aFiltering);

private:
	CTCMAdvancedGroupIDFilteringWrapper();
	void ConstructL();
	};
	
class CTCMAdvancedComponentIDFilteringWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMAdvancedComponentIDFilteringWrapper();
	static CTCMAdvancedComponentIDFilteringWrapper* NewL();
	static CTCMAdvancedComponentIDFilteringWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
	TVerdict TestComponentIdFilteringL(RULogger& aLogger, const TBool aFilter, const TBool aFiltering);
	
// Please add/modify your class members here:
private:
	CTCMAdvancedComponentIDFilteringWrapper();
	void ConstructL();
	
private:
    TUptTraceTester iTestTracer;
    TApiRunConfig iApiRunConfig;
	};
	

_LIT(KTCMAdvancedGroupIDFilteringWrapper,"TCMAdvancedGroupIDFiltering");
_LIT(KTCMAdvancedComponentIDFilteringWrapper,"TCMAdvancedComponentIDFiltering");

#endif
