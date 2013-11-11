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
// The definition for plugin used for resolver performance tests
// 
//

#include <ecom/implementationproxy.h>
#include "TestUtilities.h"	// For __FILE__LINE__

// ____________________________________________________________________________
// 
class CImplementationClassExampleTwo // codescanner::missingcclass
{
public:
	static CImplementationClassExampleTwo* NewL(TAny* aInitParams);

	virtual ~CImplementationClassExampleTwo();

private:
	CImplementationClassExampleTwo();

	void ConstructL(TAny* aInitParams);

};  // End of CImplementationClassExampleTwo definition



