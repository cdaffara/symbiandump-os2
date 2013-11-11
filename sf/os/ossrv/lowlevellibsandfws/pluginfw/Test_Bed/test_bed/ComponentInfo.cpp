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
//

#include "ComponentInfo.h"


CComponentInfo::CComponentInfo()
: CBase()
	{
	}


CComponentInfo::~CComponentInfo()
	{
	iComponentTesterGlobalCreationFuncLC = NULL;
	// We took ownership of these objects at construction, therefore we delete them
	if(iUnitTestsInfo)
		{
		iUnitTestsInfo->ResetAndDestroy();
		delete iUnitTestsInfo;
		}
	}


CComponentInfo* CComponentInfo::NewLC(ComponentTesterInitialiserLC aEntryFunc, RPointerArray<CUnitTestInfo>* aUnitTestsInfo)
	{
	CComponentInfo* self = new (ELeave) CComponentInfo();
	CleanupStack::PushL(self);

	// There must be no Leave'able functions following the Construct
  	// as we take control of the objects on exit.
	self->Construct(aEntryFunc, aUnitTestsInfo);
	return self;
	}


EXPORT_C CComponentInfo* CComponentInfo::NewL(ComponentTesterInitialiserLC aEntryFunc, RPointerArray<CUnitTestInfo>* aUnitTestsInfo)
	{
	CComponentInfo* self = NewLC(aEntryFunc, aUnitTestsInfo);
	CleanupStack::Pop();
	return self;
	}


void CComponentInfo::Construct(ComponentTesterInitialiserLC aEntryFunc, RPointerArray<CUnitTestInfo>* aUnitTestsInfo)
   	{
 	// NOTE these must be the final lines of code. 
 	// There must be NO Leave'able code following this
 	// as we take control of these objects on exit.
	iComponentTesterGlobalCreationFuncLC = aEntryFunc;
	iUnitTestsInfo  = aUnitTestsInfo;
   	}
  

