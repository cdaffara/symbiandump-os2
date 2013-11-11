// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The definition for a plugin used for resolver perfromance tests
// 
//

#include "ImplementationProxy.h"
#include "TestUtilities.h"	// For __FILE__LINE__

// ____________________________________________________________________________
// 
class CImplementationClassExampleOne // codescanner::missingcclass
{
public:
	static CImplementationClassExampleOne* NewL(TAny* aInitParams);

	virtual ~CImplementationClassExampleOne();

private:
	CImplementationClassExampleOne();
	void ConstructL(TAny* aInitParams);

private:
};  // End of CImplementationClassExampleOne definition

