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

#ifndef MEMSPYTHREADINFOITEMDATA_H
#define MEMSPYTHREADINFOITEMDATA_H

#include <memspy/driver/memspydriverobjectsshared.h>

// TMemSpyProcess data class holds data to be sent to the UI
class TMemSpyThreadInfoItemData 
	{	
public:
	//constructor & destructor
	inline TMemSpyThreadInfoItemData()		
		{
		}
	
public:
    TBuf<64> iCaption;
    TBuf<32> iValue;
	};

#endif // MEMSPYTHREADINFOITEMDATA_H
