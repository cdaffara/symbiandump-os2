// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The context for the tests upon the CDiscoverer class
// 
//

#include "DiscovererObserver.h"
#include "DiscovererTransitions.h"
#include "DiscovererObserverStub.h"

// ______________________________________________________________________________
//
CDiscoverer_UnitTestContext::CDiscoverer_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver,
												RFs& aFs)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver),	iFs(aFs)
	{
	//Do nothing
	}

CDiscoverer_UnitTestContext::~CDiscoverer_UnitTestContext()
	{
	delete iDiscoverer;
	delete iDiscovererObserver;
	delete iDir;
	delete iReturnedFileEntry;
	}

MDiscovererObserver& CDiscoverer_UnitTestContext::DiscovererObserver()
	{
	return *STATIC_CAST(MDiscovererObserver*, iDiscovererObserver);
	}

