// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration of test active object for log engine testing 
// Identical declaration exists in common\syslibs\logeng\ongoing\test\src\test.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TESTACTIVE_H
#define TESTACTIVE_H

#include <e32base.h>

class CTestActive : public CActive
	{
public:
	CTestActive(TInt aPriority = EPriorityIdle-100);
	~CTestActive();
	void Start();
	void Start(TInt);
protected:
	void DoCancel();
	void RunL();
private:
	TBool iDelayCompletion;
	TRequestStatus iStoredStatus;
	TInt iDelayTime;
	};

#endif // #ifndef TESTACTIVE_H
