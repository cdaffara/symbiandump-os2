/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Contains the declaration of  TMemSpyProcessData class
*/

#ifndef MEMSPYPROCESSDATA_H
#define MEMSPYPROCESSDATA_H

#include <memspy/driver/memspydriverobjectsshared.h>

// TMemSpyProcess data class holds data to be sent to the UI
class TMemSpyProcessData 
	{	
public:
	//constructor & destructor
	inline TMemSpyProcessData()
		: iId(0), iThreadCount(0), iExitCategory(0), iExitReason(0)
		{
		}
	
public:
    TProcessId iId;        
    TFullName iName;
    TProcessPriority iPriority;
    TExitCategoryName iExitCategory;
    TInt iExitReason;
    TExitType iExitType;
    TInt iThreadCount;
    TUint32 iSID;
    TUint32 iVID;
    TBool iIsDead;
	};

#endif // MEMSPYPROCESSDATA_H
