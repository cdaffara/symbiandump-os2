/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : signalshutdown.h
* Part of     : Signal server
* 
*
*/



#ifndef SHUTDOWN_H
#define SHUTDOWN_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  CSigShutDown
 * 
 */
class CSigShutDown : public CTimer
	{
public:
	virtual ~CSigShutDown();
	static CSigShutDown* NewL();
	void Start();
	
private:
	CSigShutDown();
	void ConstructL();
	void RunL();
	};

#endif // SHUTDOWN_H
