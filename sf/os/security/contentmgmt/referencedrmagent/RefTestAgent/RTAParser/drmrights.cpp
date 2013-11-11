/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "drmrights.h"
#include "virtualFile.h"
#include "rightsobject.h"
#include <caf/caftypes.h>

using namespace ReferenceTestAgent;


CDrmRights* CDrmRights::NewL()
	{
	CDrmRights* self = new (ELeave) CDrmRights;
	return self;
	}

CDrmRights::CDrmRights()
	{
	}

CDrmRights::~CDrmRights()
	{
	iRightsArray.ResetAndDestroy();
	}

EXPORT_C CRightsObject& CDrmRights::Rights(TInt aIndex)
	{
	return *iRightsArray[aIndex];	
	}
	
EXPORT_C TInt CDrmRights::RightsCount()
	{
	return iRightsArray.Count();
	}

EXPORT_C void CDrmRights::Remove(TInt aIndex)
	{
	CRightsObject* rights = iRightsArray[aIndex];
	delete rights;
	rights = NULL;
	iRightsArray.Remove(aIndex);
	}

void CDrmRights::AddL(CRightsObject* aRightsObject)
	{
	User::LeaveIfError(iRightsArray.Append(aRightsObject));
	}
