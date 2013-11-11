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

#include <ecom/implementationproxy.h>


// ____________________________________________________________________________
// 
class CImplementationLoadMgrExample1 // codescanner::missingcclass
{
public:
	static CImplementationLoadMgrExample1* NewL(TAny* aInitParams);

	virtual ~CImplementationLoadMgrExample1();

private:
	CImplementationLoadMgrExample1();
	void ConstructL(TAny* aInitParams);

private:
};  // End of CImplementationLoadMgrExample1
