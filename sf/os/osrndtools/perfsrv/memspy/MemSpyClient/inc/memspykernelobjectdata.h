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

#ifndef MEMSPYKERNELOBJECTDATA_H
#define MEMSPYKERNELOBJECTDATA_H

#include <memspy/driver/memspydriverenumerationsshared.h>

// Constants
const TInt KBufSize = 128;

// TMemSpyProcess data class holds data to be sent to the UI
class TMemSpyKernelObjectData 
	{	
public:
	//constructor & destructor
	inline TMemSpyKernelObjectData()
		: iCount(0), iSize(0)
		{
		}
	
public:    
    TMemSpyDriverContainerType iType;
    TFullName iName;
    TInt iCount;
    TInt64 iSize;
	};

#endif // MEMSPYKERNELOBJECTDATA_H
