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
class CImplementationLoadMgrExample2 // codescanner::missingcclass
{
public:
	static CImplementationLoadMgrExample2* NewL(TAny* aInitParams);

	virtual ~CImplementationLoadMgrExample2();

private:
	CImplementationLoadMgrExample2();
	void ConstructL(TAny* aInitParams);

private:
};  // End of CImplementationLoadMgrExample2
