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

#ifndef MEMSPYSERVERDATA_H
#define MEMSPYSERVERDATA_H

// TMemSpyProcess data class holds data to be sent to the UI
class TMemSpyServerData 
	{	
public:
	//constructor & destructor
	inline TMemSpyServerData()
		: iProcessId(0), iThreadId(0), iSessionCount(0)
		{
		}
	
public:
    TProcessId iProcessId;
    TThreadId iThreadId;
    TFullName iName;
    TInt iSessionCount;
	};

#endif // MEMSPYSERVERDATA_H
