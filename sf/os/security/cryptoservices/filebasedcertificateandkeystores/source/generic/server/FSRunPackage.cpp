/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include "FSRunPackage.h"

CRunPackage::~CRunPackage()
	{
	TInt count = iDeleteActions.Count();
	for (TInt i = 0; i < count; ++i)
		{
		delete iDeleteActions[i];
		}

	iDeleteActions.Close();
	}

// Adds a new destructor action - only called by AddDeleteOnly, AddCloseDelete, and
// AddReleaseOnly methods
TInt CRunPackage::AddDeleteAction(CBase* aAction)
	{
	TInt err = KErrNone;
	if (!aAction)
		{
		err = KErrNoMemory;
		}
	else
		{
		err = iDeleteActions.Append(aAction);

		if (err)
			{
			// delete the action if we couldn't add
			delete aAction;
			}
		}
	return err;
	}
