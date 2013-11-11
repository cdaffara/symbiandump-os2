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
//

#include <ecom/implementationproxy.h>


// ____________________________________________________________________________
// 
class CImplementationHashExample // codescanner::missingcclass
{
public:
	static CImplementationHashExample* NewL(TAny* aInitParams);

	virtual ~CImplementationHashExample();

private:
	CImplementationHashExample();
	void ConstructL(TAny* aInitParams);

private:
};  // End of CCImplementationHashExample
